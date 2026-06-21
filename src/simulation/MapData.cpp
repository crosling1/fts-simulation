#include "simulation/MapData.h"

#include <cctype>
#include <cstdint>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
struct JsonValue {
    enum class Type : std::uint8_t {
        Number,
        String,
        Array,
        Object,
    };

    Type type;
    double number = 0.0;
    std::string text;
    std::vector<JsonValue> array;
    std::map<std::string, JsonValue> object;
};

class JsonParser {
  public:
    explicit JsonParser(std::string source) : source_(std::move(source)) {}

    JsonValue Parse(void) {
        JsonValue value = ParseValue();
        SkipWhitespace();
        if (position_ != source_.size()) {
            Fail("unexpected trailing JSON content");
        }
        return value;
    }

  private:
    std::string source_;
    std::size_t position_ = 0;

    JsonValue ParseValue(void) {
        SkipWhitespace();
        if (position_ >= source_.size()) {
            Fail("unexpected end of JSON");
        }

        const char current = source_[position_];
        if (current == '{') {
            return ParseObject();
        }
        if (current == '[') {
            return ParseArray();
        }
        if (current == '"') {
            return JsonValue{JsonValue::Type::String, 0.0, ParseString(), {}, {}};
        }
        if (current == '-' || std::isdigit(static_cast<unsigned char>(current))) {
            return JsonValue{JsonValue::Type::Number, ParseNumber(), "", {}, {}};
        }

        Fail("unsupported JSON value");
    }

    JsonValue ParseObject(void) {
        Expect('{');
        JsonValue value = {JsonValue::Type::Object, 0.0, "", {}, {}};
        SkipWhitespace();
        if (Consume('}')) {
            return value;
        }

        while (true) {
            SkipWhitespace();
            const std::string key = ParseString();
            SkipWhitespace();
            Expect(':');
            value.object[key] = ParseValue();
            SkipWhitespace();
            if (Consume('}')) {
                return value;
            }
            Expect(',');
        }
    }

    JsonValue ParseArray(void) {
        Expect('[');
        JsonValue value = {JsonValue::Type::Array, 0.0, "", {}, {}};
        SkipWhitespace();
        if (Consume(']')) {
            return value;
        }

        while (true) {
            value.array.push_back(ParseValue());
            SkipWhitespace();
            if (Consume(']')) {
                return value;
            }
            Expect(',');
        }
    }

    std::string ParseString(void) {
        Expect('"');
        std::string result;
        while (position_ < source_.size()) {
            const char current = source_[position_++];
            if (current == '"') {
                return result;
            }
            if (current == '\\') {
                if (position_ >= source_.size()) {
                    Fail("unterminated JSON escape");
                }
                const char escaped = source_[position_++];
                if (escaped == '"' || escaped == '\\' || escaped == '/') {
                    result.push_back(escaped);
                } else if (escaped == 'n') {
                    result.push_back('\n');
                } else if (escaped == 't') {
                    result.push_back('\t');
                } else {
                    Fail("unsupported JSON escape");
                }
            } else {
                result.push_back(current);
            }
        }

        Fail("unterminated JSON string");
    }

    double ParseNumber(void) {
        const std::size_t start = position_;
        if (source_[position_] == '-') {
            position_++;
        }
        while (position_ < source_.size() &&
               std::isdigit(static_cast<unsigned char>(source_[position_]))) {
            position_++;
        }
        if (position_ < source_.size() && source_[position_] == '.') {
            position_++;
            while (position_ < source_.size() &&
                   std::isdigit(static_cast<unsigned char>(source_[position_]))) {
                position_++;
            }
        }

        return std::stod(source_.substr(start, position_ - start));
    }

    void SkipWhitespace(void) {
        while (position_ < source_.size() &&
               std::isspace(static_cast<unsigned char>(source_[position_]))) {
            position_++;
        }
    }

    bool Consume(char expected) {
        if (position_ < source_.size() && source_[position_] == expected) {
            position_++;
            return true;
        }
        return false;
    }

    void Expect(char expected) {
        if (!Consume(expected)) {
            Fail(std::string("expected '") + expected + "'");
        }
    }

    [[noreturn]] void Fail(const std::string& message) const {
        throw std::runtime_error("Invalid JSON near byte " + std::to_string(position_) + ": " +
                                 message);
    }
};

const JsonValue& RequireField(const JsonValue& object, const std::string& key) {
    if (object.type != JsonValue::Type::Object) {
        throw std::runtime_error("Expected JSON object while reading field: " + key);
    }

    const auto iterator = object.object.find(key);
    if (iterator == object.object.end()) {
        throw std::runtime_error("Missing JSON field: " + key);
    }

    return iterator->second;
}

const std::vector<JsonValue>& RequireArray(const JsonValue& value, const std::string& name) {
    if (value.type != JsonValue::Type::Array) {
        throw std::runtime_error("Expected JSON array: " + name);
    }
    return value.array;
}

double Number(const JsonValue& value, const std::string& name) {
    if (value.type != JsonValue::Type::Number) {
        throw std::runtime_error("Expected JSON number: " + name);
    }
    return value.number;
}

int IntegerField(const JsonValue& object, const std::string& key) {
    return static_cast<int>(Number(RequireField(object, key), key));
}

float FloatField(const JsonValue& object, const std::string& key) {
    return static_cast<float>(Number(RequireField(object, key), key));
}

Vector2 ReadVector(const JsonValue& value) {
    return {
        FloatField(value, "x"),
        FloatField(value, "y"),
    };
}

Rectangle ReadRectangle(const JsonValue& value) {
    return {
        FloatField(value, "x"),
        FloatField(value, "y"),
        FloatField(value, "width"),
        FloatField(value, "height"),
    };
}

Vector2 ReadNavigationNode(const JsonValue& value, const MapData& data) {
    if (value.object.find("ref") != value.object.end()) {
        const JsonValue& ref = RequireField(value, "ref");
        if (ref.type == JsonValue::Type::String && ref.text == "robot_start") {
            return data.robotStart;
        }
        if (ref.type == JsonValue::Type::String && ref.text == "charging_station_dock") {
            return data.chargingStation.dockPoint;
        }
        throw std::runtime_error("Unsupported navigation node ref");
    }

    if (value.object.find("dock") != value.object.end()) {
        const std::size_t dockIndex =
            static_cast<std::size_t>(Number(RequireField(value, "dock"), "dock"));
        if (dockIndex >= data.dockPoints.size()) {
            throw std::runtime_error("Navigation dock index is out of range");
        }
        return data.dockPoints[dockIndex];
    }

    return ReadVector(value);
}

void ValidateMapData(const MapData& data) {
    if (data.roads.empty()) {
        throw std::runtime_error("Map data requires at least one road");
    }
    if (data.warehouses.size() != data.dockPoints.size()) {
        throw std::runtime_error("Warehouse and dock counts must match");
    }
    if (data.pickupLagerIndex < 0 ||
        static_cast<std::size_t>(data.pickupLagerIndex) >= data.warehouses.size()) {
        throw std::runtime_error("Pickup lager index is out of range");
    }
    if (data.deliveryLagerIndex < 0 ||
        static_cast<std::size_t>(data.deliveryLagerIndex) >= data.warehouses.size()) {
        throw std::runtime_error("Delivery lager index is out of range");
    }
    if (data.navigationNodes.empty() || data.navigationEdges.empty()) {
        throw std::runtime_error("Navigation nodes and edges are required");
    }
}
} // namespace

MapData LoadMapData(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open map data file: " + path);
    }

    const std::string source((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    const JsonValue root = JsonParser(source).Parse();

    MapData data = {};
    const JsonValue& screen = RequireField(root, "screen");
    data.screenWidth = IntegerField(screen, "width");
    data.screenHeight = IntegerField(screen, "height");
    data.gridSize = IntegerField(root, "gridSize");
    data.pickupLagerIndex = IntegerField(root, "pickupLager");
    data.deliveryLagerIndex = IntegerField(root, "deliveryLager");
    data.robotStart = ReadVector(RequireField(root, "robotStart"));
    const JsonValue& chargingStation = RequireField(root, "chargingStation");
    data.chargingStation = {
        ReadRectangle(RequireField(chargingStation, "body")),
        ReadVector(RequireField(chargingStation, "dockPoint")),
    };

    for (const JsonValue& road : RequireArray(RequireField(root, "roads"), "roads")) {
        data.roads.push_back(ReadRectangle(road));
    }

    for (const JsonValue& warehouse :
         RequireArray(RequireField(root, "warehouses"), "warehouses")) {
        data.warehouses.push_back(ReadRectangle(warehouse));
    }

    for (const JsonValue& dockPoint :
         RequireArray(RequireField(root, "dockPoints"), "dockPoints")) {
        data.dockPoints.push_back(ReadVector(dockPoint));
    }

    for (const JsonValue& roadLine : RequireArray(RequireField(root, "roadLines"), "roadLines")) {
        data.roadLines.push_back({
            ReadVector(RequireField(roadLine, "from")),
            ReadVector(RequireField(roadLine, "to")),
            FloatField(roadLine, "width"),
        });
    }

    for (const JsonValue& node :
         RequireArray(RequireField(root, "navigationNodes"), "navigationNodes")) {
        data.navigationNodes.push_back(ReadNavigationNode(node, data));
    }

    for (const JsonValue& edge :
         RequireArray(RequireField(root, "navigationEdges"), "navigationEdges")) {
        const std::vector<JsonValue>& values = RequireArray(edge, "navigation edge");
        if (values.size() != 2) {
            throw std::runtime_error("Navigation edge must contain two node indexes");
        }
        data.navigationEdges.push_back({
            static_cast<std::size_t>(Number(values[0], "edge from")),
            static_cast<std::size_t>(Number(values[1], "edge to")),
        });
    }

    for (const JsonValue& blockingPath :
         RequireArray(RequireField(root, "blockingRobotPaths"), "blockingRobotPaths")) {
        BlockingRobotPath pathData = {};
        pathData.speedMultiplier = FloatField(blockingPath, "speedMultiplier");
        for (const JsonValue& point :
             RequireArray(RequireField(blockingPath, "points"), "blocking path points")) {
            pathData.points.push_back(ReadVector(point));
        }
        data.blockingRobotPaths.push_back(pathData);
    }

    ValidateMapData(data);
    return data;
}

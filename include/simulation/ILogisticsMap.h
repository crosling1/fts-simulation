#pragma once

#include "simulation/MapData.h"
#include "raylib.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

enum class LagerId : std::uint8_t { L1 = 0, L2, L3, L4, L5, L6, Count };

[[nodiscard]] inline bool isValid(LagerId id) noexcept {
    return static_cast<std::size_t>(id) < static_cast<std::size_t>(LagerId::Count);
}

class ILogisticsMap {
  public:
    virtual ~ILogisticsMap() = default;

    [[nodiscard]] virtual std::optional<Vector2> getLagerDockPosition(LagerId lagerId) const = 0;
    [[nodiscard]] virtual std::optional<Vector2> getPickupDockPosition() const = 0;
    [[nodiscard]] virtual std::optional<Vector2> getDeliveryDockPosition() const = 0;
    [[nodiscard]] virtual Vector2 getChargingStationDockPosition() const = 0;
    [[nodiscard]] virtual bool isRoadPosition(Vector2 position) const = 0;
    [[nodiscard]] virtual Vector2 clampPositionToRoad(Vector2 position) const = 0;
    [[nodiscard]] virtual const std::vector<Vector2>& getNavigationNodes() const = 0;
    [[nodiscard]] virtual const std::vector<NavigationEdge>& getNavigationEdges() const = 0;
};

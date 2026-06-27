#pragma once

#include "simulation/ILogisticsMap.h"

#include <array>
#include <optional>
#include <utility>
#include <vector>

class MockLogisticsMap : public ILogisticsMap {
  public:
    MockLogisticsMap() {
        navigationNodes_ = {{0.0f, 0.0f}, {10.0f, 0.0f}, {20.0f, 0.0f}};
        navigationEdges_ = {{0, 1}, {1, 2}};
        pickupDockPosition_ = navigationNodes_[1];
        deliveryDockPosition_ = navigationNodes_[2];
        chargingStationDockPosition_ = navigationNodes_[2];
    }

    void setLagerDockPosition(LagerId lagerId, std::optional<Vector2> position) {
        if (!isValid(lagerId)) {
            return;
        }

        lagerDockPositions_[static_cast<std::size_t>(lagerId)] = position;
    }

    void setPickupDockPosition(std::optional<Vector2> position) {
        pickupDockPosition_ = position;
    }

    void setDeliveryDockPosition(std::optional<Vector2> position) {
        deliveryDockPosition_ = position;
    }

    void setChargingStationDockPosition(Vector2 position) {
        chargingStationDockPosition_ = position;
    }

    void setNavigationGraph(std::vector<Vector2> nodes, std::vector<NavigationEdge> edges) {
        navigationNodes_ = std::move(nodes);
        navigationEdges_ = std::move(edges);
    }

    [[nodiscard]] std::optional<Vector2> getLagerDockPosition(LagerId lagerId) const override {
        if (!isValid(lagerId)) {
            return std::nullopt;
        }

        return lagerDockPositions_[static_cast<std::size_t>(lagerId)];
    }

    [[nodiscard]] std::optional<Vector2> getPickupDockPosition() const override {
        return pickupDockPosition_;
    }

    [[nodiscard]] std::optional<Vector2> getDeliveryDockPosition() const override {
        return deliveryDockPosition_;
    }

    [[nodiscard]] Vector2 getChargingStationDockPosition() const override {
        return chargingStationDockPosition_;
    }

    [[nodiscard]] bool isRoadPosition(Vector2 /*position*/) const override {
        return true;
    }

    [[nodiscard]] Vector2 clampPositionToRoad(Vector2 position) const override {
        return position;
    }

    [[nodiscard]] const std::vector<Vector2>& getNavigationNodes() const override {
        return navigationNodes_;
    }

    [[nodiscard]] const std::vector<NavigationEdge>& getNavigationEdges() const override {
        return navigationEdges_;
    }

  private:
    std::array<std::optional<Vector2>, static_cast<std::size_t>(LagerId::Count)>
        lagerDockPositions_{};
    std::optional<Vector2> pickupDockPosition_;
    std::optional<Vector2> deliveryDockPosition_;
    Vector2 chargingStationDockPosition_{};
    std::vector<Vector2> navigationNodes_;
    std::vector<NavigationEdge> navigationEdges_;
};

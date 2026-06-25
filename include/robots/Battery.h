#pragma once

class Battery {
  public:
    explicit Battery(float chargePercentage = 100.0f);

    [[nodiscard]] float getChargePercentage() const noexcept;
    [[nodiscard]] bool isFull() const noexcept;
    [[nodiscard]] bool isEmpty() const noexcept;
    [[nodiscard]] bool isLow(float thresholdPercentage) const noexcept;

    void drain(float percentage) noexcept;
    void charge(float percentage) noexcept;
    void setChargePercentage(float percentage) noexcept;

  private:
    float chargePercentage_;
};

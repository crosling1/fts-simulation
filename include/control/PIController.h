#pragma once

class PIController {
  public:
    struct Config {
        float proportionalGain = 0.0f;
        float integralGain = 0.0f;
        float maxIntegralError = 1000.0f;
    };

    struct UpdateInput {
        float error;
        float deltaTime;
        float maxOutput;
    };

    PIController();
    explicit PIController(Config config);

    [[nodiscard]] float update(UpdateInput input) noexcept;
    void reset() noexcept;

  private:
    Config config_;
    float errorIntegral_ = 0.0f;
};

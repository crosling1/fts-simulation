#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

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

    float update(UpdateInput input);
    void reset();

  private:
    Config config_;
    float errorIntegral_ = 0.0f;
};

#endif // PI_CONTROLLER_H

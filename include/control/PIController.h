#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

class PIController {
  public:
    struct Config {
        float proportionalGain = 0.0f;
        float integralGain = 0.0f;
        float maxIntegralError = 1000.0f;
    };

    explicit PIController(Config config = {});

    float update(float error, float deltaTime, float maxOutput);
    void reset();

  private:
    Config config_;
    float errorIntegral_ = 0.0f;
};

#endif // PI_CONTROLLER_H

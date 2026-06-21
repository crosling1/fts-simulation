#ifndef BATTERY_H
#define BATTERY_H

class Battery {
  public:
    explicit Battery(float chargePercentage = 100.0f);

    float getChargePercentage(void) const;
    bool isFull(void) const;
    bool isEmpty(void) const;
    bool isLow(float thresholdPercentage) const;

    void drain(float percentage);
    void charge(float percentage);
    void setChargePercentage(float percentage);

  private:
    float chargePercentage_;
};

#endif // BATTERY_H

#ifndef RPM_H
#define RPM_H

#include <Arduino.h>
#include <defs.h>

struct RpmReading {
  int raw;
  int rpm;
};

class RpmControl {
public:
  explicit RpmControl(uint8_t analogPin = RPM_ANALOG_PIN, int minRpm = RPM_MIN_VALUE, int maxRpm = RPM_MAX_VALUE,
                      int updateThreshold = RPM_UPDATE_THRESHOLD)
      : analogPin_(analogPin), minRpm_(minRpm), maxRpm_(maxRpm), updateThreshold_(updateThreshold) {}

  void begin() const {
    pinMode(analogPin_, INPUT);
  }

  int readRaw() const {
    return analogRead(analogPin_);
  }

  int readRpm() const {
    return rawToRpm(readRaw());
  }

  RpmReading read() const {
    const int raw = readRaw();
    return {raw, rawToRpm(raw)};
  }

  bool update() {
    const RpmReading currentReading = read();
    lastReading_ = currentReading;

    if (!hasStableReading_ || abs(currentReading.rpm - stableRpm_) >= updateThreshold_) {
      stableRpm_ = currentReading.rpm;
      hasStableReading_ = true;
      return true;
    }

    return false;
  }

  int rpm() const {
    return stableRpm_;
  }

  RpmReading lastReading() const {
    return lastReading_;
  }

private:
  int rawToRpm(int raw) const {
    raw = constrain(raw, 0, 1023);
    return static_cast<long>(raw) * (maxRpm_ - minRpm_) / 1023 + minRpm_;
  }

  uint8_t analogPin_;
  int minRpm_;
  int maxRpm_;
  int updateThreshold_;
  int stableRpm_ = RPM_MIN_VALUE;
  bool hasStableReading_ = false;
  RpmReading lastReading_ = {0, RPM_MIN_VALUE};
};

#endif

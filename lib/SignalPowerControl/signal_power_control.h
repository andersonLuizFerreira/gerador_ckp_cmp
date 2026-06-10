#ifndef SIGNAL_POWER_CONTROL_H
#define SIGNAL_POWER_CONTROL_H

#include <Arduino.h>
#include <defs.h>

enum class SignalPowerSource {
  Internal5V,
  External
};

struct SignalPowerPins {
  uint8_t ckpRelayPin;
  uint8_t cmpRelayPin;
};

class SignalPowerControl {
public:
  SignalPowerControl()
      : pins_({CKP_POWER_RELAY_PIN, CMP_POWER_RELAY_PIN}) {}

  explicit SignalPowerControl(const SignalPowerPins &pins)
      : pins_(pins) {}

  void begin() {
    pinMode(pins_.ckpRelayPin, OUTPUT);
    pinMode(pins_.cmpRelayPin, OUTPUT);
    setCkpPower(SignalPowerSource::Internal5V);
    setCmpPower(SignalPowerSource::Internal5V);
  }

  void setCkpPower(SignalPowerSource source) {
    ckpSource_ = source;
    digitalWrite(pins_.ckpRelayPin, relayLevelFor(source));
  }

  void setCmpPower(SignalPowerSource source) {
    cmpSource_ = source;
    digitalWrite(pins_.cmpRelayPin, relayLevelFor(source));
  }

  void configure(SignalPowerSource ckpSource, SignalPowerSource cmpSource) {
    setCkpPower(ckpSource);
    setCmpPower(cmpSource);
  }

  SignalPowerSource ckpSource() const {
    return ckpSource_;
  }

  SignalPowerSource cmpSource() const {
    return cmpSource_;
  }

private:
  static uint8_t relayLevelFor(SignalPowerSource source) {
    return source == SignalPowerSource::External ? HIGH : LOW;
  }

  SignalPowerPins pins_;
  SignalPowerSource ckpSource_ = SignalPowerSource::Internal5V;
  SignalPowerSource cmpSource_ = SignalPowerSource::Internal5V;
};

#endif

#ifndef SIGNAL_ENGINE_H
#define SIGNAL_ENGINE_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <defs.h>
#include <signal_power_control.h>

enum class SignalOutputType {
  Hall,
  Inductive
};

struct SignalPins {
  uint8_t ckpPlusPin;
  uint8_t ckpNegPin;
  uint8_t cmpPlusPin;
  uint8_t cmpNegPin;
};

struct CkpConfig {
  uint16_t toothCount;
  uint8_t missingToothCount;
  uint8_t cycleRepeatCount;
  SignalOutputType outputType;
  uint8_t missingLevel;

  SignalPowerSource powerSource;

  CkpConfig(uint16_t toothCount = CKP_DEFAULT_TOOTH_COUNT,
            uint8_t missingToothCount = CKP_DEFAULT_MISSING_TOOTH_COUNT,
            uint8_t cycleRepeatCount = CKP_DEFAULT_CYCLE_REPEAT_COUNT,
            SignalOutputType outputType = SignalOutputType::Hall,
            uint8_t missingLevel = CKP_DEFAULT_MISSING_LEVEL,
            SignalPowerSource powerSource = SignalPowerSource::Internal5V)
      : toothCount(toothCount),
        missingToothCount(missingToothCount),
        cycleRepeatCount(cycleRepeatCount),
        outputType(outputType),
        missingLevel(missingLevel),
        powerSource(powerSource) {}
};

struct CmpConfig {
  SignalOutputType outputType;
  const uint8_t *eventTable;
  uint8_t initialLevel;

  SignalPowerSource powerSource;

  CmpConfig(SignalOutputType outputType = SignalOutputType::Hall,
            const uint8_t *eventTable = nullptr,
            uint8_t initialLevel = CMP_DEFAULT_INITIAL_LEVEL,
            SignalPowerSource powerSource = SignalPowerSource::Internal5V)
      : outputType(outputType),
        eventTable(eventTable),
        initialLevel(initialLevel),
        powerSource(powerSource) {}
};

class SignalEngine {
public:
  SignalEngine()
      : pins_({CKP_PLUS_PIN, CKP_NEG_PIN, CMP_PLUS_PIN, CMP_NEG_PIN}),
        ckp_({CKP_DEFAULT_TOOTH_COUNT, CKP_DEFAULT_MISSING_TOOTH_COUNT, CKP_DEFAULT_CYCLE_REPEAT_COUNT,
              SignalOutputType::Hall, CKP_DEFAULT_MISSING_LEVEL}),
        cmp_({SignalOutputType::Hall, nullptr, CMP_DEFAULT_INITIAL_LEVEL}) {}

  void begin() {
    pinMode(pins_.ckpPlusPin, OUTPUT);
    pinMode(pins_.ckpNegPin, OUTPUT);
    pinMode(pins_.cmpPlusPin, OUTPUT);
    pinMode(pins_.cmpNegPin, OUTPUT);
    reset();
  }

  void setPins(const SignalPins &pins) {
    pins_ = pins;
  }

  void configureCkp(const CkpConfig &config) {
    ckp_ = normalizedConfig(config);
    reset();
  }

  void configureCmp(const CmpConfig &config) {
    cmp_ = normalizedConfig(config);
    resetCmp();
  }

  void reset() {
    tickIndex_ = 0;
    ottoTickIndex_ = 0;
    writeCkpIdle();
    resetCmp();
  }

  void tick() {
    tickCkp();
    tickCmp();

    tickIndex_++;
    if (tickIndex_ >= ticksPerRevolution()) {
      tickIndex_ = 0;
    }

    ottoTickIndex_++;
    if (ottoTickIndex_ >= ticksPerOttoCycle()) {
      ottoTickIndex_ = 0;
    }
  }

  uint32_t ticksPerCycle() const {
    return static_cast<uint32_t>(ckp_.toothCount) * ticksPerTooth_;
  }

  uint32_t ticksPerRevolution() const {
    return ticksPerCycle() * ckp_.cycleRepeatCount;
  }

  uint32_t ticksPerOttoCycle() const {
    return ticksPerRevolution() * 2;
  }

  uint16_t currentCkpTick() const {
    return tickIndex_;
  }

  uint16_t currentOttoTick() const {
    return ottoTickIndex_;
  }

  uint16_t currentCmpDegree() const {
    return cmpDegreeForTick(ottoTickIndex_);
  }

  CkpConfig ckpConfig() const {
    return ckp_;
  }

  CmpConfig cmpConfig() const {
    return cmp_;
  }

  SignalPins pins() const {
    return pins_;
  }

private:
  void tickCkp() {
    const uint16_t ticksInCycle = ticksPerCycle();
    const uint16_t tickInCycle = tickIndex_ % ticksInCycle;
    const uint16_t toothIndex = tickInCycle / ticksPerTooth_;
    const uint8_t toothPhase = tickInCycle % ticksPerTooth_;
    const uint16_t activeToothCount = ckp_.toothCount - ckp_.missingToothCount;

    if (toothIndex >= activeToothCount) {
      writeCkpMissing();
    } else if (ckp_.outputType == SignalOutputType::Hall) {
      writeCkpHall(toothPhase);
    } else {
      writeCkpInductive(toothPhase);
    }
  }

  static CkpConfig normalizedConfig(CkpConfig config) {
    if (config.toothCount == 0) {
      config.toothCount = 1;
    }

    if (config.missingToothCount > config.toothCount) {
      config.missingToothCount = config.toothCount;
    }

    if (config.cycleRepeatCount == 0) {
      config.cycleRepeatCount = 1;
    }

    config.missingLevel = config.missingLevel == HIGH ? HIGH : LOW;
    return config;
  }

  static CmpConfig normalizedConfig(CmpConfig config) {
    config.initialLevel = config.initialLevel == HIGH ? HIGH : LOW;
    return config;
  }

  void resetCmp() {
    cmpHallState_ = cmp_.initialLevel;
    cmpInductivePhase_ = 0;

    if (cmp_.outputType == SignalOutputType::Hall) {
      digitalWrite(pins_.cmpPlusPin, cmpHallState_);
      digitalWrite(pins_.cmpNegPin, LOW);
      return;
    }

    writeCmpIdle();
  }

  void tickCmp() {
    if (cmp_.eventTable == nullptr) {
      writeCmpIdle();
      return;
    }

    const bool event = cmpEventAtDegree(currentCmpDegree());

    if (cmp_.outputType == SignalOutputType::Hall) {
      if (event) {
        cmpHallState_ = cmpHallState_ == HIGH ? LOW : HIGH;
        digitalWrite(pins_.cmpPlusPin, cmpHallState_);
      }

      digitalWrite(pins_.cmpNegPin, LOW);
      return;
    }

    tickCmpInductive(event);
  }

  void tickCmpInductive(bool event) {
    if (cmpInductivePhase_ == 1) {
      digitalWrite(pins_.cmpPlusPin, LOW);
      digitalWrite(pins_.cmpNegPin, HIGH);
      cmpInductivePhase_ = 0;
      return;
    }

    if (event) {
      digitalWrite(pins_.cmpPlusPin, HIGH);
      digitalWrite(pins_.cmpNegPin, LOW);
      cmpInductivePhase_ = 1;
      return;
    }

    writeCmpIdle();
  }

  bool cmpEventAtDegree(uint16_t degree) const {
    const uint8_t byteIndex = degree / 8;
    const uint8_t bitIndex = degree % 8;
    const uint8_t mask = 0x80 >> bitIndex;
    const uint8_t value = pgm_read_byte(&cmp_.eventTable[byteIndex]);

    return (value & mask) != 0;
  }

  uint16_t cmpDegreeForTick(uint16_t ottoTick) const {
    return static_cast<uint32_t>(ottoTick) * CMP_CYCLE_DEGREES / ticksPerOttoCycle();
  }

  void writeCkpHall(uint8_t toothPhase) const {
    digitalWrite(pins_.ckpPlusPin, toothPhase == 0 ? HIGH : LOW);
    digitalWrite(pins_.ckpNegPin, LOW);
  }

  void writeCkpInductive(uint8_t toothPhase) const {
    digitalWrite(pins_.ckpPlusPin, toothPhase == 0 ? HIGH : LOW);
    digitalWrite(pins_.ckpNegPin, toothPhase == 0 ? LOW : HIGH);
  }

  void writeCkpMissing() const {
    if (ckp_.outputType == SignalOutputType::Hall) {
      digitalWrite(pins_.ckpPlusPin, ckp_.missingLevel);
      digitalWrite(pins_.ckpNegPin, LOW);
      return;
    }

    writeCkpIdle();
  }

  void writeCkpIdle() const {
    digitalWrite(pins_.ckpPlusPin, LOW);
    digitalWrite(pins_.ckpNegPin, LOW);
  }

  void writeCmpIdle() const {
    digitalWrite(pins_.cmpPlusPin, LOW);
    digitalWrite(pins_.cmpNegPin, LOW);
  }

  static const uint8_t ticksPerTooth_ = 2;

  SignalPins pins_;
  CkpConfig ckp_;
  CmpConfig cmp_;
  uint16_t tickIndex_ = 0;
  uint16_t ottoTickIndex_ = 0;
  uint8_t cmpHallState_ = LOW;
  uint8_t cmpInductivePhase_ = 0;
};

#endif

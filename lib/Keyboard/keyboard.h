#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Arduino.h>

enum class KeyboardKey {
  None,
  Right,
  Up,
  Down,
  Left,
  Select
};

struct KeyboardReading {
  KeyboardKey key;
  int raw;
  int millivolts;
};

class AnalogKeyboard {
public:
  explicit AnalogKeyboard(uint8_t analogPin = A1, unsigned long debounceMs = 40, int referenceMillivolts = 5000)
      : analogPin_(analogPin), debounceMs_(debounceMs), referenceMillivolts_(referenceMillivolts) {}

  void begin() const {
    pinMode(analogPin_, INPUT);
  }

  int readRaw() const {
    return analogRead(analogPin_);
  }

  int readMillivolts() const {
    return rawToMillivolts(readRaw());
  }

  KeyboardReading read() const {
    const int raw = readRaw();
    const int millivolts = rawToMillivolts(raw);
    return {keyFromMillivolts(millivolts), raw, millivolts};
  }

  KeyboardKey readKey() const {
    return read().key;
  }

  KeyboardKey update() {
    const KeyboardReading currentReading = read();
    const KeyboardKey currentKey = currentReading.key;
    const unsigned long now = millis();

    lastReading_ = currentReading;

    if (currentKey != lastReadKey_) {
      lastReadKey_ = currentKey;
      lastChangeMs_ = now;
    }

    if ((now - lastChangeMs_) < debounceMs_) {
      return KeyboardKey::None;
    }

    if (stableKey_ == currentKey) {
      return KeyboardKey::None;
    }

    stableKey_ = currentKey;
    return stableKey_;
  }

  KeyboardReading lastReading() const {
    return lastReading_;
  }

private:
  static const int rightThresholdMillivolts = 500;
  static const int upThresholdMillivolts = 1000;
  static const int downThresholdMillivolts = 2000;
  static const int leftThresholdMillivolts = 3000;
  static const int selectThresholdMillivolts = 4000;

  static KeyboardKey keyFromMillivolts(int millivolts) {
    if (millivolts < rightThresholdMillivolts) {
      return KeyboardKey::Right;
    }
    if (millivolts < upThresholdMillivolts) {
      return KeyboardKey::Up;
    }
    if (millivolts < downThresholdMillivolts) {
      return KeyboardKey::Down;
    }
    if (millivolts < leftThresholdMillivolts) {
      return KeyboardKey::Left;
    }
    if (millivolts < selectThresholdMillivolts) {
      return KeyboardKey::Select;
    }

    return KeyboardKey::None;
  }

  int rawToMillivolts(int raw) const {
    return static_cast<long>(raw) * referenceMillivolts_ / 1023;
  }

  uint8_t analogPin_;
  unsigned long debounceMs_;
  int referenceMillivolts_;
  KeyboardKey lastReadKey_ = KeyboardKey::None;
  KeyboardKey stableKey_ = KeyboardKey::None;
  KeyboardReading lastReading_ = {KeyboardKey::None, 1023, 5000};
  unsigned long lastChangeMs_ = 0;
};

inline const char *keyboardKeyName(KeyboardKey key) {
  switch (key) {
    case KeyboardKey::Right:
      return "RIGHT";
    case KeyboardKey::Up:
      return "UP";
    case KeyboardKey::Down:
      return "DOWN";
    case KeyboardKey::Left:
      return "LEFT";
    case KeyboardKey::Select:
      return "SELECT";
    case KeyboardKey::None:
    default:
      return "NONE";
  }
}

#endif

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
    return {keyFromRaw(raw), raw, rawToMillivolts(raw)};
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
  static const int rightExpectedRaw = 0;     // 0.00 V
  static const int upExpectedRaw = 145;      // 0.71 V
  static const int downExpectedRaw = 329;    // 1.61 V
  static const int leftExpectedRaw = 505;    // 2.47 V
  static const int selectExpectedRaw = 741;  // 3.62 V
  static const int noneExpectedRaw = 1023;   // 5.00 V

  static const int rightToUpThreshold = (rightExpectedRaw + upExpectedRaw) / 2;
  static const int upToDownThreshold = (upExpectedRaw + downExpectedRaw) / 2;
  static const int downToLeftThreshold = (downExpectedRaw + leftExpectedRaw) / 2;
  static const int leftToSelectThreshold = (leftExpectedRaw + selectExpectedRaw) / 2;
  static const int selectToNoneThreshold = (selectExpectedRaw + noneExpectedRaw) / 2;

  static KeyboardKey keyFromRaw(int raw) {
    if (raw <= rightToUpThreshold) {
      return KeyboardKey::Right;
    }
    if (raw <= upToDownThreshold) {
      return KeyboardKey::Up;
    }
    if (raw <= downToLeftThreshold) {
      return KeyboardKey::Down;
    }
    if (raw <= leftToSelectThreshold) {
      return KeyboardKey::Left;
    }
    if (raw <= selectToNoneThreshold) {
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

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
  static const int rightThreshold = 100;
  static const int upThreshold = 200;
  static const int downThreshold = 400;
  static const int leftThreshold = 600;
  static const int selectThreshold = 800;

  static KeyboardKey keyFromRaw(int raw) {
    if (raw < rightThreshold) {
      return KeyboardKey::Right;
    }
    if (raw < upThreshold) {
      return KeyboardKey::Up;
    }
    if (raw < downThreshold) {
      return KeyboardKey::Down;
    }
    if (raw < leftThreshold) {
      return KeyboardKey::Left;
    }
    if (raw < selectThreshold) {
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

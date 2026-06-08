#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Arduino.h>
#include <defs.h>

enum class KeyboardKey {
  None,
  Right,
  Up,
  Down,
  Left,
  Select,
  Return
};

struct KeyboardReading {
  KeyboardKey key;
  int raw;
  int millivolts;
};

class AnalogKeyboard {
public:
  explicit AnalogKeyboard(uint8_t analogPin = KEYBOARD_ANALOG_PIN, unsigned long debounceMs = KEYBOARD_DEBOUNCE_MS,
                          int referenceMillivolts = ADC_REFERENCE_MILLIVOLTS)
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
  static KeyboardKey keyFromMillivolts(int millivolts) {
    if (millivolts < KEYBOARD_RIGHT_THRESHOLD_MV) {
      return KeyboardKey::Right;
    }
    if (millivolts < KEYBOARD_UP_THRESHOLD_MV) {
      return KeyboardKey::Up;
    }
    if (millivolts < KEYBOARD_DOWN_THRESHOLD_MV) {
      return KeyboardKey::Down;
    }
    if (millivolts < KEYBOARD_LEFT_THRESHOLD_MV) {
      return KeyboardKey::Left;
    }
    if (millivolts < KEYBOARD_SELECT_THRESHOLD_MV) {
      return KeyboardKey::Select;
    }
    if (millivolts < KEYBOARD_RETURN_THRESHOLD_MV) {
      return KeyboardKey::Return;
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
  KeyboardReading lastReading_ = {KeyboardKey::None, 1023, ADC_REFERENCE_MILLIVOLTS};
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
    case KeyboardKey::Return:
      return "RETURN";
    case KeyboardKey::None:
    default:
      return "NONE";
  }
}

#endif

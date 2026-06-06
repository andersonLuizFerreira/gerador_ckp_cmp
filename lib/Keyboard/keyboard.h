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

class AnalogKeyboard {
public:
  explicit AnalogKeyboard(uint8_t analogPin = A1, unsigned long debounceMs = 40)
      : analogPin_(analogPin), debounceMs_(debounceMs) {}

  void begin() const {
    pinMode(analogPin_, INPUT);
  }

  int readRaw() const {
    return analogRead(analogPin_);
  }

  KeyboardKey readKey() const {
    const int value = readRaw();

    if (value < 70) {
      return KeyboardKey::Right;
    }
    if (value < 235) {
      return KeyboardKey::Up;
    }
    if (value < 430) {
      return KeyboardKey::Down;
    }
    if (value < 625) {
      return KeyboardKey::Left;
    }
    if (value < 870) {
      return KeyboardKey::Select;
    }

    return KeyboardKey::None;
  }

  KeyboardKey update() {
    const KeyboardKey currentKey = readKey();
    const unsigned long now = millis();

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

private:
  uint8_t analogPin_;
  unsigned long debounceMs_;
  KeyboardKey lastReadKey_ = KeyboardKey::None;
  KeyboardKey stableKey_ = KeyboardKey::None;
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

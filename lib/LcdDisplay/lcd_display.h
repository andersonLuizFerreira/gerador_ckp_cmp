#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <defs.h>

class LcdDisplay {
public:
  explicit LcdDisplay(uint8_t address = LCD_I2C_ADDRESS, uint8_t columns = LCD_COLUMNS, uint8_t rows = LCD_ROWS)
      : address_(address), columns_(columns), rows_(rows) {}

  void begin() {
    Wire.begin();
    delay(50);

    writeNibble(0x03);
    delay(5);
    writeNibble(0x03);
    delayMicroseconds(150);
    writeNibble(0x03);
    delayMicroseconds(150);
    writeNibble(0x02);

    command(0x28);
    command(0x08);
    clear();
    command(0x06);
    command(0x0C);
  }

  void clear() {
    command(0x01);
    delay(2);
  }

  void setCursor(uint8_t column, uint8_t row) {
    static const uint8_t rowOffsets[] = {0x00, 0x40, 0x14, 0x54};

    if (row >= rows_) {
      row = rows_ - 1;
    }
    if (column >= columns_) {
      column = columns_ - 1;
    }

    command(0x80 | (column + rowOffsets[row]));
  }

  void print(const char *text) {
    while (*text != '\0') {
      write(*text++);
    }
  }

  void printPadded(const char *text, uint8_t width = LCD_COLUMNS) {
    uint8_t printed = 0;

    while (*text != '\0' && printed < width) {
      write(*text++);
      printed++;
    }

    while (printed < width) {
      write(' ');
      printed++;
    }
  }

  void backlight(bool enabled) {
    backlightEnabled_ = enabled;
    expanderWrite(0);
  }

private:
  void command(uint8_t value) {
    send(value, 0);
  }

  void write(uint8_t value) {
    send(value, LCD_PIN_RS);
  }

  void send(uint8_t value, uint8_t mode) {
    writeNibble((value >> 4) & 0x0F, mode);
    writeNibble(value & 0x0F, mode);
  }

  void writeNibble(uint8_t nibble, uint8_t mode = 0) {
    const uint8_t data = (nibble << 4) | mode | backlightMask();

    expanderWrite(data);
    pulseEnable(data);
  }

  void pulseEnable(uint8_t data) {
    expanderWrite(data | LCD_PIN_ENABLE);
    delayMicroseconds(1);
    expanderWrite(data & ~LCD_PIN_ENABLE);
    delayMicroseconds(50);
  }

  void expanderWrite(uint8_t data) {
    Wire.beginTransmission(address_);
    Wire.write(data | backlightMask());
    Wire.endTransmission();
  }

  uint8_t backlightMask() const {
    return backlightEnabled_ ? LCD_PIN_BACKLIGHT : 0;
  }

  uint8_t address_;
  uint8_t columns_;
  uint8_t rows_;
  bool backlightEnabled_ = true;
};

#endif

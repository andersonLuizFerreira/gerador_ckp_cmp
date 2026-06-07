#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <defs.h>

class LcdDisplay {
public:
  explicit LcdDisplay(uint8_t address = LCD_I2C_ADDRESS, uint8_t columns = LCD_COLUMNS, uint8_t rows = LCD_ROWS)
      : lcd_(address, columns, rows), columns_(columns), rows_(rows) {}

  void begin() {
    lcd_.begin(columns_, rows_);
    lcd_.backlight();
    clear();
  }

  void clear() {
    lcd_.clear();
  }

  void setCursor(uint8_t column, uint8_t row) {
    if (row >= rows_) {
      row = rows_ - 1;
    }
    if (column >= columns_) {
      column = columns_ - 1;
    }

    lcd_.setCursor(column, row);
  }

  void print(const char *text) {
    lcd_.print(text);
  }

  void printPadded(const char *text, uint8_t width = LCD_COLUMNS) {
    uint8_t printed = 0;

    while (*text != '\0' && printed < width) {
      lcd_.write(*text++);
      printed++;
    }

    while (printed < width) {
      lcd_.write(' ');
      printed++;
    }
  }

  void backlight(bool enabled) {
    if (enabled) {
      lcd_.backlight();
    } else {
      lcd_.noBacklight();
    }
  }

private:
  LiquidCrystal_I2C lcd_;
  uint8_t columns_;
  uint8_t rows_;
};

#endif

#include <Arduino.h>
#include <defs.h>
#include <keyboard.h>
#include <lcd_display.h>

AnalogKeyboard keyboard;
LcdDisplay lcd;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  keyboard.begin();
  lcd.begin();
  lcd.setCursor(0, 0);
  lcd.printPadded("GERADOR CKP CMP");
  lcd.setCursor(0, 1);
  lcd.printPadded("Tecla: NONE");
}

void loop() {
  const KeyboardKey key = keyboard.update();

  if (key != KeyboardKey::None) {
    const KeyboardReading reading = keyboard.lastReading();

    Serial.print("Tecla pressionada: ");
    Serial.print(keyboardKeyName(key));
    Serial.print(" | ADC: ");
    Serial.print(reading.raw);
    Serial.print(" | Tensao: ");
    Serial.print(reading.millivolts);
    Serial.println(" mV");

    lcd.setCursor(0, 1);
    lcd.print("Tecla: ");
    lcd.printPadded(keyboardKeyName(key), LCD_COLUMNS - 7);
  }

  delay(10);
}

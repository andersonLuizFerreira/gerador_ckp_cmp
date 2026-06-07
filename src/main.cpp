#include <Arduino.h>
#include <defs.h>
#include <keyboard.h>
#include <lcd_display.h>
#include <rpm.h>

AnalogKeyboard keyboard;
LcdDisplay lcd;
RpmControl rpmControl;

void printRpm(int rpm) {
  char message[LCD_COLUMNS + 1];
  snprintf(message, sizeof(message), "RPM: %d", rpm);
  lcd.setCursor(0, 0);
  lcd.printPadded(message);
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  keyboard.begin();
  rpmControl.begin();
  lcd.begin();
  rpmControl.update();
  printRpm(rpmControl.rpm());
  lcd.setCursor(0, 1);
  lcd.printPadded("Tecla: NONE");
}

void loop() {
  if (rpmControl.update()) {
    const RpmReading reading = rpmControl.lastReading();

    Serial.print("RPM: ");
    Serial.print(reading.rpm);
    Serial.print(" | ADC: ");
    Serial.println(reading.raw);

    printRpm(reading.rpm);
  }

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

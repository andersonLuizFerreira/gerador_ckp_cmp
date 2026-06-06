#include <Arduino.h>
#include <keyboard.h>

AnalogKeyboard keyboard(A1);

void setup() {
  Serial.begin(9600);
  keyboard.begin();
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
  }

  delay(10);
}

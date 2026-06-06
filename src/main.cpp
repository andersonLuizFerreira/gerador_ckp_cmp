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
    Serial.print("Tecla pressionada: ");
    Serial.println(keyboardKeyName(key));
  }

  delay(10);
}

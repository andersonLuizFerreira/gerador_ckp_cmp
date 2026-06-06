#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("GERADOR_CKP_CMP iniciado");
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

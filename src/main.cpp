#include <Arduino.h>
#include <avr/interrupt.h>
#include <defs.h>
#include <keyboard.h>
#include <lcd_display.h>
#include <rpm.h>
#include <signal_engine.h>
#include <signal_menu.h>
#include <signal_patterns.h>

AnalogKeyboard keyboard;
LcdDisplay lcd;
RpmControl rpmControl;
SignalEngine signalEngine;
volatile bool simulatorRun = false;
SignalMenu signalMenu(lcd, signalEngine, simulatorRun);

struct Timer1Prescaler {
  uint16_t value;
  uint8_t clockSelectBits;
};

static const Timer1Prescaler TIMER1_PRESCALERS[] = {
    {1, _BV(CS10)},
    {8, _BV(CS11)},
    {64, static_cast<uint8_t>(_BV(CS11) | _BV(CS10))},
    {256, _BV(CS12)},
    {1024, static_cast<uint8_t>(_BV(CS12) | _BV(CS10))},
};

ISR(TIMER1_COMPA_vect) {
  if (simulatorRun) {
    signalEngine.tick();
  }
}

void stopSignalTimer() {
  noInterrupts();
  TIMSK1 &= ~_BV(OCIE1A);
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  interrupts();
}

bool configureTimer1Period(uint32_t periodMicros) {
  if (periodMicros == 0) {
    stopSignalTimer();
    return false;
  }

  for (uint8_t i = 0; i < sizeof(TIMER1_PRESCALERS) / sizeof(TIMER1_PRESCALERS[0]); i++) {
    const Timer1Prescaler prescaler = TIMER1_PRESCALERS[i];
    uint32_t timerCounts = (static_cast<uint64_t>(F_CPU) * periodMicros) / 1000000UL / prescaler.value;

    if (timerCounts == 0) {
      timerCounts = 1;
    }

    if (timerCounts <= 65536UL) {
      noInterrupts();
      TCCR1A = 0;
      TCCR1B = 0;
      TCNT1 = 0;
      OCR1A = static_cast<uint16_t>(timerCounts - 1);
      TCCR1B = _BV(WGM12) | prescaler.clockSelectBits;
      TIMSK1 |= _BV(OCIE1A);
      interrupts();
      return true;
    }
  }

  stopSignalTimer();
  return false;
}

void configureSignalTimer(int rpm) {
  if (!simulatorRun) {
    stopSignalTimer();
    return;
  }

  if (rpm <= 0) {
    stopSignalTimer();
    return;
  }

  const uint32_t ticksPerRevolution = signalEngine.ticksPerRevolution();
  if (ticksPerRevolution == 0) {
    stopSignalTimer();
    return;
  }

  const uint32_t denominator = static_cast<uint32_t>(rpm) * ticksPerRevolution;
  const uint32_t periodMicros = (60000000UL + (denominator / 2)) / denominator;
  configureTimer1Period(periodMicros);
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  keyboard.begin();
  rpmControl.begin();
  lcd.begin();
  rpmControl.update();
  signalMenu.begin();
  configureSignalTimer(rpmControl.rpm());
}

void loop() {
  const bool rpmChanged = rpmControl.update();
  if (rpmChanged) {
    const RpmReading reading = rpmControl.lastReading();

    Serial.print("RPM: ");
    Serial.print(reading.rpm);
    Serial.print(" | ADC: ");
    Serial.println(reading.raw);
  }

  const KeyboardKey key = keyboard.update();
  const bool wasRunning = simulatorRun;

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

  signalMenu.update(key, rpmControl.rpm());

  if (rpmChanged || wasRunning != simulatorRun) {
    configureSignalTimer(rpmControl.rpm());
  }

  delay(10);
}

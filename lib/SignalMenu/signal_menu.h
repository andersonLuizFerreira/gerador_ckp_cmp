#ifndef SIGNAL_MENU_H
#define SIGNAL_MENU_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <keyboard.h>
#include <lcd_display.h>
#include <signal_engine.h>
#include <signal_patterns.h>

static const char MENU_TEXT_MANUFACTURER[] PROGMEM = "Fabricante:";
static const char MENU_TEXT_MODEL[] PROGMEM = "Modelo:";

enum class SignalMenuPage {
  Manufacturer,
  Model
};

class SignalMenu {
public:
  SignalMenu(LcdDisplay &lcd, SignalEngine &engine, volatile bool &runFlag)
      : lcd_(lcd), engine_(engine), runFlag_(runFlag) {}

  void begin() {
    selectedManufacturerIndex_ = 0;
    selectedModelIndex_ = 0;
    page_ = SignalMenuPage::Manufacturer;
    runFlag_ = false;
    engine_.begin();
    renderStopped();
  }

  void update(KeyboardKey key, int rpm) {
    if (key == KeyboardKey::None && !runFlag_ && rendered_) {
      return;
    }

    if (key == KeyboardKey::None && runFlag_ && rpm == lastRenderedRpm_) {
      return;
    }

    if (runFlag_) {
      if (key == KeyboardKey::Select || key == KeyboardKey::Right || key == KeyboardKey::Return) {
        stop();
      }

      render(rpm);
      return;
    }

    if (key == KeyboardKey::Select || key == KeyboardKey::Right) {
      confirmStoppedSelection();
    } else if (key == KeyboardKey::Left || key == KeyboardKey::Return) {
      backStoppedSelection();
    } else if (key == KeyboardKey::Down) {
      nextItem();
    } else if (key == KeyboardKey::Up) {
      previousItem();
    }

    render(rpm);
  }

  uint8_t selectedPatternIndex() const {
    return currentPatternIndex();
  }

  const SignalPattern &selectedPattern() const {
    return *SIGNAL_PATTERNS[currentPatternIndex()];
  }

private:
  void start() {
    runFlag_ = false;
    applySignalPattern(engine_, selectedPattern());
    engine_.reset();
    runFlag_ = true;
  }

  void stop() {
    runFlag_ = false;
    engine_.reset();
  }

  void confirmStoppedSelection() {
    if (page_ == SignalMenuPage::Manufacturer) {
      page_ = SignalMenuPage::Model;
      selectedModelIndex_ = 0;
      rendered_ = false;
      return;
    }

    start();
  }

  void backStoppedSelection() {
    if (page_ == SignalMenuPage::Model) {
      page_ = SignalMenuPage::Manufacturer;
      rendered_ = false;
    }
  }

  void nextItem() {
    if (page_ == SignalMenuPage::Manufacturer) {
      selectedManufacturerIndex_++;
      if (selectedManufacturerIndex_ >= SIGNAL_MANUFACTURER_COUNT) {
        selectedManufacturerIndex_ = 0;
      }
    } else {
      selectedModelIndex_++;
      if (selectedModelIndex_ >= currentManufacturer().modelCount) {
        selectedModelIndex_ = 0;
      }
    }

    rendered_ = false;
  }

  void previousItem() {
    if (page_ == SignalMenuPage::Manufacturer) {
      if (selectedManufacturerIndex_ == 0) {
        selectedManufacturerIndex_ = SIGNAL_MANUFACTURER_COUNT - 1;
      } else {
        selectedManufacturerIndex_--;
      }
    } else {
      if (selectedModelIndex_ == 0) {
        selectedModelIndex_ = currentManufacturer().modelCount - 1;
      } else {
        selectedModelIndex_--;
      }
    }

    rendered_ = false;
  }

  void render(int rpm) {
    lastRenderedRpm_ = rpm;

    if (runFlag_) {
      renderRunning(rpm);
    } else {
      renderStopped();
    }
  }

  void renderStopped() {
    lastRenderedRpm_ = -1;
    rendered_ = true;

    if (page_ == SignalMenuPage::Manufacturer) {
      renderManufacturer();
      return;
    }

    renderModel();
  }

  void renderManufacturer() {
    lcd_.setCursor(0, 0);
    lcd_.printPadded_P(MENU_TEXT_MANUFACTURER);
    lcd_.setCursor(0, 1);
    lcd_.printPadded_P(currentManufacturer().name, LCD_COLUMNS - 1);
    lcd_.setCursor(LCD_COLUMNS - 1, 1);
    lcd_.print(">");
  }

  void renderModel() {
    lcd_.setCursor(0, 0);
    lcd_.printPadded_P(MENU_TEXT_MODEL);
    lcd_.setCursor(0, 1);
    lcd_.printPadded_P(selectedPattern().name, LCD_COLUMNS - 1);
    lcd_.setCursor(LCD_COLUMNS - 1, 1);
    lcd_.print(">");
  }

  void renderRunning(int rpm) {
    char rpmText[LCD_COLUMNS + 1];
    snprintf(rpmText, sizeof(rpmText), "RPM:%-4d     %c/%c", rpm, outputTypeCode(selectedPattern().ckp.outputType),
             outputTypeCode(selectedPattern().cmp.outputType));

    lcd_.setCursor(0, 0);
    lcd_.printPadded_P(selectedPattern().name);
    lcd_.setCursor(0, 1);
    lcd_.printPadded(rpmText);
    rendered_ = true;
  }

  static char outputTypeCode(SignalOutputType outputType) {
    return outputType == SignalOutputType::Inductive ? 'I' : 'H';
  }

  const SignalManufacturer &currentManufacturer() const {
    return SIGNAL_MANUFACTURERS[selectedManufacturerIndex_];
  }

  uint8_t currentPatternIndex() const {
    return currentManufacturer().modelPatternIndexes[selectedModelIndex_];
  }

  LcdDisplay &lcd_;
  SignalEngine &engine_;
  volatile bool &runFlag_;
  SignalMenuPage page_ = SignalMenuPage::Manufacturer;
  uint8_t selectedManufacturerIndex_ = 0;
  uint8_t selectedModelIndex_ = 0;
  int lastRenderedRpm_ = -1;
  bool rendered_ = false;
};

#endif

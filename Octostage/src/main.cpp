#include "OctoConfig.h"
#include "OctoDisplay.h"
#include "OctoMidi.h"
#include "OctoMux.h"
#include "OctoStorage.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

// Global Objects
OctoMux mux;
OctoDisplay display;
OctoStorage storage;
OctoMidi midi;
Adafruit_NeoPixel pixels(10, PIN_LEDS, NEO_GRB + NEO_KHZ800);

void setup() {
  // 1. Init Serial (USB)
  Serial.begin(115200);

  // 2. Init Subsystems
  mux.begin();
  display.begin();
  storage.begin(); // Inits Serial1
  midi.begin();
  pixels.begin();

  // 3. Initial Load
  storage.loadPreset(0);

  // Welcome Screen
  display.setLabel(0, "Octostage");
  display.setLabel(1, "Ready");
}

void loop() {
  // 1. Read Mux Inputs
  uint16_t buttons = mux.readAll();

  // 2. Handle Storage / Preset Updates
  if (storage.update()) {
    ActivePreset *p = storage.getCurrentPreset();
    // Update Displays based on new preset
    for (int i = 0; i < 10; i++) {
      if (i < 8) { // Only 8 displays
        display.setLabel(i, p->switches[i].label);
      }
      // Update LEDs
      pixels.setPixelColor(i, p->switches[i].color);
    }
    pixels.show();
  }

  // 3. Logic based on buttons (Example: Send MIDI on press)
  // Simple state change detection required for actual triggering
  // For this bootstrap, we just show we are reading.
  static uint16_t lastButtons = 0;
  if (buttons != lastButtons) {
    for (int i = 0; i < 10; i++) {
      bool btnState = (buttons >> i) & 1;
      bool lastState = (lastButtons >> i) & 1;

      if (btnState && !lastState) { // Press
        ActivePreset *p = storage.getCurrentPreset();
        // Send MIDI based on switch config
        midi.sendProgramChange(p->switches[i].pc, 1);
      }
    }
    lastButtons = buttons;
  }

  // 4. Update One Display per Loop
  display.updateNextDirty();

  // 5. Update MIDI
  midi.update();
}

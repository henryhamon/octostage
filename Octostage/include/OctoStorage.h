#ifndef OCTO_STORAGE_H
#define OCTO_STORAGE_H

#include "OctoConfig.h"
#include <Arduino.h>

// Struct Definitions
struct __attribute__((packed)) SwitchConfig {
  char label[9];
  uint32_t color; // 0xRRGGBB
  uint8_t pc;
  uint8_t cc;
  uint8_t val;
  uint8_t toggle; // 0 or 1
};

struct __attribute__((packed)) ActivePreset {
  char name[9];
  SwitchConfig switches[10];
};

class OctoStorage {
public:
  OctoStorage();
  void begin();

  // Request a preset from the ESP32
  void loadPreset(uint8_t id);

  // Check if data is available and parse it
  bool update();

  ActivePreset *getCurrentPreset();

private:
  ActivePreset _activePreset;
  bool _isLoading;
};

#endif

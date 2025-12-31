#ifndef OCTO_MUX_H
#define OCTO_MUX_H

#include "OctoConfig.h"

class OctoMux {
public:
  OctoMux();
  void begin();
  void setChannelFast(uint8_t channel);
  uint16_t readAll(); // Reads all 16 channels and returns a bitmask

private:
  uint16_t _state;
  uint16_t _lastState;
  unsigned long _lastDebounceTime[16];
  uint8_t _debounceDelay;
};

#endif

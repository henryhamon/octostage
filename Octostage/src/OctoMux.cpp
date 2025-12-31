#include "OctoMux.h"

OctoMux::OctoMux() : _state(0), _lastState(0), _debounceDelay(10) {
  for (int i = 0; i < 16; i++)
    _lastDebounceTime[i] = 0;
}

void OctoMux::begin() {
  // Set Mux Address pins as Output
  pinMode(PIN_MUX_S0, OUTPUT);
  pinMode(PIN_MUX_S1, OUTPUT);
  pinMode(PIN_MUX_S2, OUTPUT);
  pinMode(PIN_MUX_S3, OUTPUT);

  // Set Signal pin as Input Pullup
  pinMode(PIN_MUX_SIG, INPUT_PULLUP);
}

// Critical Optimization: Direct PORT Manipulation
void OctoMux::setChannelFast(uint8_t channel) {
  // Teensy 2.0 Pins 0, 1, 2, 3 map to PORTB 0, 1, 2, 3
  // We mask out the lower 4 bits of PORTB and OR in the channel
  PORTB = (PORTB & 0xF0) | (channel & 0x0F);
}

uint16_t OctoMux::readAll() {
  uint16_t currentReading = 0;

  for (uint8_t i = 0; i < 16; i++) {
    setChannelFast(i);
    // Small delay might be needed for signal propagation, but usually 74HC4067
    // is fast enough. If issues arise into the future, add asm("nop\n\t") here.

    if (digitalRead(PIN_MUX_SIG) == LOW) {
      currentReading |= (1 << i);
    }
  }

  // Simple Debounce Logic returning stable state
  // Note: For a true "readAll" in a loop, full debounce per button state
  // machine is complex to return as a single atomic scalar each frame without
  // holding state. Assuming this function scans and returns the RAW state for
  // the main loop to handle edge detection OR we implement the debounce here
  // and return the DEBOUNCED state.

  // Let's implement robust debounce per-bit.
  unsigned long now = millis();
  uint16_t changed = currentReading ^ _lastState;
  uint16_t stableState = _state;

  // Use loop to update debouncers
  for (int i = 0; i < 16; i++) {
    if ((changed >> i) & 1) {
      _lastDebounceTime[i] = now;
    }

    if ((now - _lastDebounceTime[i]) > _debounceDelay) {
      // If the bit in currentReading is different from stableState, update it
      bool bitVal = (currentReading >> i) & 1;
      if (bitVal != ((_state >> i) & 1)) {
        if (bitVal) {
          _state |= (1 << i);
        } else {
          _state &= ~(1 << i);
        }
      }
    }
  }

  _lastState = currentReading;
  return _state;
}

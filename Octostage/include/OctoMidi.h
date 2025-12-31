#ifndef OCTO_MIDI_H
#define OCTO_MIDI_H

#include <Arduino.h>

class OctoMidi {
public:
  OctoMidi();
  void begin();

  // Generic Send Wrapper that routes to both USB and Serial1
  void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel);
  void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel);
  void sendControlChange(uint8_t control, uint8_t value, uint8_t channel);
  void sendProgramChange(uint8_t program, uint8_t channel);

  // Update input reading (if needed for Thru, but prompt focused on Output
  // logic mostly)
  void update();

private:
};

#endif

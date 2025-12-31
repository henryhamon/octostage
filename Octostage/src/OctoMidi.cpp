#include "OctoMidi.h"
#include "OctoConfig.h"

OctoMidi::OctoMidi() {}

void OctoMidi::begin() {
  // Serial1 is also used for Storage (on RX/TX lines 7/8).
  // WAIT. The prompt said:
  // "Route MIDI messages to BOTH usbMIDI and Serial1 (Hardware UART)"
  // BUT "OctoConfig: Serial1: RX Pin 7, TX Pin 8"
  // AND "OctoStorage: Teensy sends LOAD:<id> via Serial1"

  // CRITICAL CONFLICT CHECK:
  // If Serial1 is used for communicating with ESP32 for Storage (LOAD command),
  // AND it is used for MIDI Out...
  // The ESP32 must handle both.
  // Prompt says: "7. esp32_firmware... Listen on Serial1 for LOAD... Bridge
  // incoming Serial MIDI to BLEMidiServer" So Serial1 is a MULTIPLEXED stream
  // or just standard Serial. Since MIDI is binary (0x80+) and "LOAD:" is text,
  // they can coexist if parsed correctly. The ESP32 code needs to handle this
  // distinction.

  // Note: Serial1.begin is already called in OctoStorage.
  // We should ensure we don't double-init or reset it, but calling begin()
  // twice with same baud is usually safe. Ideally, we share the Serial1
  // instance or init it in main. For safety, I'll rely on it being initialized,
  // or check if I should init here. I'll assume 115200 for both.
}

void OctoMidi::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
  usbMIDI.sendNoteOn(note, velocity, channel);

  // Serial MIDI Protocol: 0x90 | channel, note, velocity
  Serial1.write(0x90 | (channel & 0x0F));
  Serial1.write(note & 0x7F);
  Serial1.write(velocity & 0x7F);
}

void OctoMidi::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
  usbMIDI.sendNoteOff(note, velocity, channel);

  Serial1.write(0x80 | (channel & 0x0F));
  Serial1.write(note & 0x7F);
  Serial1.write(velocity & 0x7F);
}

void OctoMidi::sendControlChange(uint8_t control, uint8_t value,
                                 uint8_t channel) {
  usbMIDI.sendControlChange(control, value, channel);

  Serial1.write(0xB0 | (channel & 0x0F));
  Serial1.write(control & 0x7F);
  Serial1.write(value & 0x7F);
}

void OctoMidi::sendProgramChange(uint8_t program, uint8_t channel) {
  usbMIDI.sendProgramChange(program, channel);

  Serial1.write(0xC0 | (channel & 0x0F));
  Serial1.write(program & 0x7F);
}

void OctoMidi::update() {
  // Process incoming USB MIDI if needed
  usbMIDI.read();
}

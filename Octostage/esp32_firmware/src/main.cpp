#include <Arduino.h>
#include <ArduinoJson.h>
#include <BLEMidi.h>
#include <LittleFS.h>

// Struct Validation
struct __attribute__((packed)) SwitchConfig {
  char label[9];
  uint32_t color;
  uint8_t pc;
  uint8_t cc;
  uint8_t val;
  uint8_t toggle;
};

struct __attribute__((packed)) ActivePreset {
  char name[9];
  SwitchConfig switches[10];
};

ActivePreset responsePreset;

void setup() {
  Serial.begin(115200);

  // Init File System
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  // Init BLE MIDI
  BLEMidiServer.begin("Octostage BLE");
}

void sendPreset(int id) {
  File file = LittleFS.open("/presets.json", "r");
  if (!file)
    return;

  // Use ArduinoJson to filter query to save memory?
  // Or just load the array. ESP32 has plenty of RAM relative to this file size.
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error)
    return;

  JsonArray root = doc.as<JsonArray>();
  JsonObject p;
  bool found = false;
  for (JsonObject v : root) {
    if (v["id"] == id) {
      p = v;
      found = true;
      break;
    }
  }

  if (!found)
    return; // Or send default

  // Packs JSON into Binary Struct
  memset(&responsePreset, 0, sizeof(ActivePreset));
  strncpy(responsePreset.name, p["name"] | "Unknown", 8); // Safe copy

  JsonArray sw = p["switches"];
  int i = 0;
  for (JsonObject s : sw) {
    if (i >= 10)
      break;
    strncpy(responsePreset.switches[i].label, s["label"] | "", 8);
    responsePreset.switches[i].color = s["color"];
    responsePreset.switches[i].pc = s["pc"];
    responsePreset.switches[i].cc = s["cc"];
    responsePreset.switches[i].val = s["val"];
    responsePreset.switches[i].toggle = s["toggle"];
    i++;
  }

  // Send Raw Bytes
  Serial.write((const uint8_t *)&responsePreset, sizeof(ActivePreset));
}

void loop() {
  // 1. Handle Serial Commands
  if (Serial.available()) {
    char c = Serial.peek();

    // Check for "LOAD:" command
    // Simple string parsing
    if (c == 'L') {
      String cmd = Serial.readStringUntil(':');
      if (cmd == "LOAD") {
        int id = Serial.parseInt();
        // Clear newline residue
        while (Serial.available() && Serial.peek() <= 32)
          Serial.read();
        sendPreset(id);
      }
    } else if (c & 0x80) {
      // It's a MIDI byte (Status >= 128)
      // Forward whole packet to BLE
      // BLEMidiServer usually takes standard MIDI packets.
      // But Serial serves raw bytes. We need to parse or forward blindly.
      // MIDI output from Teensy is nicely framed by write calls, but we might
      // read them fragmented. Simple bridge:
      uint8_t status = Serial.read();
      // Depending on status, read 1 or 2 data bytes
      int len = 0;
      uint8_t d1 = 0, d2 = 0;

      if ((status & 0xF0) == 0xC0 || (status & 0xF0) == 0xD0) {
        len = 1; // PC or Channel Pressure (2 bytes total)
      } else if ((status & 0xF0) != 0xF0) {
        len = 2; // Note On/Off, CC, Pitch Bend (3 bytes total)
      }

      if (len > 0) {
        while (Serial.available() < len) {
          // Timeout protection recommended here for real pro code
          delay(0);
        }
        d1 = Serial.read();
        if (len == 2)
          d2 = Serial.read();

        // Construct packet
        // BLEMidi library helper?
        if (BLEMidiServer.isConnected()) {
          // Only forwarding typical performance messages
          if ((status & 0xF0) == 0x90)
            BLEMidiServer.noteOn(status & 0x0F, d1, d2);
          else if ((status & 0xF0) == 0x80)
            BLEMidiServer.noteOff(status & 0x0F, d1, d2);
          else if ((status & 0xF0) == 0xB0)
            BLEMidiServer.controlChange(status & 0x0F, d1, d2);
          else if ((status & 0xF0) == 0xC0)
            BLEMidiServer.programChange(status & 0x0F, d1);
        }
      }
    } else {
      // Consume garbage
      Serial.read();
    }
  }
}

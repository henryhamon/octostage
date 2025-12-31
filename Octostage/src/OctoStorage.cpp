#include "OctoStorage.h"

OctoStorage::OctoStorage() : _isLoading(false) {
  // Initialize default values
  memset(&_activePreset, 0, sizeof(ActivePreset));
  strcpy(_activePreset.name, "Init");
}

void OctoStorage::begin() {
  Serial1.begin(115200); // Communicate with ESP32
}

void OctoStorage::loadPreset(uint8_t id) {
  Serial1.print("LOAD:");
  Serial1.println(id);
  _isLoading = true;
}

bool OctoStorage::update() {
  if (Serial1.available() >= sizeof(ActivePreset)) {
    // Read the binary struct directly
    // We assume the ESP32 sends ONLY the binary data after the request.
    // In a real robust protocol, we'd have SOF/EOF bytes or checksums.
    // For this strict prompt requirement, we read raw.

    Serial1.readBytes((char *)&_activePreset, sizeof(ActivePreset));
    _isLoading = false;
    return true;
  }
  return false;
}

ActivePreset *OctoStorage::getCurrentPreset() { return &_activePreset; }

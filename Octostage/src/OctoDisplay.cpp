#include "OctoDisplay.h"

// Define the static member
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C OctoDisplay::u8g2(U8G2_R0,
                                                         U8X8_PIN_NONE);

OctoDisplay::OctoDisplay() : _currentUpdateIndex(0) {
  for (int i = 0; i < 8; i++) {
    _dirty[i] = true;
    _labels[i][0] = '\0';
  }
}

void OctoDisplay::begin() {
  Wire.begin();
  Wire.setClock(400000); // 400kHz
  u8g2.begin();
}

void OctoDisplay::selectChannel(uint8_t i) {
  if (i > 7)
    return;
  Wire.beginTransmission(TCA9548A_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void OctoDisplay::setLabel(uint8_t displayIndex, const char *label) {
  if (displayIndex >= 8)
    return;
  strncpy(_labels[displayIndex], label, 15);
  _labels[displayIndex][15] = '\0'; // Ensure null term
  _dirty[displayIndex] = true;
}

void OctoDisplay::updateNextDirty() {
  // Find next dirty screen via round-robin to avoid starvation
  for (int i = 0; i < 8; i++) {
    int target = (_currentUpdateIndex + i) % 8;
    if (_dirty[target]) {
      // Found a dirty screen

      // 1. Switch TCA Channel
      selectChannel(target);

      // 2. Clear Buffer logic handled by u8g2 first page
      u8g2.clearBuffer();

      // 3. Render
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 20, _labels[target]);

      // 4. Send Buffer
      u8g2.sendBuffer();

      // 5. Clear Dirty Flag
      _dirty[target] = false;

      // Update index for next call
      _currentUpdateIndex = (target + 1) % 8;
      return; // Only update one per call
    }
  }
}

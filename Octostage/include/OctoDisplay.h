#ifndef OCTO_DISPLAY_H
#define OCTO_DISPLAY_H

#include <U8g2lib.h>
#include <Wire.h>

#define TCA9548A_ADDR 0x70

class OctoDisplay {
public:
  OctoDisplay();
  void begin();

  // Draw string to a specific display's virtual state.
  // Actual rendering happens in updateNextDirty()
  void setLabel(uint8_t displayIndex, const char *label);

  // Main loop function
  void updateNextDirty();

private:
  void selectChannel(uint8_t i);

  // We only have one actual U8G2 instance and buffer
  static U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2;

  // State for contents of screens
  char _labels[8][16]; // Store labels for re-drawing.
  bool _dirty[8];
  uint8_t _currentUpdateIndex;
};

#endif

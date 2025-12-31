#ifndef OCTO_CONFIG_H
#define OCTO_CONFIG_H

#include <Arduino.h>

// Teensy 2.0 Pinout Definitions

// I2C (Hardware Pins)
#define PIN_I2C_SDA 5
#define PIN_I2C_SCL 6

// Serial1 (Hardware UART)
#define PIN_RX1 7
#define PIN_TX1 8

// Mux Control (74HC4067)
// Pins 0-3 correspond to PORTB 0-3 on ATmega32U4 (Teensy 2.0)
// S0=PB0, S1=PB1, S2=PB2, S3=PB3
#define PIN_MUX_S0 0
#define PIN_MUX_S1 1
#define PIN_MUX_S2 2
#define PIN_MUX_S3 3

// Mux Input
#define PIN_MUX_SIG 9

// LED Strip
#define PIN_LEDS 4

#endif

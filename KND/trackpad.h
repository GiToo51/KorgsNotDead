/*!
 * Inspired from Adafruit_PS2_Trackpad (but faster, lighter and interrupt driven)
 * Developed for Synaptics 920-000241-02 RevA (Chip T1006). Not tested with other devices.
 * SPECS found here: https://www.aquaphoenix.com/hardware/ledlamp/reference/synaptics_touchpad_interfacing_guide.pdf
 * PS2 trackpad, clk interupt driven for reading, clk change waiting for writing
 * GND on the big iron zone
 * T22: +5v
 * T10: Clk
 * T11: Dat
 */

#ifndef _TRACKPAD_H
#define _TRACKPAD_H

#include "Arduino.h"

#define TRACKPAD_RESET         0xFF //!< Resets PS2 device
#define TRACKPAD_RESEND        0xFE //!< Resends command
#define TRACKPAD_ERROR         0xFC //!< PS2 Error response byte
#define TRACKPAD_ACK           0xFA //!< PS2 acknowledge byte
#define TRACKPAD_SETDEFAULTS   0xF6 //!< Used to set the default settings
#define TRACKPAD_DISABLE       0xF5 //!< Disables the PS2 device
#define TRACKPAD_ENABLE        0xF4 //!< Enables the PS2 device
#define TRACKPAD_SETRATE       0xF3 //!< Sets the report rate
#define TRACKPAD_GETDEVICEID   0xF2 //!< Gets the device ID
#define TRACKPAD_SETPOLL       0xF0 //!< Polls the PS2 device
#define TRACKPAD_SETWRAPMODE   0xEE //!< Turns on wrap mode
#define TRACKPAD_RESETWRAPMODE 0xEC //!< Turns off wrap mode
#define TRACKPAD_READDATA      0xEB //!< Reads data received from the PS2 device
#define TRACKPAD_SETSTREAMMODE 0xEA //!< Turns on streammode
#define TRACKPAD_STATUSREQUEST 0xE9 //!< Asks the PS2 device for its status
#define TRACKPAD_SETRES        0xE8 //!< Sets resolution, sends one argument
#define TRACKPAD_SETSCALE11    0xE6 //!< Sets scaling to 1:1
#define TRACKPAD_SETSCALE21    0xE7 //!< Sets scaling to 2:1
#define TRACKPAD_GETINFO       0xE9 //!< Status request. Receives 3 arg. bytes

#define TRACKPAD_TIMEOUT_COUNT 32

#define TRACKPAD_MAX_14BITS 16383

class Trackpad {
public:
  bool setup(uint8_t clk, uint8_t data);

  void scan();
  void interrupt(void);
  bool command(uint8_t cmd);
  bool command(uint8_t cmd, uint8_t param);

private:
  uint8_t clkPin;
  uint8_t dataPin;

  uint8_t waitByte();
  bool write(uint8_t x);
  bool waitFor(uint8_t p, uint8_t v);
  void high(uint8_t p);
  void low(uint8_t p);

  volatile int8_t bitPos;
  volatile uint8_t dataPos;

  bool writing;
  volatile uint8_t dataWriting; // 0 or 1 to avoid corrupted data while reading incomplete data
  uint8_t data[2][6]; // so we use 2 buffers and alternate reading / writing between the 2

  volatile bool pressed;
  uint8_t scanPos; // scan X, then scan Y on another cycle to be fast
  int16_t lastX;
  int16_t startY;
  int16_t lastY;
  uint8_t lastZ;
};

extern Trackpad trackpad;

#endif

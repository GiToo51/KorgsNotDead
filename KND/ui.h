
// LED
extern uint8_t ledMode;
enum {
  LED_MODE_OFF   = 0,
  LED_MODE_BOOT  = 1,
  LED_MODE_MENU  = 2,
  LED_MODE_SOLO  = 3,
  LED_MODE_SCALE = 4, // Scale
  LED_MODE_SCORE = 5, // Partition
  LED_MODE_MIDI_ONLY  = 6, // Midi in only (default)
  LED_MODE_MIDI_SOLO  = 7, // Solo + Midi in
  LED_MODE_GAME  = 8, // White if match midi, Red otherwize
  LED_MODE_COUNT = 9,
};
extern const computeByte_f ledComputeKey[LED_MODE_COUNT];
extern const computeByte_f ledComputeAlt[LED_MODE_COUNT];

// LCD
enum {
  LCD_MODE_OFF   = 0,
  LCD_MODE_BOOT  = 1,
  LCD_MODE_MENU  = 2,
  LCD_MODE_VELO  = 3,
  LCD_MODE_DEBUG = 4,
  LCD_MODE_COUNT = 5,
};
extern const computeByte_f lcdComputeByte[LCD_MODE_COUNT];
extern uint8_t lcdMode;
#define LCD_DEBUG_VELOCITY_SIZE 14
extern uint8_t lcdDebugVelocity[LCD_DEBUG_VELOCITY_SIZE];
extern uint8_t lcdVelocityPos;


class UI {

public:
  UI();
  // When this serial port is disabled and input mode, ui arduino nano can be flashed for code upgrade
  void enableSerial();
  void disableSerial();

  void reset();
  void sendNext();

private:
  void writeUIBegin();
  void writeUILcd();
  void writeUILed();

  uint8_t uiWritePos;
  uint8_t uiWriteStep;
  uint8_t uiTotalFrames;
  bool uiSerialEnabled;
};

extern UI ui;

#undef DEBUG_MODE

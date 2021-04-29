
#include "trackpad.h"
#include "midi.h"
#include "settings.h"

#define TRACKPAD_X_DELTA 5696
#define TRACKPAD_Y_DELTA 14944
#define TRACKPAD_Z_MINI 16
#define TRACKPAD_PITCH_SMOOTH 64

Trackpad trackpad;

void trackpadInterrupt(void) { trackpad.interrupt(); }

bool Trackpad::setup(uint8_t c, uint8_t d)
{
  clkPin = c;
  dataPin = d;
  lastX = 0;
  lastY = TRACKPAD_MAX_14BITS/2;
  lastZ = 255;
  scanPos = 0;
  pressed = false;

  bitPos = -1;
  dataPos = 0;
  writing = false;
  dataWriting = 0;

  memset(data, 0, sizeof(data));

  high(clkPin);
  high(dataPin);
  attachInterrupt(digitalPinToInterrupt(clkPin), trackpadInterrupt, FALLING);

  if ( ! command(TRACKPAD_RESET        )) return false;
  if ( ! command(TRACKPAD_SETDEFAULTS  )) return false;
  
  if ( ! command(TRACKPAD_SETRES,  0x02)) return false; // 0x03 for fast mode
  if ( ! command(TRACKPAD_SETRES,  0x00)) return false;
  if ( ! command(TRACKPAD_SETRES,  0x00)) return false;
  if ( ! command(TRACKPAD_SETRES,  0x01)) return false;
  if ( ! command(TRACKPAD_SETRATE, 0x14)) return false;

/*  if ( ! command(TRACKPAD_STATUSREQUEST)) return false;
  Serial.print("STATUS: ");
  Serial.print(waitByte(), HEX);
  Serial.print(' ');
  Serial.print(waitByte(), HEX);
  Serial.print(' ');
  Serial.println(waitByte(), HEX);*/

  if ( ! command(TRACKPAD_DISABLE)) return false;
  if ( ! command(TRACKPAD_DISABLE)) return false;
  if ( ! command(TRACKPAD_DISABLE)) return false;
  if ( ! command(TRACKPAD_DISABLE)) return false;
  if ( ! command(TRACKPAD_ENABLE )) return false;

  return true;
}

// to receive 8bit words to configure
void Trackpad::interrupt(void)
{
  if (writing) return;
  uint8_t val = digitalRead(dataPin);

  switch (bitPos) {
    case -1: // start
      data[dataWriting][dataPos] = 0;
      bitPos = 0;
      break;

    case 0: case 1: case 2: case 3:
    case 4: case 5: case 6: case 7:
      data[dataWriting][dataPos] |= (val << bitPos);
      bitPos += 1;
      break;

    case 8: // parity TODO verify ?
      bitPos = 9;
      break;

    case 9: // end
      bitPos = -1;
      if (dataPos == 5) {
        if (data[dataWriting][2] >= TRACKPAD_Z_MINI) { // requre minimum pressing force
          pressed = true;
          dataWriting = dataWriting ? 0 : 1;
          lastZ = data[dataWriting][2];
        } else {
          pressed = false;
        }
        dataPos = 0;
      } else {
        dataPos += 1;
      }
      break;
  }
}

void Trackpad::scan() {
  /*
    Cf: https://www.aquaphoenix.com/hardware/ledlamp/reference/synaptics_touchpad_interfacing_guide.pdf Page:43
    X & Y are switched (trackpad is vertically mounted)
    Y = Pitch
    X = Modulation, reversed (0 on bottom)
    uint8_t z = data[2];
    uint8_t s = ((data[0] & B00110000) << 2 | (data[0] & B00000100) << 3 | (data[3] & B00000111) << 2 | (data[0] & B00000011));
  */
  uint8_t dataReading = dataWriting ? 0 : 1;
  int16_t newValue;
  switch (scanPos) {
    case 0:
      newValue = ((data[dataReading][3] & B00010000) << (14-4) | (data[dataReading][1] & B00001111) << (10-0) | data[dataReading][4]<<2) - TRACKPAD_X_DELTA;
      if (newValue < 0)
        newValue = TRACKPAD_MAX_14BITS;
      else if (newValue >= TRACKPAD_MAX_14BITS)
        newValue = 0;
      else
        newValue = TRACKPAD_MAX_14BITS - newValue;

      if (lastX != newValue) {
          lastX  = newValue;
        midiExt.sendControlChange(midi::ModulationWheel   , (uint8_t)(lastX/128), settings.outputChannel);
        midiExt.sendControlChange(midi::ModulationWheel+32, (uint8_t)(lastX%128), settings.outputChannel);
      }
      scanPos = 1;
      break;
    case 1:
//        newValue = ((data[dataReading][3] & B00100000) << (15-5) | (data[dataReading][1] & B11110000) << (11-4) | (data[dataReading][5]<<3)) - TRACKPAD_Y_DELTA;
      newValue = ((data[dataReading][3] & B00100000) << (14-5) | (data[dataReading][1] & B11110000) << (10-4) | (data[dataReading][5]<<2));
      if (pressed) {
        if (startY == -1) {
          startY = newValue;
        } else {
          newValue = 8192 - startY - newValue;
          if (newValue < 0)
            newValue = 0;
          else if (newValue >= TRACKPAD_MAX_14BITS)
            newValue = TRACKPAD_MAX_14BITS;
          if (lastY != newValue) {
              lastY = newValue;
            midiExt.sendPitchBend(MIDI_PITCHBEND_MIN + lastY, settings.outputChannel);
          }
        }
      } else {
        // smooth return to pitch 0
        startY = -1;
        newValue = 8192;
        if (lastY != newValue) {
          if (lastY > newValue) {
            lastY -= TRACKPAD_PITCH_SMOOTH;
            if (lastY < newValue)
              lastY = newValue;
          } else {
            lastY += TRACKPAD_PITCH_SMOOTH;
            if (lastY > newValue)
              lastY = newValue;
          }
          midiExt.sendPitchBend(MIDI_PITCHBEND_MIN + lastY, settings.outputChannel);
        }
      }
      scanPos = 2; // we temporise ControlChange / PitchBend to avoid Serial port saturation
      break;
    default:
      scanPos = 0;
      break;
  }
}

bool Trackpad::command(uint8_t cmd) {
  return (write(cmd) && waitByte() == TRACKPAD_ACK);
}

bool Trackpad::command(uint8_t cmd, uint8_t param) {
  return (write(cmd) && write(param) && waitByte() == TRACKPAD_ACK);
}

bool Trackpad::write(uint8_t b) {
  writing = true;
//  delayMicroseconds(300);
  low(clkPin);
//  delayMicroseconds(100);
  low(dataPin);
//  delayMicroseconds(10);

  // we pull the clock line up to indicate we're ready
  high(clkPin);
  // wait for the device to acknowledge by pulling it down
  if ( ! waitFor(clkPin, LOW )) return false;

  uint8_t parity = 1;
  for (uint8_t i = 0; i < 8; i++) {
    if (b & 0x1)
      high(dataPin);
    else
      low(dataPin);

  // the clock lines are driven by the -DEVICE- so we wait
  if ( ! waitFor(clkPin, HIGH)) return false;
  if ( ! waitFor(clkPin, LOW )) return false;

    parity ^= b;

    b >>= 1;
  }
  if (parity & 0x1)
    high(dataPin);
  else
    low(dataPin);
  if ( ! waitFor(clkPin, HIGH)) return false;
  if ( ! waitFor(clkPin, LOW )) return false;

  // send stop bit (high)
  high(dataPin);
  delayMicroseconds(50);

  if ( ! waitFor(clkPin , LOW )) return false;
  if ( ! waitFor(clkPin , HIGH)) return false;
  if ( ! waitFor(dataPin, HIGH)) return false;

  writing = false;
  return true;
}

uint8_t Trackpad::waitByte()
{
  for (uint8_t count = 0; count < TRACKPAD_TIMEOUT_COUNT; count++) {
    if (dataPos != 0) {
      dataPos = 0;
      return data[dataWriting][0];
    }
    delayMicroseconds(50);
  }
  return 0; // timeout
}

bool Trackpad::waitFor(uint8_t p, uint8_t v) {
  for (uint8_t count = 0; count < TRACKPAD_TIMEOUT_COUNT; count++)
    if (digitalRead(p) == v)
      return true;
  return false; // timeout
}

void Trackpad::high(uint8_t p) {
  digitalWrite(p, HIGH);
  pinMode(p, INPUT_PULLUP);
}
void Trackpad::low(uint8_t p) {
  digitalWrite(p, LOW);
  pinMode(p, OUTPUT);
}

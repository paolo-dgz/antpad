#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>

class Board {
public:
  virtual void boardInit(bool motw_servob = false);
  virtual void motRSetSpeed(int speed);
  virtual void motLSetSpeed(int speed);
  virtual void motWSetSpeed(int speed);
  virtual void servoASetAngle(int angle);
  virtual void servoBSetAngle(int angle);
  virtual void failsafe();
  virtual void setLed(bool state);
  const byte EEPROM_VALID_BYTE = 99;
};

#endif
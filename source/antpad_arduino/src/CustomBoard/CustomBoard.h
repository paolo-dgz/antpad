#ifndef C3SUPERMINI_DRV8833_H
#define C3SUPERMINI_DRV8833_H


//used to create additional custom board but keeping the main behaviour
#include "../Board.h"
#include <Arduino.h>

#define EEPROM_UNIQUE_VALUE 101

class CustomBoard : public Board {
public:
  void boardInit(bool motw_servob = false);
  void motRSetSpeed(int speed);
  void motLSetSpeed(int speed);
  void motWSetSpeed(int speed);
  void servoASetAngle(int angle);
  void servoBSetAngle(int angle);
  void failsafe();
  void setLed(bool state);
  const byte EEPROM_VALID_BYTE = EEPROM_UNIQUE_VALUE;

  //add your private functions and vars here

};

#endif
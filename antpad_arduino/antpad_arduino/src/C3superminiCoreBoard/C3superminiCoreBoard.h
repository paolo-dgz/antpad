#ifndef C3SUPERMINI_CORE_H
#define C3SUPERMINI_CORE_H

//custom www.irobottari.com shield for c3 supermini and 3 dc drivers
#include "../Board.h"
#include <driver/ledc.h>
#include <Arduino.h>

#define EEPROM_UNIQUE_VALUE 101


class C3superminiCoreBoard : public Board {
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

private:
  bool motwServoMode = false;
  void setMotorSpeed(char en_ledc_ch, char ph_pin, int speed);
  void setServoAngle(char servo_ledcch, int angle);
};

#endif
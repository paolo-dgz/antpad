#ifdef ARDUINO_ESP32S3_DEV

#ifndef ESP32S3_RAC_H
#define ESP32S3_RAC_H

// to use with RAC core boards
#include "../Board.h"
#include <driver/ledc.h>
#include <driver/mcpwm.h>
#include <Arduino.h>
#include "RACPinouts.h"


class ESP32S3RACBoard : public Board
{
public:
  void boardInit(board_cfg_t init_cfg);
  void motRSetSpeed(int speed);
  void motLSetSpeed(int speed);
  void motWSetSpeed(int speed);
  void motWSeekPot(int angle, int dc_dir = 1);
  void motW2SetSpeed(int speed);
  void servoASetAngle(int angle);
  void servoBSetAngle(int angle);
  void failsafe();
  void setLed(bool state);

private:
  void setMotorSpeed(mcpwm_unit_t unit, mcpwm_timer_t timer, int speed);
  void setServoAngle(char servo_ledcch, int angle);
  float pid_prop_k = 4;
  float pid_deriv_k = 15.0;
  float pid_integ_k = 0.3;
  float prev_error = 0.0;
  float pid_integral = 0.0;
  unsigned long pid_prev_time = 0;
};

#endif
#endif
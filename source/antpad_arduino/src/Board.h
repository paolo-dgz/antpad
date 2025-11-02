#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>

class Board
{
public:
  struct board_cfg_t
  {
    bool dc_servo = false;
    bool servo_stretcher = false;
  };
  virtual void boardInit(board_cfg_t init_cfg);
  virtual void motRSetSpeed(int speed);
  virtual void motLSetSpeed(int speed);
  virtual void motWSetSpeed(int speed);
  virtual void motWSeekPot(int angle, int dc_dir = 1);
  virtual void servoASetAngle(int angle);
  virtual void servoBSetAngle(int angle);
  virtual void failsafe();
  virtual void setLed(bool state);

protected:
  board_cfg_t board_cfg;
  int servo_min_duty = 820; //410
  int servo_max_duty = 1638; //2048
};

#endif
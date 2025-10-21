#ifndef C3SUPERMINI_DRV8833_H
#define C3SUPERMINI_DRV8833_H


//used to create additional custom board but keeping the main behaviour
#include "../Board.h"
#include <Arduino.h>


class CustomBoard : public Board {
public:
  void boardInit(board_cfg_t init_cfg);
  void motRSetSpeed(int speed);
  void motLSetSpeed(int speed);
  void motWSetSpeed(int speed);
  void motWSeekPot(int angle);
  void servoASetAngle(int angle);
  void servoBSetAngle(int angle);
  void failsafe();
  void setLed(bool state);

  //add your private functions and vars here

};

#endif
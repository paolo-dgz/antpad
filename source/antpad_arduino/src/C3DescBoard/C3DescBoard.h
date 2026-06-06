#ifndef C3DESCBOARD_H
#define C3DESCBOARD_H


//used to create additional custom board but keeping the main behaviour
#include "../Board.h"
#include <driver/ledc.h>
#include <Arduino.h>

#define LED_PIN 8

#define SERVOA_PIN 4 // pwm

#define MOTR_PIN 0

#define MOTL_PIN 0

#define MOTR_LEDCCH 0 // pwm
#define MOTL_LEDCCH 1 // pwm
#define SERVOA_LEDCCH 2 // pwm

class C3DescBoard : public Board {
public:
  void boardInit(board_cfg_t init_cfg);
  void motRSetSpeed(int speed);
  void motLSetSpeed(int speed);
  void motWSetSpeed(int speed);
  void motWSeekPot(int angle, int dc_dir = 1);
  void servoASetAngle(int angle);
  void servoBSetAngle(int angle);
  void failsafe();
  void setLed(bool state);

  //add your private functions and vars here
private:
  int motor_center_duty = servo_min_duty + (servo_max_duty-servo_min_duty)/2;
  int motor_min_duty = servo_min_duty;
  int motor_max_duty = servo_max_duty;
  void setMotorSpeed(char motor_ledcc, int speed);
  void setServoAngle(char servo_ledcch, int angle);

};

#endif
#ifndef C3SUPERMINI_CORE_H
#define C3SUPERMINI_CORE_H

// custom www.irobottari.com shield for c3 supermini and 3 dc drivers
#include "../Board.h"
#include <driver/ledc.h>
#include <Arduino.h>

#define POT_PIN 3 // adc ADC1_CH3
#define LED_PIN 8

#define SERVOA_PIN 0 // pwm

#define MOTR_PH_PIN 7
#define MOTR_EN_PIN 6 // pwm

#define MOTL_PH_PIN 10
#define MOTL_EN_PIN 1 // pwm

#define MOTW_PH_PIN 5
#define MOTW_EN_PIN 4 // pwm

#define MOTR_EN_LEDCCH 0 // pwm
#define MOTL_EN_LEDCCH 1 // pwm
#define MOTW_EN_LEDCCH 2 // pwm

#define SERVOB_LEDCCH 4 // adc ADC1_CH3
#define SERVOA_LEDCCH 5 // pwm

class C3superminiCoreBoard : public Board
{
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

private:
  void setMotorSpeed(char en_ledc_ch, char ph_pin, int speed);
  void setServoAngle(char servo_ledcch, int angle);
};

#endif
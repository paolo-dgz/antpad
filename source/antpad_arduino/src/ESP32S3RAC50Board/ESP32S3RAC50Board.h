#define ARDUINO_ESP32S3_DEV

#ifdef ARDUINO_ESP32S3_DEV

#ifndef ESP32S3_RAC50_H
#define ESP32S3_RAC50_H

// to use with any c3 supermini and drv8833 board
#include "../Board.h"
#include <driver/ledc.h>
#include <driver/mcpwm.h>
#include <Arduino.h>

#define POT_PIN 11 // adc ADC1_CH3

#define SERVOA_PIN 14
#define SERVOA_LEDCCH 0

#define LED_PIN 21

#define MOTR_IN1_PIN 16
#define MOTR_IN2_PIN 17
#define MOTR_PWM_UNIT MCPWM_UNIT_0
#define MOTR_PWM_TIMER MCPWM_TIMER_0

#define MOTL_IN1_PIN 8
#define MOTL_IN2_PIN 18
#define MOTL_PWM_UNIT MCPWM_UNIT_0
#define MOTL_PWM_TIMER MCPWM_TIMER_1

#define MOTW_IN1_PIN 4
#define MOTW_IN2_PIN 5
#define MOTW_PWM_UNIT MCPWM_UNIT_0
#define MOTW_PWM_TIMER MCPWM_TIMER_2

#define MOTW2_IN1_PIN 6
#define MOTW2_IN2_PIN 15
#define MOTW2_PWM_UNIT MCPWM_UNIT_1
#define MOTW2_PWM_TIMER MCPWM_TIMER_0

class ESP32S3RAC50Board : public Board
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
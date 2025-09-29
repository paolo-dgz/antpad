#ifndef C3SUPERMINI_DRV8833_H
#define C3SUPERMINI_DRV8833_H


//to use with any c3 supermini and drv8833 board
#include "../Board.h"
#include <driver/ledc.h>
#include <Arduino.h>

#define EEPROM_UNIQUE_VALUE 100

#define POT_PIN 2  //adc ADC1_CH3

#define SERVOA_PIN 4

#define LED_PIN 8

#define MOTR_IN1_PIN 0
#define MOTR_IN2_PIN 10

#define MOTL_IN1_PIN 1
#define MOTL_IN2_PIN 3

#define MOTR_IN1_LEDCCH 0
#define MOTR_IN2_LEDCCH 1
#define MOTL_IN1_LEDCCH 2
#define MOTL_IN2_LEDCCH 3
#define SERVOA_LEDCCH 4

class C3superminiDRV8833Board : public Board {
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
  void setMotorSpeed(char in1_ch, char in2_ch, int speed);
  void setServoAngle(char servo_ledcch, int angle);
};



#endif
#include "C3superminiDrv8833Board.h"

// private
void C3superminiDRV8833Board::setMotorSpeed(char in1_ch, char in2_ch, int speed)
{
  speed = constrain(speed, -512, 512);
  if (speed < 0)
  {
    ledcWrite(in1_ch, -speed);
    ledcWrite(in2_ch, 0);
    return;
  }
  if (speed == 0)
  {
    ledcWrite(in1_ch, 512);
    ledcWrite(in2_ch, 512);
    return;
  }
  if (speed > 0)
  {
    ledcWrite(in1_ch, 0);
    ledcWrite(in2_ch, speed);
    return;
  }
}

void C3superminiDRV8833Board::setServoAngle(char servo_ledcch, int angle)
{
  angle = constrain(angle, 0, 1023);
  int duty = map(angle, 0, 1023, servo_min_duty, servo_max_duty);
  ledcWrite(servo_ledcch, duty);
}

// public
void C3superminiDRV8833Board::boardInit(board_cfg_t init_cfg)
{
  Serial.println("DRV8833 init");

  pinMode(MOTR_IN1_PIN, OUTPUT);
  pinMode(MOTR_IN2_PIN, OUTPUT);
  pinMode(MOTL_IN1_PIN, OUTPUT);
  pinMode(MOTL_IN2_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  ledcSetup(MOTR_IN1_LEDCCH, 1500, LEDC_TIMER_9_BIT);
  ledcAttachPin(MOTR_IN1_PIN, MOTR_IN1_LEDCCH);

  ledcSetup(MOTR_IN2_LEDCCH, 1500, LEDC_TIMER_9_BIT);
  ledcAttachPin(MOTR_IN2_PIN, MOTR_IN2_LEDCCH);

  ledcSetup(MOTL_IN1_LEDCCH, 1500, LEDC_TIMER_9_BIT);
  ledcAttachPin(MOTL_IN1_PIN, MOTL_IN1_LEDCCH);

  ledcSetup(MOTL_IN2_LEDCCH, 1500, LEDC_TIMER_9_BIT);
  ledcAttachPin(MOTL_IN2_PIN, MOTL_IN2_LEDCCH);

  pinMode(SERVOA_PIN, OUTPUT);
  ledcSetup(SERVOA_LEDCCH, 50, LEDC_TIMER_14_BIT);
  ledcAttachPin(SERVOA_PIN, SERVOA_LEDCCH);

  pinMode(POT_PIN, INPUT);
}

void C3superminiDRV8833Board::motRSetSpeed(int speed)
{
  setMotorSpeed(MOTR_IN1_LEDCCH, MOTR_IN2_LEDCCH, speed);
  return;
}

void C3superminiDRV8833Board::motLSetSpeed(int speed)
{
  setMotorSpeed(MOTL_IN1_LEDCCH, MOTL_IN2_LEDCCH, speed);
  return;
}

void C3superminiDRV8833Board::motWSetSpeed(int speed)
{
  return;
}

void C3superminiDRV8833Board::motWSeekPot(int angle)
{
  return;
}

void C3superminiDRV8833Board::servoASetAngle(int angle)
{
  setServoAngle(SERVOA_LEDCCH, angle);
}

void C3superminiDRV8833Board::servoBSetAngle(int angle)
{
  return;
}

void C3superminiDRV8833Board::failsafe()
{
  ledcWrite(SERVOA_LEDCCH, 0);
  motRSetSpeed(0);
  motLSetSpeed(0);
  return;
}

void C3superminiDRV8833Board::setLed(bool state)
{
  if (!state)
  {
    digitalWrite(LED_PIN, HIGH);
    return;
  }
  digitalWrite(LED_PIN, LOW);
}

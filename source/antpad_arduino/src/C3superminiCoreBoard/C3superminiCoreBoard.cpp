#include "hal/gpio_types.h"
#include "C3superminiCoreBoard.h"

// private

void C3superminiCoreBoard::setMotorSpeed(char en_ledc_ch, char ph_pin, int speed)
{
  speed = constrain(speed, -512, 512);
  if (speed < 0)
  {
    digitalWrite(ph_pin, LOW);
    ledcWrite(en_ledc_ch, -speed);
    return;
  }
  if (speed == 0)
  {
    digitalWrite(ph_pin, LOW);
    ledcWrite(en_ledc_ch, 0);
    return;
  }
  if (speed > 0)
  {
    digitalWrite(ph_pin, HIGH);
    ledcWrite(en_ledc_ch, speed);
    return;
  }
}

void C3superminiCoreBoard::setServoAngle(char servo_ledcch, int angle)
{ // TODO fix motwservo
  if (angle < 0)
  {
    ledcWrite(servo_ledcch, 0);
    return;
  }
  angle = constrain(angle, 0, 1023);
  int duty = map(angle, 0, 1023, servo_min_duty, servo_max_duty);
  ledcWrite(servo_ledcch, duty);
}

// public

void C3superminiCoreBoard::boardInit(board_cfg_t init_cfg)
{
  Serial.println("CORE init");
  gpio_reset_pin(GPIO_NUM_4);
  gpio_reset_pin(GPIO_NUM_5);

  pinMode(MOTR_EN_PIN, OUTPUT);
  pinMode(MOTR_PH_PIN, OUTPUT);

  pinMode(MOTL_EN_PIN, OUTPUT);
  pinMode(MOTL_PH_PIN, OUTPUT);

  pinMode(MOTW_EN_PIN, OUTPUT);
  pinMode(MOTW_PH_PIN, OUTPUT);

  digitalWrite(MOTR_EN_PIN, LOW);
  digitalWrite(MOTR_PH_PIN, LOW);

  digitalWrite(MOTL_EN_PIN, LOW);
  digitalWrite(MOTL_PH_PIN, LOW);

  digitalWrite(MOTW_EN_PIN, LOW);
  digitalWrite(MOTW_PH_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);

  ledcSetup(MOTR_EN_LEDCCH, 1500, LEDC_TIMER_9_BIT);
  ledcAttachPin(MOTR_EN_PIN, MOTR_EN_LEDCCH);

  ledcSetup(MOTL_EN_LEDCCH, 1500, LEDC_TIMER_9_BIT);
  ledcAttachPin(MOTL_EN_PIN, MOTL_EN_LEDCCH);

  ledcSetup(MOTW_EN_LEDCCH, 1500, LEDC_TIMER_9_BIT);
  ledcAttachPin(MOTW_EN_PIN, MOTW_EN_LEDCCH);

  pinMode(SERVOA_PIN, OUTPUT);
  ledcSetup(SERVOA_LEDCCH, 50, LEDC_TIMER_14_BIT);
  ledcAttachPin(SERVOA_PIN, SERVOA_LEDCCH);

  if (false)
  {
    pinMode(POT_PIN, INPUT);
  }
  else
  {
    pinMode(POT_PIN, OUTPUT);
    ledcSetup(SERVOB_LEDCCH, 50, LEDC_TIMER_14_BIT);
    ledcAttachPin(POT_PIN, SERVOB_LEDCCH);
  }
}

void C3superminiCoreBoard::motRSetSpeed(int speed)
{
  setMotorSpeed(MOTR_EN_LEDCCH, MOTR_PH_PIN, speed);
  return;
}

void C3superminiCoreBoard::motLSetSpeed(int speed)
{
  setMotorSpeed(MOTL_EN_LEDCCH, MOTL_PH_PIN, speed);
  return;
}

void C3superminiCoreBoard::motWSetSpeed(int speed)
{
  setMotorSpeed(MOTW_EN_LEDCCH, MOTW_PH_PIN, speed);
  return;
}

void C3superminiCoreBoard::motWSeekPot(int angle, int dc_dir)
{
  return;
}

void C3superminiCoreBoard::servoASetAngle(int angle)
{
  setServoAngle(SERVOA_LEDCCH, angle);
}

void C3superminiCoreBoard::servoBSetAngle(int angle)
{ // TODO fix motwservo
  if (false)
  {
    return;
  }
  else
  {
    setServoAngle(SERVOB_LEDCCH, angle);
  }
}

void C3superminiCoreBoard::failsafe()
{ // TODO fix motwservo
  ledcWrite(SERVOA_LEDCCH, 0);
  ledcWrite(SERVOB_LEDCCH, 0);
  motRSetSpeed(0);
  motLSetSpeed(0);
  motWSetSpeed(0);
  return;
}

void C3superminiCoreBoard::setLed(bool state)
{
  if (!state)
  {
    digitalWrite(LED_PIN, HIGH);
    return;
  }
  digitalWrite(LED_PIN, LOW);
}

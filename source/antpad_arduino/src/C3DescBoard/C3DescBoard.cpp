#include "C3DescBoard.h"

// private

// your private functions here

void C3DescBoard::setMotorSpeed(char motor_ledcch, int speed)
{
  speed = constrain(speed, -512, 512);
  if (speed < 0)
  {
    speed = -speed;
    int duty = map(speed, 0, 512, motor_center_duty, motor_max_duty);
    ledcWrite(motor_ledcch, duty);
    return;
  }
  if (speed > 0)
  {
    int duty = map(speed, 0, 512, motor_min_duty, motor_center_duty);
    ledcWrite(motor_ledcch, duty);
    return;
  }
  ledcWrite(motor_ledcch, motor_center_duty);
}

void C3DescBoard::setServoAngle(char servo_ledcch, int angle)
{
  angle = constrain(angle, 0, 1023);
  int duty = map(angle, 0, 1023, servo_min_duty, servo_max_duty);
  ledcWrite(servo_ledcch, duty);
}

// public
void C3DescBoard::boardInit(board_cfg_t init_cfg)
{
  Serial.println("CORE init");
  board_cfg = init_cfg;
  //Serial.println(board_cfg.dc_servo);
  //Serial.println(board_cfg.servo_stretcher);
  gpio_reset_pin(GPIO_NUM_4);
  gpio_reset_pin(GPIO_NUM_5);

  pinMode(MOTR_PIN, OUTPUT);
  pinMode(MOTL_PIN, OUTPUT);

  digitalWrite(MOTR_PIN, LOW);
  digitalWrite(MOTL_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);

  pinMode(SERVOA_PIN, OUTPUT);
  ledcSetup(SERVOA_LEDCCH, 50, LEDC_TIMER_14_BIT);
  ledcAttachPin(SERVOA_PIN, SERVOA_LEDCCH);

  pinMode(MOTR_PIN, OUTPUT);
  ledcSetup(MOTR_LEDCCH, 50, LEDC_TIMER_14_BIT);
  ledcAttachPin(MOTR_PIN, MOTR_LEDCCH);

  pinMode(MOTL_PIN, OUTPUT);
  ledcSetup(MOTL_LEDCCH, 50, LEDC_TIMER_14_BIT);
  ledcAttachPin(MOTL_PIN, MOTL_LEDCCH);

  
  if(board_cfg.servo_stretcher){
    servo_min_duty = 410;
    servo_max_duty = 2048;
  }
}

void C3DescBoard::motRSetSpeed(int speed)
{
  setMotorSpeed(MOTR_LEDCCH, speed);
  return;
}

void C3DescBoard::motLSetSpeed(int speed)
{
  setMotorSpeed(MOTL_LEDCCH, speed);
  return;
}

void C3DescBoard::motWSetSpeed(int speed)
{
  // move your weapon motor
  return;
}

void C3DescBoard::motWSeekPot(int angle, int dc_dir)
{
  // move your weapon motor so that it makes the potentometer
  // reach the target readings
  return;
}

void C3DescBoard::servoASetAngle(int angle)
{
  setServoAngle(SERVOA_LEDCCH, angle);
  return;
}

void C3DescBoard::servoBSetAngle(int angle)
{
  // move your ch4 linked servo
  return;
}

void C3DescBoard::failsafe()
{
  ledcWrite(SERVOA_LEDCCH, 0);
  ledcWrite(MOTL_LEDCCH, motor_center_duty);
  ledcWrite(MOTR_LEDCCH, motor_center_duty);
  return;
}

void C3DescBoard::setLed(bool state)
{
  // handle the debug led function used by firmware
  // statre = true -> led on
  if (!state)
  {
    digitalWrite(LED_PIN, HIGH);
    return;
  }
  digitalWrite(LED_PIN, LOW);
  return;
}

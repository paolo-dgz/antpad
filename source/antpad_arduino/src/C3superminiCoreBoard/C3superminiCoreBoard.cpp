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
{ 
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
  board_cfg = init_cfg;
  Serial.println(board_cfg.dc_servo);
  Serial.println(board_cfg.servo_stretcher);
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

  if (board_cfg.dc_servo)
  {
    pinMode(POT_PIN, INPUT);
  }
  else
  {
    pinMode(POT_PIN, OUTPUT);
    ledcSetup(SERVOB_LEDCCH, 50, LEDC_TIMER_14_BIT);
    ledcAttachPin(POT_PIN, SERVOB_LEDCCH);
  }
  
  if(board_cfg.servo_stretcher){
    servo_min_duty = 410;
    servo_max_duty = 2048;
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
  if (board_cfg.dc_servo)
  {
    return;
  }
  setMotorSpeed(MOTW_EN_LEDCCH, MOTW_PH_PIN, speed);
  return;
}

void C3superminiCoreBoard::motWSeekPot(int angle, int dc_dir)
{
  if (!board_cfg.dc_servo)
  {
    return;
  }

  if (angle < 0)
  {
    setMotorSpeed(MOTW_EN_LEDCCH, MOTW_PH_PIN, 0);
    return;
  }

  setMotorSpeed(MOTW_EN_LEDCCH, MOTW_PH_PIN, 0);
  angle = constrain(angle, 0, 1023);
  int current_pos = analogRead(POT_PIN);
  //Serial.println(current_pos);
  unsigned long pid_time = millis();
  float pid_error = angle - current_pos;
  float pid_prop = pid_error * pid_prop_k;
  float pid_deriv = (pid_error - prev_error) * pid_deriv_k;

  if (abs(pid_deriv) < 90.0)
  {
    pid_deriv = 0.0;
  }
  if (abs(pid_error) < 20.0)
  {
    pid_integral = pid_integral + pid_error * pid_integ_k;
  }
  else
  {
    pid_integral = 0;
  }
  if (abs(pid_error) < 7.0)
  {
    pid_integral = 0;
    pid_prop = 0;
    pid_deriv = 0;
  }
  int wpn_pwm = pid_prop + pid_deriv + pid_integral;
  setMotorSpeed(MOTW_EN_LEDCCH, MOTW_PH_PIN, wpn_pwm * dc_dir);

  // prev sets for next iteration
  prev_error = pid_error;
  /*
  //if(wpn_speed < 350) return 1;
  Serial.print(target_pos);
  Serial.print("\t");
  Serial.print(current_pos);
  Serial.print("\t");
  Serial.print(pid_error);
  Serial.print("\t");
  Serial.print(pid_prop);
  Serial.print("\t");
  Serial.print(pid_deriv);
  Serial.print("\t");
  Serial.print(pid_integral);
  Serial.print("\t");
  Serial.println(wpn_pwm);
  //*/
}

void C3superminiCoreBoard::servoASetAngle(int angle)
{
  setServoAngle(SERVOA_LEDCCH, angle);
}

void C3superminiCoreBoard::servoBSetAngle(int angle)
{ 
  if (board_cfg.dc_servo)
  {
    return;
  }
  setServoAngle(SERVOB_LEDCCH, angle);
}

void C3superminiCoreBoard::failsafe()
{ 
  servoASetAngle(-1);
  servoBSetAngle(-1);
  motWSeekPot(-1);

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

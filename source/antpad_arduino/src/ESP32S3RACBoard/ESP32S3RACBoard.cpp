#ifdef ARDUINO_ESP32S3_DEV
#include "ESP32S3RACBoard.h"

// private
void ESP32S3RACBoard::setMotorSpeed(mcpwm_unit_t unit, mcpwm_timer_t timer, int speed)
{
  /*
  Serial.print(unit);
  Serial.print("\t");
  Serial.print(timer);
  Serial.print("\t");
  Serial.println(speed);
  //*/
  speed = constrain(speed, -512, 512);

  float duty = ((float)abs(speed)) / 5.12f;
  if (speed < 0)
  {
    mcpwm_set_duty(unit, timer, MCPWM_GEN_A, 0);
    mcpwm_set_duty(unit, timer, MCPWM_GEN_B, duty);
    return;
  }
  if (speed == 0)
  {
    mcpwm_set_duty(unit, timer, MCPWM_GEN_A, 100);
    mcpwm_set_duty(unit, timer, MCPWM_GEN_B, 100);
    return;
  }
  if (speed > 0)
  {
    mcpwm_set_duty(unit, timer, MCPWM_GEN_A, duty);
    mcpwm_set_duty(unit, timer, MCPWM_GEN_B, 0);
    return;
  }
}

void ESP32S3RACBoard::setServoAngle(char servo_ledcch, int angle)
{
  angle = constrain(angle, 0, 1023);
  int duty = map(angle, 0, 1023, servo_min_duty, servo_max_duty);
  ledcWrite(servo_ledcch, duty);
}

// public
void ESP32S3RACBoard::boardInit(board_cfg_t init_cfg)
{
  Serial.println("DRV8833 init");
  board_cfg = init_cfg;

  pinMode(MOTR_IN1_PIN, OUTPUT);
  pinMode(MOTR_IN2_PIN, OUTPUT);
  pinMode(MOTL_IN1_PIN, OUTPUT);
  pinMode(MOTL_IN2_PIN, OUTPUT);
  pinMode(MOTW_IN1_PIN, OUTPUT);
  pinMode(MOTW_IN2_PIN, OUTPUT);
  pinMode(MOTW2_IN1_PIN, OUTPUT);
  pinMode(MOTW2_IN2_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);

  mcpwm_config_t pwm_config;
  pwm_config.frequency = 980;
  pwm_config.cmpr_a = 0;
  pwm_config.cmpr_b = 0;
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

  mcpwm_gpio_init(MOTR_PWM_UNIT, MCPWM0A, MOTR_IN1_PIN);
  mcpwm_gpio_init(MOTR_PWM_UNIT, MCPWM0B, MOTR_IN2_PIN);
  mcpwm_init(MOTR_PWM_UNIT, MOTR_PWM_TIMER, &pwm_config);

  mcpwm_gpio_init(MOTL_PWM_UNIT, MCPWM1A, MOTL_IN1_PIN);
  mcpwm_gpio_init(MOTL_PWM_UNIT, MCPWM1B, MOTL_IN2_PIN);
  mcpwm_init(MOTL_PWM_UNIT, MOTL_PWM_TIMER, &pwm_config);

  mcpwm_gpio_init(MOTW_PWM_UNIT, MCPWM2A, MOTW_IN1_PIN);
  mcpwm_gpio_init(MOTW_PWM_UNIT, MCPWM2B, MOTW_IN2_PIN);
  mcpwm_init(MOTW_PWM_UNIT, MOTW_PWM_TIMER, &pwm_config);

  mcpwm_gpio_init(MOTW2_PWM_UNIT, MCPWM0A, MOTW2_IN1_PIN);
  mcpwm_gpio_init(MOTW2_PWM_UNIT, MCPWM0B, MOTW2_IN2_PIN);
  mcpwm_init(MOTW2_PWM_UNIT, MOTW2_PWM_TIMER, &pwm_config);

  pinMode(SERVOA_PIN, OUTPUT);
  ledcSetup(SERVOA_LEDCCH, 50, LEDC_TIMER_14_BIT);
  ledcAttachPin(SERVOA_PIN, SERVOA_LEDCCH);
}

void ESP32S3RACBoard::motRSetSpeed(int speed)
{
  setMotorSpeed(MOTR_PWM_UNIT, MOTR_PWM_TIMER, speed);
  return;
}

void ESP32S3RACBoard::motLSetSpeed(int speed)
{
  setMotorSpeed(MOTL_PWM_UNIT, MOTL_PWM_TIMER, speed);
  return;
}

void ESP32S3RACBoard::motWSetSpeed(int speed)
{
  if (board_cfg.dc_servo)
  {
    return;
  }
  setMotorSpeed(MOTW_PWM_UNIT, MOTW_PWM_TIMER, speed);
  return;
}

void ESP32S3RACBoard::motWSeekPot(int angle, int dc_dir)
{
  if (!board_cfg.dc_servo || angle < 0)
  {
    setMotorSpeed(MOTW_PWM_UNIT, MOTW_PWM_TIMER, 0);
    return;
  }
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
  setMotorSpeed(MOTW_PWM_UNIT, MOTW_PWM_TIMER, wpn_pwm*dc_dir);

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

void ESP32S3RACBoard::motW2SetSpeed(int speed)
{
  //Serial.println(speed);
  setMotorSpeed(MOTW2_PWM_UNIT, MOTW2_PWM_TIMER, speed);
  return;
}

void ESP32S3RACBoard::servoASetAngle(int angle)
{
  setServoAngle(SERVOA_LEDCCH, angle);
}

void ESP32S3RACBoard::servoBSetAngle(int angle)
{
  return;
}

void ESP32S3RACBoard::failsafe()
{
  ledcWrite(SERVOA_LEDCCH, 0);
  motRSetSpeed(0);
  motLSetSpeed(0);
  motWSetSpeed(0);
  motW2SetSpeed(0);
  return;
}

void ESP32S3RACBoard::setLed(bool state)
{
  if (!state)
  {
    digitalWrite(LED_PIN, LOW);
    return;
  }
  digitalWrite(LED_PIN, HIGH);
}

#endif
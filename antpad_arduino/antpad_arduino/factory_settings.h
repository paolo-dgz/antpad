struct remote_config_t {
  int8_t ch1_reverse = 1;
  int8_t ch2_reverse = 1;
  int8_t ch3_reverse = -1;
  int8_t ch4_reverse = 1;
  bool left_stick = false;
  bool ch3_bdir = false;
  bool tank_drive = false;
};

struct board_config_t {
  int16_t servo_a_min = 0;
  int16_t servo_a_max = 1023;

  int16_t servo_b_min = 0;
  int16_t servo_b_max = 1023;

  int8_t motr_reverse = -1;
  int8_t motl_reverse = 1;
  int8_t motw_reverse = 1;

  bool servo_a_reverse = false;
  bool servo_b_reverse = false;

  bool motw_servo = false;
  bool servo_stretcher = false;
  bool swap_motor = true;
};
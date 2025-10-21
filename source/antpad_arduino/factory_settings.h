bool factory_reset = false;

struct remote_config_t { //configs that can be applied live
  bool swap_sticks = false;
  bool ch3_centered = false;
  bool swap_lr_motors = false;
  bool servo_0_reverse = false;
  bool servo_1_reverse = false;

  int8_t ch1_dir = 1;
  int8_t ch2_dir = 1;
  int8_t ch3_dir = 1;
  int8_t ch4_dir = 1;

  int8_t motr_dir = -1;
  int8_t motl_dir = 1;
  int8_t motw_dir = 1;

  int16_t servo_mins[2] = {0,0};
  int16_t servo_maxs[2] = {1023,1023};
};

struct board_config_t { //config that need a restart to be applied
  bool dc_servo = false;
  bool servo_stretcher = false;
};

struct channel_map_t{
  uint8_t steering = 1;
  uint8_t accelerator = 2; 
  uint8_t servo_0 = 3;
  uint8_t servo_1 = 4;
  uint8_t motor_r = 0;
  uint8_t motor_l = 0;
  uint8_t motor_w = 4;
};

uint8_t ControllerAddress[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
remote_config_t RemoteConfig;
board_config_t BoardConfig;
channel_map_t ChannelMap;
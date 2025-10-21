#include <Bluepad32.h>
#include <EEPROM.h>
#include "src/ledUtility/ledUtility.h"
#include "eeprom_utils.h"

//#define C3SUPERMINI_CORE
//#define C3SUPERMINI_DRV8833
#define ESP32S3_RAC50
//#define CUSTOM_BOARD

/*
naming convention
ALL_UPPER_CASE_WITH_UNDERSCORES for global constants.
CamelCaseWithInitialCapital for global functions and variables.
camelCaseWithInitialLowercase for local variables and class members (both functions and variables).
*/

#ifdef C3SUPERMINI_CORE
#include "src/C3superminiCoreBoard/C3superminiCoreBoard.h"
C3superminiCoreBoard RobotBoard;
#endif

#ifdef C3SUPERMINI_DRV8833
#include "src/C3superminiDrv8833Board/C3superminiDrv8833Board.h"
C3superminiDRV8833Board RobotBoard;
#endif

#ifdef ESP32S3_RAC50
#include "src/ESP32S3RAC50Board/ESP32S3RAC50Board.h"
ESP32S3RAC50Board RobotBoard;
#endif

#ifdef CUSTOM_BOARD
#include "src/CustomBoard/CustomBoard.h"
CustomBoard RobotBoard;
#endif

LedUtility LedTask = LedUtility(&RobotBoard);



//global vars

String AntpadVersion = "0.1.0";
ControllerPtr RemoteController;
unsigned long CurrentMs = 0;
int ch_vals[7] = { 0, 0, 0, 0, 0, 0, 0 };

bool binding = true;
bool failsafe = true;
bool connection_ok = false;

int motRSpeed = 0;
int motLSpeed = 0;
int motWSpeed = 0;

int servoAAngle = 0;
int servoBAngle = 0;

//generic Menu vars

enum MenuCmd { CMD_UP,
               CMD_DOWN,
               CMD_LEFT,
               CMD_RIGHT,
               CMD_X,
               CMD_Y,
               CMD_A,
               CMD_B,
               CMD_AMOUNT,
               CMD_NONE };
bool CmdStates[CMD_AMOUNT];
bool CmdStatesPrev[CMD_AMOUNT];

MenuCmd CmdLocked = CMD_NONE;
MenuCmd CmdTriggered = CMD_NONE;
unsigned long CmdLockMs = 0;

enum MenuState { MENU_LIST,
                 MENU_SERVOS,
                 MENU_MOTORS,
                 MENU_REMOTE,
                 MENU_BOARD,
                 MENU_CHANNELS,
                 MENU_RESET,
                 MENU_NONE };

MenuState MenuStateCurrent = MENU_NONE;
MenuState MenuStateNext = MENU_LIST;
bool disable_movements = false;

//menu list vars
uint8_t MenuListItem = MENU_LIST;
uint8_t MenuListMax = MENU_RESET;
uint8_t MenuListMin = MENU_SERVOS;
bool MenuSaveRemote = false;
bool MenuSaveBoard = false;

//menu servo vars
uint8_t MenuCurrentServo = 0;
uint8_t MenuCurrentServoEPA = 0;




// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  ControllerProperties properties = ctl->getProperties();
  Serial.printf("NEW => CONNECT: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", properties.btaddr[0], properties.btaddr[1], properties.btaddr[2], properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);
  Serial.printf("OLD => VALID: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", ControllerAddress[0], ControllerAddress[1], ControllerAddress[2], ControllerAddress[3], ControllerAddress[4], properties.btaddr[5]);
  bool valid_controller = true;

  if (MacEepromValid) {
    for (int b = 0; b < sizeof(ControllerAddress); b++) {
      if (ControllerAddress[b] != properties.btaddr[b]) {
        Serial.println("WRONG => DSCONNECT");
        ctl->disconnect();
        return;
      }
    }
  } else {
    SaveMac(properties.btaddr);
    LoadMac(ControllerAddress);
    Serial.printf("NEW => SAVED: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", ControllerAddress[0], ControllerAddress[1], ControllerAddress[2], ControllerAddress[3], ControllerAddress[4], ControllerAddress[5]);
  }
  RemoteController = ctl;
  failsafe = false;
  connection_ok = true;
  BP32.enableNewBluetoothConnections(false);
  Serial.println("VALID => LOCKED");
}

void onDisconnectedController(ControllerPtr ctl) {
  ControllerProperties properties = ctl->getProperties();
  Serial.printf("DISCONNECT: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", properties.btaddr[0], properties.btaddr[1], properties.btaddr[2], properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);
  bool valid_controller = true;

  if (MacEepromValid) {
    for (int b = 0; b < sizeof(ControllerAddress); b++) {
      if (ControllerAddress[b] != properties.btaddr[b]) {
        valid_controller = false;
        return;
      }
    }
  }
  failsafe = true;
  BP32.enableNewBluetoothConnections(true);
  Serial.println("VALID => UNLOCKED");
  RemoteController = nullptr;
}

void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
    "idx=%d, dpad: 0x%02x, buttons: 0x%02x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, misc: 0x%02x",
    ctl->index(),       // Controller Index
    ctl->dpad(),        // D-pad
    ctl->buttons(),     // bitmask of pressed buttons
    ctl->axisX(),       // (-511 - 512) left X Axis
    ctl->axisY(),       // (-511 - 512) left Y axis
    ctl->axisRX(),      // (-511 - 512) right X axis
    ctl->axisRY(),      // (-511 - 512) right Y axis
    ctl->brake(),       // (0 - 1023): brake button
    ctl->throttle(),    // (0 - 1023): throttle (AKA gas) button
    ctl->miscButtons()  // bitmask of pressed "misc" buttons
  );
  Serial.println();
}

void processController() {
  //dumpGamepad(RemoteController);

  ch_vals[1] = constrain(-RemoteController->axisRY() * RemoteConfig.ch1_dir, -508, 508);
  ch_vals[2] = constrain(RemoteController->axisRX() * RemoteConfig.ch2_dir, -508, 508);
  ch_vals[3] = constrain(-RemoteController->axisY() * RemoteConfig.ch3_dir, -508, 508);
  ch_vals[4] = constrain(RemoteController->axisX() * RemoteConfig.ch4_dir, -508, 508);
  ch_vals[5] = 508 * RemoteController->l1() - 508 * RemoteController->r1();
  ch_vals[6] = RemoteController->throttle() - RemoteController->brake();

  if (RemoteConfig.swap_sticks) {
    ch_vals[3] = constrain(-RemoteController->axisRY() * RemoteConfig.ch1_dir, -508, 508);
    ch_vals[4] = constrain(RemoteController->axisRX() * RemoteConfig.ch2_dir, -508, 508);
    ch_vals[1] = constrain(-RemoteController->axisY() * RemoteConfig.ch3_dir, -508, 508);
    ch_vals[2] = constrain(RemoteController->axisX() * RemoteConfig.ch4_dir, -508, 508);
    ch_vals[5] = 508 * RemoteController->r1() - 508 * RemoteController->l1();
    ch_vals[6] = RemoteController->throttle() - RemoteController->brake();
  }


  if (abs(ch_vals[1]) < 50) {
    ch_vals[1] = 0;
  }
  if (abs(ch_vals[2]) < 50) {
    ch_vals[2] = 0;
  }
  if (abs(ch_vals[3]) < 50) {
    ch_vals[3] = 0;
  }
  if (abs(ch_vals[4]) < 50) {
    ch_vals[4] = 0;
  }

  if (abs(ch_vals[6]) < 50) {
    ch_vals[6] = 0;
  }

  if(ch_vals[5] != 0){
    ch_vals[3] = ch_vals[5];
  }

  if (!RemoteConfig.ch3_centered) {
    int temp_ch3 = ch_vals[3];
    temp_ch3 = constrain(temp_ch3, 0, 508);
    temp_ch3 = map(temp_ch3, 0, 508, -508, 508);
    ch_vals[3] = temp_ch3;
  }
  

  int steeringVal = ChannelMap.accelerator != 0 ? ch_vals[ChannelMap.accelerator] : 0;
  int acceleratorVal = ChannelMap.steering != 0 ? ch_vals[ChannelMap.steering] : 0;
  int servoAVal = ChannelMap.servo_a != 0 ? ch_vals[ChannelMap.servo_a] : 0;
  int servoBVal = ChannelMap.servo_b != 0 ? ch_vals[ChannelMap.servo_b] : 0;
  int motorRVal = ChannelMap.motor_r != 0 ? ch_vals[ChannelMap.motor_r] : 0;
  int motorLVal = ChannelMap.motor_l != 0 ? ch_vals[ChannelMap.motor_l] : 0;
  int motorWVal = ChannelMap.motor_w != 0 ? ch_vals[ChannelMap.motor_w] : 0;




  int leftThrottle = acceleratorVal + steeringVal;
  int rightThrottle = acceleratorVal - steeringVal;

  // in some cases the max is 100, in some cases it is 200
  // let's factor in the difference so the max is always 200
  int diff = abs(abs(acceleratorVal) - abs(steeringVal));
  leftThrottle = leftThrottle < 0 ? leftThrottle - diff : leftThrottle + diff;
  rightThrottle = rightThrottle < 0 ? rightThrottle - diff : rightThrottle + diff;

  //Map from ±200 to ± 100 or whatever range you need
  leftThrottle = map(leftThrottle, -1016, 1016, -512, 512);
  rightThrottle = map(rightThrottle, -1016, 1016, -512, 512);

  //constrain

  motRSpeed = constrain(rightThrottle, -512, 512);
  motLSpeed = constrain(leftThrottle, -512, 512);



  //weaps
  int temp_min = RemoteConfig.servo_mins[0];
  int temp_max = RemoteConfig.servo_maxs[0];

  if (RemoteConfig.servo_a_reverse) {
    temp_min = RemoteConfig.servo_mins[0];
    temp_max = RemoteConfig.servo_maxs[0];
  }

  servoAAngle = map(servoAVal, -508, 508, temp_min, temp_max);


  temp_min = RemoteConfig.servo_mins[1];
  temp_max = RemoteConfig.servo_maxs[1];

  if (RemoteConfig.servo_a_reverse) {
    temp_min = RemoteConfig.servo_mins[1];
    temp_max = RemoteConfig.servo_maxs[1];
  }

  servoBAngle = map(servoBVal, -508, 508, temp_min, temp_max);

  motWSpeed = map(motorWVal, -508, 508, -512, 512);
  motWSpeed = constrain(motWSpeed, -512, 512);
}

void processBoard() {
  /*
  Serial.print("  R:\t");
  Serial.print(motRSpeed);
  Serial.print("  L:\t");
  Serial.print(motLSpeed);
  Serial.print("  W:\t");
  Serial.print(motWSpeed);
  Serial.print("  A:\t");
  Serial.print(servoAAngle);
  Serial.print("  B:\t");
  Serial.println(servoBAngle);
  //*/
  if (failsafe || disable_movements) {
    motRSpeed = 0;
    motLSpeed = 0;
    motWSpeed = 0;
    servoAAngle = -1;
    servoBAngle = -1;
    RobotBoard.failsafe();
    return;
  }

  RobotBoard.motLSetSpeed(motLSpeed * RemoteConfig.motl_dir);
  RobotBoard.motRSetSpeed(motRSpeed * RemoteConfig.motr_dir);

  if (BoardConfig.dc_servo) {
    RobotBoard.motWSeekPot(servoAAngle, RemoteConfig.motw_dir);
    RobotBoard.servoASetAngle(servoBAngle);
  } else {
    RobotBoard.servoASetAngle(servoAAngle);
    RobotBoard.servoBSetAngle(servoBAngle);
    RobotBoard.motWSetSpeed(motWSpeed * RemoteConfig.motw_dir);
  }
}


void check_mode() {
  unsigned long current_time = millis();
  if (!connection_ok) {
    if (current_time > 60000 && binding == false) {
      Serial.println("ENTERED BINDING");
      MenuStateCurrent = MENU_NONE;
      MacEepromValid = false;
      binding = true;
      return;
    }
    if (current_time > 6000 && MenuStateCurrent == MENU_NONE && current_time < 59000) {
      Serial.println("ENTERED SETTINGS");
      MenuStateCurrent = MENU_LIST;
      return;
    }
  } else {
    binding = false;
  }
}


void handle_blink() {
  if (binding) {
    LedTask.setBlinks(2, 500, 10);
    return;
  }
  if (MenuStateCurrent != MENU_NONE) {
    switch (MenuStateCurrent) {
      case MENU_LIST:
        if (MenuListItem > 0) {
          LedTask.setBlinks(MenuListItem, 500, 2);
        } else {
          LedTask.setBlinks(1, -1, 5);
        }
        break;
    }
    return;
  }
  if (failsafe) {
    LedTask.setBlinks(1, -1, 1);
    return;
  }
  LedTask.ledOn();
}

MenuCmd getControllerCmd() {
  MenuCmd resultMenuCmd = CMD_NONE;
  CmdStates[CMD_UP] = RemoteController->dpad() & 0b1;
  CmdStates[CMD_DOWN] = RemoteController->dpad() & 0b10;
  CmdStates[CMD_LEFT] = RemoteController->dpad() & 0b1000;
  CmdStates[CMD_RIGHT] = RemoteController->dpad() & 0b100;
  CmdStates[CMD_A] = RemoteController->a();
  CmdStates[CMD_B] = RemoteController->b();
  CmdStates[CMD_Y] = RemoteController->y();
  CmdStates[CMD_X] = RemoteController->x();

  if (CmdLocked == CMD_NONE) {
    for (uint8_t cmd_i; cmd_i < CMD_AMOUNT; cmd_i++) {
      /*
      Serial.print(cmd_i);
      Serial.print("->");
      Serial.print(CmdStatesPrev[cmd_i]);
      Serial.print(">");
      Serial.print(CmdStates[cmd_i]);
      Serial.print(",  ");
      //*/
      if (!CmdStatesPrev[cmd_i] && CmdStates[cmd_i]) {
        CmdLocked = (MenuCmd)cmd_i;
        CmdLockMs = CurrentMs;
        break;
      }
    }
    //Serial.println();
  } else {
    //Serial.print(CurrentMs - CmdLockMs);
    //Serial.print(",  ");
    //Serial.println(CmdLocked);
    if (!CmdStates[CmdLocked]) {
      if (CurrentMs - CmdLockMs > 100) {
        resultMenuCmd = CmdLocked;
      }
      CmdLocked = CMD_NONE;
      CmdLockMs = 0;
    }
  }

  for (uint8_t cmd_i; cmd_i < CMD_AMOUNT; cmd_i++) {
    CmdStatesPrev[cmd_i] = CmdStates[cmd_i];
  }
  return resultMenuCmd;
}

void processMenuState(MenuCmd cmd) {
  //handle state code
  switch (MenuStateCurrent) {
    case MENU_NONE:
      disable_movements = false;
      return;
      break;
    case MENU_LIST:
      disable_movements = true;
      switch (cmd) {
        case CMD_DOWN:
          MenuListItem = MenuListItem + 1;
          MenuListItem = constrain(MenuListItem, MenuListMin, MenuListMax);
          break;
        case CMD_UP:
          MenuListItem = (MenuListMax + MenuListItem - 1) % MenuListMax;
          MenuListItem = constrain(MenuListItem, MenuListMin, MenuListMax);
          break;
      }
      break;
    case MENU_SERVOS:
      disable_movements = false;
      switch (cmd) {
        case CMD_DOWN:
          MenuCurrentServo = 0;
          break;
        case CMD_UP:
          MenuCurrentServo = 1;
          break;
        case CMD_LEFT:
          if (MenuCurrentServoEPA) {
            int temp_angle = RemoteConfig.servo_mins[MenuCurrentServo];
            temp_angle = temp_angle - 10;
            temp_angle = constrain(temp_angle, 0, RemoteConfig.servo_maxs[MenuCurrentServo] - 20);
            RemoteConfig.servo_mins[MenuCurrentServo] = temp_angle;
          } else {
            int temp_angle = RemoteConfig.servo_maxs[MenuCurrentServo];
            temp_angle = temp_angle - 10;
            temp_angle = constrain(temp_angle, RemoteConfig.servo_mins[MenuCurrentServo] + 20, 1023);
            RemoteConfig.servo_maxs[MenuCurrentServo] = temp_angle;
          }
          break;
        case CMD_RIGHT:
          if (MenuCurrentServoEPA) {
            int temp_angle = RemoteConfig.servo_mins[MenuCurrentServo];
            temp_angle = temp_angle + 10;
            temp_angle = constrain(temp_angle, 0, RemoteConfig.servo_maxs[MenuCurrentServo] - 20);
            RemoteConfig.servo_mins[MenuCurrentServo] = temp_angle;
          } else {
            int temp_angle = RemoteConfig.servo_maxs[MenuCurrentServo];
            temp_angle = temp_angle + 10;
            temp_angle = constrain(temp_angle, RemoteConfig.servo_mins[MenuCurrentServo] + 20, 1023);
            RemoteConfig.servo_maxs[MenuCurrentServo] = temp_angle;
          }
          break;
        case CMD_X:
          MenuCurrentServoEPA = (MenuCurrentServoEPA + 1) % 2;
          break;
        case CMD_Y:
          if (MenuCurrentServo) {
            RemoteConfig.servo_a_reverse = !RemoteConfig.servo_a_reverse;
          } else {
            RemoteConfig.servo_b_reverse = !RemoteConfig.servo_b_reverse;
          }
          break;
      }

      if (MenuCurrentServo) {
        if (MenuCurrentServoEPA) {
          servoBAngle = RemoteConfig.servo_mins[MenuCurrentServo];
        } else {
          servoBAngle = RemoteConfig.servo_maxs[MenuCurrentServo];
        }
      } else {
        if (MenuCurrentServoEPA) {
          servoAAngle = RemoteConfig.servo_mins[MenuCurrentServo];
        } else {
          servoAAngle = RemoteConfig.servo_maxs[MenuCurrentServo];
        }
      }

      break;
    case MENU_MOTORS:
      disable_movements = false;

      switch (cmd) {
        case CMD_UP:
          RemoteConfig.motw_dir = RemoteConfig.motw_dir * -1;
          break;
        case CMD_LEFT:
          RemoteConfig.motl_dir = RemoteConfig.motl_dir * -1;
          break;
        case CMD_RIGHT:
          RemoteConfig.motr_dir = RemoteConfig.motr_dir * -1;
          break;
        case CMD_Y:
          RemoteConfig.swap_lr_motors = !RemoteConfig.swap_lr_motors;
          break;
      }
      break;
    case MENU_REMOTE:
      disable_movements = false;
      switch (cmd) {
        case CMD_DOWN:
          RemoteConfig.ch1_dir = RemoteConfig.ch1_dir * -1;
          break;
        case CMD_UP:
          RemoteConfig.ch3_dir = RemoteConfig.ch3_dir * -1;
          break;
        case CMD_LEFT:
          RemoteConfig.ch4_dir = RemoteConfig.ch4_dir * -1;
          break;
        case CMD_RIGHT:
          RemoteConfig.ch2_dir = RemoteConfig.ch2_dir * -1;
          break;
        case CMD_Y:
          RemoteConfig.swap_sticks = !RemoteConfig.swap_sticks;
          break;
        case CMD_X:
          RemoteConfig.ch3_centered = !RemoteConfig.ch3_centered;
          break;
      }
      break;
    case MENU_BOARD:
      disable_movements = true;
      switch (cmd) {
        case CMD_DOWN:
          BoardConfig.dc_servo = false;
          break;
        case CMD_UP:
          BoardConfig.dc_servo = true;
          break;
        case CMD_LEFT:
          BoardConfig.servo_stretcher = false;
          break;
        case CMD_RIGHT:
          BoardConfig.servo_stretcher = true;
          break;
      }
      break;
  }

  //handle transitions between states
  switch (MenuStateCurrent) {
    case MENU_NONE:
      return;
      break;
    case MENU_LIST:
      switch (cmd) {
        case CMD_A:
          MenuStateNext = (MenuState)MenuListItem;
          break;
        case CMD_B:
          MenuStateNext = MENU_NONE;
          break;
      }
      break;
    case MENU_SERVOS:
    case MENU_MOTORS:
    case MENU_REMOTE:
      switch (cmd) {
        case CMD_A:
          SaveRemoteConfig(&RemoteConfig);
          MenuStateNext = MENU_LIST;
          break;
        case CMD_B:
          ESP.restart();
          MenuStateNext = MENU_LIST;
          break;
      }
      break;
    case MENU_BOARD:
      switch (cmd) {
        case CMD_A:
          SaveBoardConfig(&BoardConfig);
          ESP.restart();
          MenuStateNext = MENU_LIST;
          break;
        case CMD_B:
          ESP.restart();
          MenuStateNext = MENU_LIST;
          break;
      }
      break;
    case MENU_CHANNELS:
      switch (cmd) {
        case CMD_A:
          MenuStateNext = MENU_LIST;
          break;
        case CMD_B:
          MenuStateNext = MENU_LIST;
          break;
      }
      break;
    case MENU_RESET:
      switch (cmd) {
        case CMD_A:
          ClearEeprom();
          ESP.restart();
          MenuStateNext = MENU_LIST;
          break;
        case CMD_B:
          MenuStateNext = MENU_LIST;
          break;
      }
      break;
  }
  if (cmd != CMD_NONE) {
    Serial.print(MenuStateCurrent);
    Serial.print("->");
    Serial.print(cmd);
    Serial.print("->");
    Serial.println(MenuStateNext);
    Serial.print(RemoteConfig.servo_maxs[MenuCurrentServo]);
    Serial.print(" > ");
    Serial.println(RemoteConfig.servo_mins[MenuCurrentServo]);
    Serial.print("S:");
    Serial.print(MenuCurrentServo);
    Serial.print(" > E:");
    Serial.println(MenuCurrentServoEPA);
    Serial.print("A:");
    Serial.print(servoAAngle);
    Serial.print(" > B:");
    Serial.println(servoBAngle);
    Serial.println();
  }

  MenuStateCurrent = MenuStateNext;
  CmdTriggered = CMD_NONE;
}


// Arduino setup function. Runs in CPU 1
void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  Serial.printf("Antpad: %s\n", AntpadVersion);
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);


  LedTask.init();
  InitEeprom();
  if(MacEepromValid){
    binding = false;
  }
  Board::board_cfg_t init_cfg;
  init_cfg.dc_servo = BoardConfig.dc_servo;
  init_cfg.servo_stretcher = BoardConfig.servo_stretcher;
  RobotBoard.boardInit(init_cfg);
  for (int cmd_i; cmd_i < CMD_AMOUNT; cmd_i++) {
    CmdStates[cmd_i] = false;
    CmdStatesPrev[cmd_i] = false;
  }

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But it might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();

  // Enables mouse / touchpad support for gamepads that support them.
  // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
  // - First one: the gamepad
  // - Second one, which is a "virtual device", is a mouse.
  // By default, it is disabled.
  BP32.enableVirtualDevice(false);
  analogReadResolution(11);
  analogSetAttenuation(ADC_11db);
}

// Arduino loop function. Runs in CPU 1.
void loop() {
  // This call fetches all the controllers' data.
  // Call this function in your main loop.
  bool dataUpdated = BP32.update();
  CurrentMs = millis();
  if (RemoteController != nullptr && !failsafe) {
    if (dataUpdated) {
      processController();
      CmdTriggered = getControllerCmd();
    }
  }
  processMenuState(CmdTriggered);
  processBoard();

  // The main loop must have some kind of "yield to lower priority task" event.
  // Otherwise, the watchdog will get triggered.
  // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
  // Detailed info here:
  // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

  vTaskDelay(1);
  check_mode();
  handle_blink();
  delay(10);
}

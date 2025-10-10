#include <Bluepad32.h>
#include <EEPROM.h>
#include "src/ledUtility/ledUtility.h"
#include "factory_settings.h"


#define C3SUPERMINI_CORE
//#define C3SUPERMINI_DRV8833
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

#ifdef CUSTOM_BOARD
#include "src/CustomBoard/CustomBoard.h"
CustomBoard RobotBoard;
#endif

LedUtility LedTask = LedUtility(&RobotBoard);

const byte EEPROM_VALID_BYTE = RobotBoard.EEPROM_VALID_BYTE;
const byte CLEAR_BYTE = 0;


//global vars
bool MacEepromValid = false;
bool CfgRemoteEepromValid = false;
bool CfgBoardEepromValid = false;

bool setting = false;
bool binding = true;


ControllerPtr RemoteController;
uint8_t ControllerAddress[6];
remote_config_t RemoteConfig;
board_config_t BoardConfig;
bool failsafe = true;
bool connection_ok = false;

int motRSpeed = 0;
int motLSpeed = 0;
int motWSpeed = 0;

int servoAAngle = 0;
int servoBAngle = 0;

const uint8_t ADDR_MAC_VALID = 0;
const uint8_t ADDR_MAC = ADDR_MAC_VALID + sizeof(EEPROM_VALID_BYTE);
const uint8_t ADDR_RMTCONFIG_VALID = ADDR_MAC + sizeof(ControllerAddress);
const uint8_t ADDR_RMTCONFIG = ADDR_RMTCONFIG_VALID + sizeof(EEPROM_VALID_BYTE);
const uint8_t ADDR_BOARDCONFIG_VALID = ADDR_RMTCONFIG + sizeof(RemoteConfig);
const uint8_t ADDR_BOARDCONFIG = ADDR_BOARDCONFIG_VALID + sizeof(EEPROM_VALID_BYTE);

bool cmd_up = false;
bool cmd_down = false;
bool cmd_enter = false;
bool cmd_back = false;

bool state_up = false;
bool state_down = false;
bool state_enter = false;
bool state_back = false;

bool state_up_pre = false;
bool state_down_pre = false;
bool state_enter_pre = false;
bool state_back_pre = false;

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
        valid_controller = false;
        break;
      }
    }
  } else {
    EEPROM.writeBytes(ADDR_MAC_VALID, &EEPROM_VALID_BYTE, sizeof(EEPROM_VALID_BYTE));
    EEPROM.writeBytes(ADDR_MAC, properties.btaddr, sizeof(ControllerAddress));
    EEPROM.commit();
    memcpy(ControllerAddress, properties.btaddr, sizeof(ControllerAddress));
    MacEepromValid = true;
    Serial.printf("NEW => SAVED: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", ControllerAddress[0], ControllerAddress[1], ControllerAddress[2], ControllerAddress[3], ControllerAddress[4], ControllerAddress[5]);
  }

  if (valid_controller) {
    failsafe = false;
    RemoteController = ctl;
    connection_ok = true;
    BP32.enableNewBluetoothConnections(false);
    Serial.println("VALID => LOCKED");
  } else {
    Serial.println("WRONG => DSCONNECT");
    ctl->disconnect();
    ;
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  ControllerProperties properties = ctl->getProperties();
  Serial.printf("DISCONNECT: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", properties.btaddr[0], properties.btaddr[1], properties.btaddr[2], properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);
  bool valid_controller = true;

  if (MacEepromValid) {
    for (int b = 0; b < sizeof(ControllerAddress); b++) {
      if (ControllerAddress[b] != properties.btaddr[b]) {
        valid_controller = false;
        break;
      }
    }
  }
  if (valid_controller) {
    failsafe = true;
    BP32.enableNewBluetoothConnections(true);
    Serial.println("VALID => UNLOCKED");
    RemoteController = nullptr;
  }
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

int convertThrottle(int speed) {
  if (speed = 0) {
    return 0;
  }
  if (speed > 0) {
    return speed;
  }
  if (speed < 0) {
    return 1047 - speed;
  }
}


void processController() {
  //dumpGamepad(RemoteController);

  //int angle = RemoteController->axisY()+512;
  //servoa_set_angle(angle);
  //ServoASetAngle(1023);



  int joystick_y = -RemoteController->axisRY();
  int joystick_x = RemoteController->axisRX();
  int ch3 = RemoteController->axisY();
  int ch4 = RemoteController->axisX();
  bool ch3_full = RemoteController->l1();

  if (RemoteConfig.left_stick) {
    joystick_y = -RemoteController->axisY();
    joystick_x = RemoteController->axisX();
    int ch3 = RemoteController->axisRY();
    int ch4 = RemoteController->axisRX();
  }

  ch3 = ch3 * RemoteConfig.ch3_reverse;
  ch4 = ch4 * RemoteConfig.ch4_reverse;


  //setups
  joystick_y = constrain(joystick_y, -508, 508);
  joystick_x = constrain(joystick_x, -508, 508);

  if (RemoteConfig.ch3_bdir) {
    ch3 = constrain(ch3, -508, 508);
  } else {
    ch3 = constrain(ch3, 0, 508);
  }

  if (ch3_full) {
    ch3 = 508;
  }
  ch4 = constrain(ch4, -508, 508);

  int leftThrottle = joystick_y + joystick_x;
  int rightThrottle = joystick_y - joystick_x;

  // in some cases the max is 100, in some cases it is 200
  // let's factor in the difference so the max is always 200
  int diff = abs(abs(joystick_y) - abs(joystick_x));
  leftThrottle = leftThrottle < 0 ? leftThrottle - diff : leftThrottle + diff;
  rightThrottle = rightThrottle < 0 ? rightThrottle - diff : rightThrottle + diff;

  //Map from ±200 to ± 100 or whatever range you need
  leftThrottle = map(leftThrottle, -1016, 1016, -512, 512);
  rightThrottle = map(rightThrottle, -1016, 1016, -512, 512);

  //constrain

  motRSpeed = constrain(rightThrottle, -512, 512);
  motLSpeed = constrain(leftThrottle, -512, 512);



  //weaps

  if (RemoteConfig.ch3_bdir) {
    servoAAngle = map(ch3, -508, 508, BoardConfig.servo_a_min, BoardConfig.servo_a_max);
  } else {
    servoAAngle = map(ch3, 0, 508, BoardConfig.servo_a_max, BoardConfig.servo_a_min);
  }

  servoBAngle = map(ch4, -508, 508, 0, 1023);

  motWSpeed = map(ch4, -508, 508, -512, 512);


  //cmds
  cmd_up = false;
  cmd_down = false;
  cmd_enter = false;
  cmd_back = false;

  state_up = RemoteController->dpad() & 0b1;
  state_down = RemoteController->dpad() & 0b10;
  state_enter = RemoteController->a();
  state_back = RemoteController->b();

  if (!state_up && state_up_pre) {
    cmd_up = true;
  }

  if (!state_down && state_down_pre) {
    cmd_down = true;
  }

  if (!state_enter && state_enter_pre) {
    cmd_enter = true;
  }

  if (!state_back && state_back_pre) {
    cmd_back = true;
  }

  state_up_pre = state_up;
  state_down_pre = state_down;
  state_enter_pre = state_enter;
  state_back_pre = state_back;




  /*
  Serial.print(cmd_up);
  Serial.print(cmd_down);
  Serial.print(cmd_enter);
  Serial.println(cmd_back);
  //*/
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
  if (failsafe) {
    cmd_up = false;
    cmd_down = false;
    cmd_enter = false;
    cmd_back = false;
    motRSpeed = 0;
    motLSpeed = 0;
    motWSpeed = 0;
    servoAAngle = -1;
    servoBAngle = -1;
    RobotBoard.failsafe();
    return;
  }

  RobotBoard.motLSetSpeed(motLSpeed * BoardConfig.motl_reverse);
  RobotBoard.motRSetSpeed(motRSpeed * BoardConfig.motr_reverse);
  RobotBoard.motWSetSpeed(motWSpeed * BoardConfig.motw_reverse);

  RobotBoard.servoASetAngle(servoAAngle);
  RobotBoard.servoBSetAngle(servoBAngle);
}

void initEeprom() {
  Serial.println("EEPROM start");
  EEPROM.begin(64);
  byte valid = EEPROM.readByte(ADDR_MAC_VALID);
  if (valid == EEPROM_VALID_BYTE) {
    EEPROM.readBytes(ADDR_MAC, &ControllerAddress, sizeof(ControllerAddress));
    MacEepromValid = true;
    binding = false;
    Serial.println("-> Loaded MAC");
  } else {
    Serial.println("-> no MAC");
  }
  valid = EEPROM.readByte(ADDR_RMTCONFIG_VALID);
  if (valid == EEPROM_VALID_BYTE) {
    EEPROM.readBytes(ADDR_RMTCONFIG, &RemoteConfig, sizeof(RemoteConfig));
    CfgRemoteEepromValid = true;
    Serial.println("-> Loaded RMT");
  } else {
    Serial.println("-> no RMT");
  }
  valid = EEPROM.readByte(ADDR_BOARDCONFIG_VALID);
  if (valid == EEPROM_VALID_BYTE) {
    EEPROM.readBytes(ADDR_BOARDCONFIG, &BoardConfig, sizeof(BoardConfig));
    Serial.println("-> Loaded BOARD");
  } else {
    Serial.println("-> no BOARD");
  }
  Serial.println("EEPROM end");
}


void check_mode() {
  unsigned long current_time = millis();
  if (!connection_ok) {
    if (current_time > 60000 && binding == false) {
      Serial.println("rebinding");
      MacEepromValid = false;
      binding = true;
      EEPROM.writeBytes(ADDR_MAC_VALID, &CLEAR_BYTE, sizeof(CLEAR_BYTE));
      EEPROM.commit();
      return;
    }
    if (current_time > 6000 && setting == false) {
      Serial.println("setting...");
      setting = true;
      return;
    }
  } else {
    binding = false;
  }
}



int setting_mode = 0;

void handle_blink() {
  if (binding) {
    LedTask.setBlinks(2, 500, 10);
    return;
  }
  if (setting) {
    if (setting_mode == 0) {
      LedTask.setBlinks(1, -1, 15);
    } else {
      LedTask.setBlinks(1, -1, 5);
    }
    return;
  }
  if (failsafe) {
    LedTask.setBlinks(1, -1, 1);
    return;
  }
  LedTask.ledOn();
}


void processCmd() {
  if (!setting) {
    return;
  }

  if (cmd_back) {
    EEPROM.writeBytes(ADDR_BOARDCONFIG_VALID, &EEPROM_VALID_BYTE, sizeof(EEPROM_VALID_BYTE));
    EEPROM.writeBytes(ADDR_BOARDCONFIG, &BoardConfig, sizeof(BoardConfig));
    EEPROM.commit();
    setting = false;
  }

  if (cmd_enter) {
    setting_mode = (setting_mode + 1) % 2;
  }

  if (setting_mode == 1) {
    int temp_angle = BoardConfig.servo_a_min;
    if (cmd_up) {
      temp_angle = temp_angle - 10;
      constrain(temp_angle, 0, BoardConfig.servo_a_max - 20);
      BoardConfig.servo_a_min = temp_angle;
    }
    if (cmd_down) {
      temp_angle = temp_angle + 10;
      constrain(temp_angle, 0, 1023);
      BoardConfig.servo_a_min = temp_angle;
    }
  }
  if (setting_mode == 0) {
    int temp_angle = BoardConfig.servo_a_max;
    if (cmd_up) {
      temp_angle = temp_angle - 10;
      constrain(temp_angle, BoardConfig.servo_a_min + 20, 1023);
      BoardConfig.servo_a_max = temp_angle;
    }
    if (cmd_down) {
      temp_angle = temp_angle + 10;
      constrain(temp_angle, 0, 1023);
      BoardConfig.servo_a_max = temp_angle;
    }
  }
  Serial.print(BoardConfig.servo_a_min);
  Serial.print("  ");
  Serial.println(BoardConfig.servo_a_max);
}


// Arduino setup function. Runs in CPU 1
void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  RobotBoard.boardInit();
  LedTask.init();
  initEeprom();


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
}

// Arduino loop function. Runs in CPU 1.
void loop() {
  // This call fetches all the controllers' data.
  // Call this function in your main loop.
  bool dataUpdated = BP32.update();

  if (RemoteController != nullptr && !failsafe) {
    if (dataUpdated) {
      processController();
    }
  }
  processCmd();
  processBoard();

  // The main loop must have some kind of "yield to lower priority task" event.
  // Otherwise, the watchdog will get triggered.
  // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
  // Detailed info here:
  // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

  //     vTaskDelay(1);
  check_mode();
  handle_blink();
  delay(10);
}

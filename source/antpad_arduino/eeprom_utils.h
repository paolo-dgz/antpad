#include "factory_settings.h"
const byte EEPROM_VER = 1;

const byte EEPROM_VALID_BYTE = EEPROM_VER;
const byte CLEAR_BYTE = 0;


const uint8_t ADDR_EEPROM_VER_VALID = 0;
const uint8_t ADDR_EEPROM_VER = ADDR_EEPROM_VER_VALID + sizeof(EEPROM_VALID_BYTE);

const uint8_t ADDR_MAC_VALID = ADDR_EEPROM_VER + sizeof(EEPROM_VER);
const uint8_t ADDR_MAC = ADDR_MAC_VALID + sizeof(EEPROM_VALID_BYTE);

const uint8_t ADDR_RMTCONFIG_VALID = ADDR_MAC + sizeof(ControllerAddress);
const uint8_t ADDR_RMTCONFIG = ADDR_RMTCONFIG_VALID + sizeof(EEPROM_VALID_BYTE);

const uint8_t ADDR_BOARDCONFIG_VALID = ADDR_RMTCONFIG + sizeof(remote_config_t);
const uint8_t ADDR_BOARDCONFIG = ADDR_BOARDCONFIG_VALID + sizeof(EEPROM_VALID_BYTE);

const uint8_t ADDR_EEPROM_END = ADDR_BOARDCONFIG + sizeof(board_config_t);

bool MacEepromValid = false;

void ValidateEeprom() {
  Serial.println("-> Saved VER");
  EEPROM.writeBytes(ADDR_EEPROM_VER_VALID, &EEPROM_VALID_BYTE, sizeof(EEPROM_VALID_BYTE));
  EEPROM.writeBytes(ADDR_EEPROM_VER, &EEPROM_VER, sizeof(EEPROM_VER));
  EEPROM.commit();
}

void ClearEeprom() {
  Serial.println("-> Clearing EEPROM");
  for(int i = ADDR_EEPROM_VER_VALID; i <ADDR_EEPROM_END;i++ ){
    EEPROM.write(i,0);
  }
  EEPROM.commit();
}

bool CheckEepromVer() {
  Serial.print("EEPROM Checking for v");
  Serial.println((int)EEPROM_VER);
  byte valid = EEPROM.readByte(ADDR_EEPROM_VER_VALID);
  if (valid == EEPROM_VALID_BYTE) {
    byte eepromVer = 0;
    EEPROM.readBytes(ADDR_EEPROM_VER, &eepromVer, sizeof(eepromVer));
    if (eepromVer == EEPROM_VER) {
      Serial.print("EEPROM OK: Found v");
      Serial.println((int)eepromVer);
      return true;
    }
    Serial.print("EEPROM MISMATCH: Found v");
    Serial.println((int)eepromVer);
    return false;
  }
  Serial.println("EEPROM NOT VALID");
  return false;
}

void SaveMac(uint8_t* macPtr) {
  Serial.println("-> Saved MAC");
  EEPROM.writeBytes(ADDR_MAC_VALID, &EEPROM_VALID_BYTE, sizeof(EEPROM_VALID_BYTE));
  EEPROM.writeBytes(ADDR_MAC, macPtr, sizeof(ControllerAddress));
  EEPROM.commit();
}

int LoadMac(uint8_t* macPtr) {
  byte valid = EEPROM.readByte(ADDR_MAC_VALID);
  if (valid == EEPROM_VALID_BYTE) {
    EEPROM.readBytes(ADDR_MAC, macPtr, sizeof(ControllerAddress));
    Serial.println("-> Loaded MAC");
    MacEepromValid = true;
    return 1;
  }
  Serial.println("-> failed MAC");
  return -1;
}

void SaveRemoteConfig(remote_config_t* remoteConfigPtr) {
  Serial.println("-> Saved RMT");
  EEPROM.writeBytes(ADDR_RMTCONFIG_VALID, &EEPROM_VALID_BYTE, sizeof(EEPROM_VALID_BYTE));
  EEPROM.writeBytes(ADDR_RMTCONFIG, remoteConfigPtr, sizeof(remote_config_t));
  EEPROM.commit();
}

int LoadRemoteConfig(remote_config_t* remoteConfigPtr) {
  byte valid = EEPROM.readByte(ADDR_RMTCONFIG_VALID);
  if (valid == EEPROM_VALID_BYTE) {
    EEPROM.readBytes(ADDR_RMTCONFIG, remoteConfigPtr, sizeof(remote_config_t));
    Serial.println("-> Loaded RMT");
    return 1;
  }
  Serial.println("-> Failed RMT");
  return -1;
}

void SaveBoardConfig(board_config_t* boardConfigPtr) {
  Serial.println("-> Saved BOARD");
  EEPROM.writeBytes(ADDR_BOARDCONFIG_VALID, &EEPROM_VALID_BYTE, sizeof(EEPROM_VALID_BYTE));
  EEPROM.writeBytes(ADDR_BOARDCONFIG, boardConfigPtr, sizeof(board_config_t));
  EEPROM.commit();
}

int LoadBoardConfig(board_config_t* boardConfigPtr) {
  byte valid = EEPROM.readByte(ADDR_BOARDCONFIG_VALID);
  if (valid == EEPROM_VALID_BYTE) {
    EEPROM.readBytes(ADDR_BOARDCONFIG, boardConfigPtr, sizeof(board_config_t));
    Serial.println("-> Loaded BOARD");
    return 1;
  }
  Serial.println("-> Failed BOARD");
  return -1;
}


void InitEeprom() {
  Serial.println("EEPROM start");
  EEPROM.begin(64);
  if (CheckEepromVer() && !factory_reset) {
    LoadMac(ControllerAddress);
    LoadRemoteConfig(&RemoteConfig);
    LoadBoardConfig(&BoardConfig);
  } else {
    Serial.println("EEPROM overwriting");
    SaveRemoteConfig(&RemoteConfig);
    SaveBoardConfig(&BoardConfig);
    ValidateEeprom();
  }
  Serial.println("EEPROM end");
}

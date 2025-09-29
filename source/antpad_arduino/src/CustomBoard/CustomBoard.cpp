#include "CustomBoard.h"

//public
void CustomBoard::boardInit(bool motw_servob) {
  //setup your pins here, gets called onm setup
  return;
}


void CustomBoard::motRSetSpeed(int speed) {
  //move your right motor, positive speed forward
  return;
}

void CustomBoard::motLSetSpeed(int speed) {
  //move your left motor, positive speed forward
  return;
}

void CustomBoard::motWSetSpeed(int speed) {
  //move your weapon motor
  return;
}

void CustomBoard::servoASetAngle(int angle) {
  //move your ch3 linked servo
  return;
}

void CustomBoard::servoBSetAngle(int angle) {
  //move your ch4 linked servo
  return;
}

void CustomBoard::failsafe(){
  //failsafe your board when no connection
  return;
}

void CustomBoard::setLed(bool state){
  //handle the debug led function used by firmware
  //statre = true -> led on
  return;
}

//private

//your private functions here
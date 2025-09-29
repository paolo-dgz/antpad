#include "ledUtility.h"
#include "esp_log.h"
bool LedUtility::_led_state;
int16_t LedUtility::_led_i;
int16_t LedUtility::_blinks;
int16_t LedUtility::_blink_delay = 300;
float LedUtility::_blink_frequency = 3.0;
Board* LedUtility::_current_board;

LedUtility::LedUtility(Board* board){
    _current_board = board;
    _led_state = false;
    _led_i = 0;
    _blinks = 2;
}

void LedUtility::setBlinks(uint16_t amount, int16_t pause_ms, float frequency ){
  if(amount == 0 || frequency <= 0.0){
    ledOff();
  }
    _blinks = amount;
    _blink_delay = pause_ms;
    _blink_frequency = frequency;
}

void LedUtility::setBlinks(uint16_t amount, int16_t pause_ms ){
    setBlinks(amount, pause_ms, 3.0);
}

void LedUtility::setBlinks(uint16_t amount){
    setBlinks(amount, 300, 3.0);
}

void LedUtility::init(){
    ledOn();
    
      xTaskCreatePinnedToCore(
        this->ledUtilityTask,    /* Function to implement the task */
      "ledUtilityTask", /* Name of the task */
      4096,         /* Stack size in words */
      NULL,          /* Task input parameter */
      0,             /* Priority of the task */
      NULL,          /* Task handle. */
      0);     /* Core where the task should run */
    ESP_LOGI("LED UTILITY", "core task OK");

}

void LedUtility::ledOn(){
  _blinks = 0;
  _current_board->setLed(true);
  _led_state = true;
}

void LedUtility::ledOff(){
  _blinks = 0;
  _current_board->setLed(false);
  _led_state = false;
}

void LedUtility::ledUtilityTask(void *pvParameters){
  while (1){
    if(_blinks != 0){
        while (_led_i < _blinks)
        {
            //ESP_LOGI("LED", "LED state %d",_led_state);
            if(_led_state){
                _current_board->setLed(false);
                _led_state = false;
                _led_i++;
            }else{
                _current_board->setLed(true);
                _led_state = true;
            }
            vTaskDelay(1000.0f/(2.0f*_blink_frequency) / portTICK_PERIOD_MS);
        }
        if (_blink_delay >= 0){
          vTaskDelay(_blink_delay / portTICK_PERIOD_MS);
        }else{
          vTaskDelay(1000.0f/(2.0f*_blink_frequency) / portTICK_PERIOD_MS);
        }
        _led_i = 0;
    }else{
        _led_i = 0;
        vTaskDelay(150 / portTICK_PERIOD_MS);
    }

  }
}

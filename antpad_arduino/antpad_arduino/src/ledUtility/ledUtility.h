#ifndef LED_UTILITY_H
#define LED_UTILITY_H
#include <Arduino.h>
#include "../Board.h"

/**
     * @brief create a led object to handle on, off and blinks
     * in a separate core task
     * @attention 1. requires init() on setup to work
     * @attention 2. led pin will be used as output
     * 
     * @param board  current board object to use setled function
     */
class LedUtility
{
public:

    LedUtility(Board* board);
    /**
     * @brief    starts the led utility coroutine
     *
     */
    void init();
    /**
     * @brief set led blinking behaviour 
     * 
     * @param amount  blinks streak lenght before pausing off
     * @param pause_ms ms of pausing off bewteen blink streaks for best streak
     * legibility put this higher than 1000/amount
     * @param blink_frequency blinks per second while doing a blink streak 
     */
    static void setBlinks(uint16_t amount, int16_t pause_ms, float frequency);
    /**
     * @brief set led blinking behaviour 
     * 
     * @param amount  blinks streak lenght before pausing off
     * @param pause_ms ms of pausing off bewteen blink streaks 
     * with 3 blinks per second, lower than 166ms pause will cause
     * streaks to be contiguous
     */
    static void setBlinks(uint16_t amount, int16_t pause_ms);
    /**
     * @brief set led blinking behaviour 
     * 
     * @param amount  blinks streak lenght before pausing off for 300ms 
     * with 3 blinks per second
     */
    static void setBlinks(uint16_t amount);
    /**
     * @brief set led permanently ON ignoring blinks
     */
    static void ledOn();
    /**
     * @brief set led permanently OFF ignoring blinks
     */
    static void ledOff();
    static void ledUtilityTask(void *pvParameters);

private:
    static bool _led_state;
    static int16_t _led_i;
    static int16_t _blinks;
    static int16_t _blink_delay;
    static float _blink_frequency;
    static Board* _current_board;
};

#endif

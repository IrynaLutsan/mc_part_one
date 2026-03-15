#pragma once 
#include <stdint.h>

#define LED_COUNT 3

class CLedController
{
public:
    enum class animation
    {
        L1_L2_L3,
        L3_L2_L1
    };
    CLedController(uint8_t pin_1, uint8_t pin_2, uint8_t pin_3);
    void Init();
    void StartAnimation();
    void HandleLeds();

private:
    void HandleAnimation_L1_L2_L3(bool start);
    void HandleAnimation_L3_L2_L1(bool start);

    uint8_t m_led_pins[LED_COUNT];
    bool m_busy{false};
    animation m_cur_animation{CLedController::animation::L1_L2_L3};
    uint8_t m_cur_led_idx{0};
    unsigned long m_led_on_time_ms{0};
};

#pragma once 
#include <stdint.h>
#include <map>
#include <functional> 

#define LED_COUNT 3

class CLedController
{
public:
    using LedNotify_t = std::function<void(uint8_t, bool)>;
    enum class animation
    {
        L1_L2_L3,
        L3_L2_L1
    };
    CLedController();
    void Init();
    void StartAnimation();
    void HandleLeds();
    void SetPinMapping(uint8_t index, uint8_t gpio_pin);
    void SetNotifyCallback(LedNotify_t cb) { m_notify_cb = cb; }

private:
    void HandleAnimation_L1_L2_L3(bool start);
    void HandleAnimation_L3_L2_L1(bool start);
    void SetLedState(uint8_t led_idx, bool led_state);

    std::map<uint8_t, uint8_t> m_led_pins;
    bool m_busy{false};
    animation m_cur_animation{CLedController::animation::L1_L2_L3};
    uint8_t m_cur_led_idx{0};
    unsigned long m_led_on_time_ms{0};
    LedNotify_t m_notify_cb;
};

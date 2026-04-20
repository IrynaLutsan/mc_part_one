#pragma once
#include <functional> 

class CButton 
{
public:
    using ButtonNotify_t = std::function<void(void)>;
    CButton(int gpio_pin);
    void Init();
    void Run();
    void SetNotifyCallback(ButtonNotify_t cb) { m_callback = cb; }
    
private:
    int m_gpio_pin;
    bool m_prev_state;
    unsigned long m_press_time;
    ButtonNotify_t m_callback;
};
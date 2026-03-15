#pragma once

using ButtonCallback_t = void(*)();

class CButton 
{
public:
    CButton(int gpio_pin, ButtonCallback_t cb = nullptr);
    void Init();
    void Run();
    
private:
    int m_gpio_pin;
    bool m_prev_state;
    unsigned long m_press_time;
    ButtonCallback_t m_callback;
};
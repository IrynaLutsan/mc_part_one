#include "Arduino.h"
#include "CLedController.h"

#define LED_ON_DURATION_MS 500

CLedController:: CLedController()
{
}

void CLedController::SetPinMapping(uint8_t index, uint8_t gpio_pin) {
    m_led_pins[index] = gpio_pin;
}

void CLedController::Init() {
    for (auto const& [index, pin] : m_led_pins) {
        pinMode(pin, OUTPUT);
    }
}

void CLedController:: StartAnimation()
{
    if (!m_busy){
        switch (m_cur_animation){
            case animation:: L1_L2_L3:
                m_cur_animation = animation:: L3_L2_L1;
                 HandleAnimation_L3_L2_L1(true);
                break;
            case animation:: L3_L2_L1:
                m_cur_animation = animation:: L1_L2_L3;
                 HandleAnimation_L1_L2_L3(true);
                break;
        }
        m_busy = true;
    }
}

void CLedController:: HandleLeds()
{
    if (m_busy){
        switch (m_cur_animation){
            case animation:: L1_L2_L3:
                HandleAnimation_L1_L2_L3(false);
                break;
            case animation:: L3_L2_L1:
                HandleAnimation_L3_L2_L1(false);
                break;
        }
    }
}

void CLedController:: HandleAnimation_L1_L2_L3(bool start)
{
    if(start){
        m_cur_led_idx = 0;
        SetLedState(m_cur_led_idx, HIGH);
        m_led_on_time_ms = millis();
    }else{
        if (millis() - m_led_on_time_ms >= LED_ON_DURATION_MS) {
            SetLedState(m_cur_led_idx, LOW);
            if(m_cur_led_idx == LED_COUNT - 1){
                m_busy = false;
                return;
            }else{
                m_cur_led_idx++;
                SetLedState(m_cur_led_idx, HIGH);
                m_led_on_time_ms = millis();
            }
        }
    }
}
    
void CLedController:: HandleAnimation_L3_L2_L1(bool start)
{
    if(start){
        m_cur_led_idx = LED_COUNT - 1;
        SetLedState(m_cur_led_idx, HIGH);
        m_led_on_time_ms = millis();
    }else{
        if (millis() - m_led_on_time_ms >= LED_ON_DURATION_MS) {
            SetLedState(m_cur_led_idx, LOW);
            if(m_cur_led_idx == 0){
                m_busy = false;
                return;
            }else{
                m_cur_led_idx--;
                SetLedState(m_cur_led_idx, HIGH);
                m_led_on_time_ms = millis();
            }
        }
    }
}

void CLedController:: SetLedState(uint8_t led_idx, bool led_state){
    if (m_led_pins.count(led_idx)) {
        digitalWrite(m_led_pins[led_idx], led_state);
        if (m_notify_cb) {
            m_notify_cb(led_idx, led_state); 
        }
    }
}
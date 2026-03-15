#include "Arduino.h"
#include "CLedController.h"

#define LED_ON_DURATION_MS 500

CLedController:: CLedController(uint8_t pin_1, uint8_t pin_2, uint8_t pin_3):
m_led_pins ({pin_1, pin_2, pin_3})
{
   
}

void CLedController:: Init()
{
    for(int i = 0; i < LED_COUNT; i++){
        pinMode(m_led_pins[i], OUTPUT);
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
        digitalWrite(m_led_pins[m_cur_led_idx], HIGH);
        m_led_on_time_ms = millis();
    }else{
        if (millis() - m_led_on_time_ms >= LED_ON_DURATION_MS) {
            digitalWrite(m_led_pins[m_cur_led_idx], LOW);
            if(m_cur_led_idx == LED_COUNT - 1){
                m_busy = false;
                return;
            }else{
                m_cur_led_idx++;
                digitalWrite(m_led_pins[m_cur_led_idx], HIGH);
                m_led_on_time_ms = millis();
            }
        }
    }
}
    
void CLedController:: HandleAnimation_L3_L2_L1(bool start)
{
    if(start){
        m_cur_led_idx = LED_COUNT - 1;
        digitalWrite(m_led_pins[ m_cur_led_idx], HIGH);
        m_led_on_time_ms = millis();
    }else{
        if (millis() - m_led_on_time_ms >= LED_ON_DURATION_MS) {
            digitalWrite(m_led_pins[m_cur_led_idx], LOW);
            if(m_cur_led_idx == 0){
                m_busy = false;
                return;
            }else{
                m_cur_led_idx--;
                digitalWrite(m_led_pins[m_cur_led_idx], HIGH);
                m_led_on_time_ms = millis();
            }
        }
    }
}
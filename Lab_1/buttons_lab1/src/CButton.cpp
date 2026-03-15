#include "CButton.h"
#include <Arduino.h>

#define DEBOUNCE_DURATION_MS 50

CButton:: CButton(int gpio_pin, ButtonCallback_t cb) 
: m_gpio_pin(gpio_pin), 
  m_prev_state(HIGH), 
  m_press_time(0), 
  m_callback(cb)
{
   
}

void CButton:: Init()
{
    pinMode(m_gpio_pin, INPUT_PULLUP);
}

void CButton:: Run()
{
    bool cur_button_state = digitalRead(m_gpio_pin);
    if (!m_press_time && m_prev_state == HIGH && cur_button_state == LOW){
        m_press_time = millis();
        Serial.println("Physical Button: pressed");
    }
    if(m_press_time && ((millis() - m_press_time) >= DEBOUNCE_DURATION_MS)){
        m_press_time = 0;
        if (m_callback != nullptr) {
            m_callback();
        }
        
    }
    m_prev_state = cur_button_state;
}

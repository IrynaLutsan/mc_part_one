#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include "web_interface.h"

#define LED_1 25
#define LED_2 26
#define LED_3 27
#define BUTTON 12
#define DEBOUNCE_DURATION_MS 50
#define LED_ON_DURATION_MS 500

#define L2P(i) (state ? (i) : 2 - (i))

void handle_leds(void);
void start_led_animation(void);

bool leds_busy = false;
unsigned char state = 0;
unsigned char leds_array[] = {LED_1, LED_2, LED_3};

bool prev_button_state = HIGH;

unsigned long button_press_time;
unsigned char cur_led_idx;
unsigned long led_on_time_ms;


void setup() {
  Serial.begin(115200);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  initWiFi();
}

void loop() {
  updateWebInterface();
  bool cur_button_state = digitalRead(BUTTON);
  if (!button_press_time && prev_button_state == HIGH && cur_button_state == LOW){
    button_press_time = millis();
    Serial.println("Physical Button: pressed");
  }
  if(button_press_time && ((millis() - button_press_time) >= DEBOUNCE_DURATION_MS)){
    button_press_time = 0;
    start_led_animation();
  }

  prev_button_state = cur_button_state;

  handle_leds();
}

void start_led_animation(void){
  if(!leds_busy){
    leds_busy = true;
    state = !state;
    Serial.println(state ? "Direction: L1 -> L3" : "Direction: L3 -> L1");
    cur_led_idx = 0;
    digitalWrite(leds_array[L2P(cur_led_idx)], HIGH);
    led_on_time_ms = millis();
  }
}

void handle_leds(void){
  if (leds_busy){
    if (millis() - led_on_time_ms >= LED_ON_DURATION_MS){
      digitalWrite(leds_array[L2P(cur_led_idx)], LOW);
      if(cur_led_idx == 2){
        leds_busy = false;
        return;
      }else{
        cur_led_idx++;
        digitalWrite(leds_array[L2P(cur_led_idx)], HIGH);
        led_on_time_ms = millis();
      }
    }
  }
}


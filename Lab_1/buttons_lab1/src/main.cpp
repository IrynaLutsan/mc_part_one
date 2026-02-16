#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include "web_interface.h"

#define LED_1 25
#define LED_2 26
#define LED_3 27
#define BUTTON 12

void blink_leds(void);

bool leds_busy = false;
unsigned char state = 0;
unsigned char leds_array[] = {LED_1, LED_2, LED_3};

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

  if (digitalRead(BUTTON) == LOW && !leds_busy) {
    delay(50); 
    if (digitalRead(BUTTON) == LOW) {
      Serial.println("Physical Button: pressed");
      blink_leds(); 
    }
  }
}

void blink_leds(void){
  leds_busy = true;
  state = !state;
  Serial.println(state ? "Direction: L1 -> L3" : "Direction: L3 -> L1");

  for (int i = 0; i < 3; i++){
    int target_led = 0;
    if(state){    // блимкання L1->L2->L3
      target_led = i;
    }else{        // блимкання L3->L2->L1
      target_led = 2 - i;
    }
    digitalWrite(leds_array[target_led], 1);
    for(int j = 0; j < 50; j++) {
      delay(10); 
      updateWebInterface();
    }
    digitalWrite(leds_array[target_led], 0);
  }
  leds_busy = false;
  Serial.flush();
}
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include "web_interface.h"
#include "CButton.h"
#include "CLedController.h"

#define LED_1 25
#define LED_2 26
#define LED_3 27
#define BUTTON 12

void start_led_animation(void);

CButton button(BUTTON, start_led_animation);
CLedController led_controller(LED_1, LED_2, LED_3);

void setup() {
  Serial.begin(115200);
  button.Init();  
  led_controller.Init();
  initWiFi();
}

void loop() {
  updateWebInterface();
  button.Run();
  led_controller.HandleLeds();
}

void start_led_animation(void){
  led_controller.StartAnimation();
}

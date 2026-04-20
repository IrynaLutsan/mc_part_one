#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include "web_interface.h"
#include <WebSocketsServer.h> 
#include "CButton.h"
#include "CLedController.h"

#define LED_1 25
#define LED_2 26
#define LED_3 27
#define BUTTON 12
#define CHECK_PIN 4 // if its esp1 - pin will be at 0(LOW), if its esp2 - 1(HIGH)

extern WebSocketsServer webSocket;
void start_led_animation(void);

CButton button(BUTTON);
CLedController led_controller;


void setup() {
  Serial.begin(115200);
  pinMode(CHECK_PIN, INPUT);

  bool isEsp1 = digitalRead(CHECK_PIN);
  Serial.printf("I am %s\n", isEsp1 ? "Esp1" : "Esp2");

  button.SetNotifyCallback(start_led_animation);

  button.Init();

  if (isEsp1){
    led_controller.SetPinMapping(0, LED_1);
    led_controller.SetPinMapping(1, LED_2);
    led_controller.SetPinMapping(2, LED_3);
  }else{
    led_controller.SetPinMapping(0, LED_3);
    led_controller.SetPinMapping(1, LED_2);
    led_controller.SetPinMapping(2, LED_1);
  }

  led_controller.Init();

  int rx_pin = isEsp1 ? 16 : 17;  
  int tx_pin = isEsp1 ? 17 : 16;

  Serial2.begin(9600, SERIAL_5O1, rx_pin, tx_pin);
  
  initWiFi(isEsp1);

  led_controller.SetNotifyCallback([](uint8_t idx, bool state) {
    String message = String(idx) + (state ? "_ON" : "_OFF");
    
    webSocket.broadcastTXT(message); 
  });
}

void loop() {
  updateWebInterface();
  button.Run();
  led_controller.HandleLeds();

  #if 0
  if (Serial2.available()) {
    uint8_t incomingByte = Serial2.read(); 
    if (incomingByte == 4) { // Check if it's 'I' (0x49)
        Serial.println("UART: Command 'I' receved! Starting blink algorithm.");
        start_led_animation();
    }
  }
  #else
  if (Serial2.available()) {
    String rx_data = Serial2.readString(); 
    //rx_data.trim(); 

    if (rx_data.length() == 2 && rx_data[0] == 4 && rx_data[1] == 9) {
      Serial.println("UART: Command 'I' receved! Starting blink algorithm.");
      start_led_animation(); 
    }
  }
  #endif
}

void start_led_animation(void){
  led_controller.StartAnimation();
}

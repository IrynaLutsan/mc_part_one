#include "web_interface.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsServer.h> 
#include <WebServer.h>

extern void start_led_animation(void);

WebServer server(80);
WebSocketsServer webSocket(81); //= WebSocketsServer(81);

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String cmd = String((char*)payload);
    
    if (cmd == "LOCAL_START") {
      Serial.println("Web: Запуск локальної анімації"); 
      start_led_animation(); 
    } 
    else if (cmd == "REMOTE_START") {
      Serial.println("Web: Send command to partner on UART");
      Serial2.write(4); 
      Serial2.write(9); 
      //Serial2.write('\n'); 
    }
  }
}

void initWiFi(bool isAP) {
    if (isAP) {
        Serial.printf("Starting AP: %s ...\n", AP_SSID);
        WiFi.softAP(AP_SSID, AP_PASS);
    }else{
        WiFi.begin(AP_SSID, AP_PASS);
        if (WiFi.waitForConnectResult() == WL_CONNECTED){
            Serial.printf("Connected! IP Adress: %s\n", WiFi.localIP().toString());
        }else{
            Serial.printf("Failed to connect to AP %s\n", AP_SSID);
        }
    }
    //Serial.flush();
    server.on("/", handleRoot);
    server.begin();
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          body { font-family: sans-serif; background: #1a1a2e; color: white; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; margin: 0; }
          .card { background: rgba(255, 255, 255, 0.05); padding: 40px; border-radius: 24px; text-align: center; backdrop-filter: blur(10px); border: 1px solid rgba(255,255,255,0.1); box-shadow: 0 10px 30px rgba(0,0,0,0.5); }
          
          /* Контейнер для світлодіодів */
          .led-container { display: flex; justify-content: center; gap: 20px; margin: 30px 0; }
          .led-box { width: 60px; height: 60px; border: 3px solid #444; border-radius: 12px; transition: 0.2s; background: transparent; }
          
          /* Кнопки */
          .btn-group { display: flex; gap: 15px; justify-content: center; }
          .btn { background: linear-gradient(135deg, #00d2ff, #3a7bd5); border: none; color: white; padding: 15px 25px; font-size: 1rem; border-radius: 12px; cursor: pointer; transition: 0.3s; font-weight: bold; }
          .btn:active { transform: scale(0.95); opacity: 0.9; }
        </style>
      </head>
      <body>
        <div class="card">
          <h1>ESP Pushbutton Web Server</h1>
          
          <div class="led-container">
            <div id="led0" class="led-box" style="border-color: #ff4d4d;"></div>
            <div id="led1" class="led-box" style="border-color: #ffa500;"></div>
            <div id="led2" class="led-box" style="border-color: #ffff00;"></div>
          </div>

          <div class="btn-group">
            <button class="btn" onclick="sendCmd('LOCAL_START')">Algorithm 1</button>
            <button class="btn" onclick="sendCmd('REMOTE_START')">Algorithm 2</button>
          </div>
        </div>

        <script>
          var gateway = `ws://${window.location.hostname}:81/`;
          var websocket = new WebSocket(gateway);

          function sendCmd(command) {
            console.log("Sending: " + command);
            websocket.send(command);
          }

          websocket.onmessage = function(event) {
            let msg = event.data;
            switch(msg) {
              case '0_ON':  document.getElementById('led0').style.backgroundColor = '#ff4d4d'; break;
              case '0_OFF': document.getElementById('led0').style.backgroundColor = 'transparent'; break;
              case '1_ON':  document.getElementById('led1').style.backgroundColor = '#ffa500'; break;
              case '1_OFF': document.getElementById('led1').style.backgroundColor = 'transparent'; break;
              case '2_ON':  document.getElementById('led2').style.backgroundColor = '#ffff00'; break;
              case '2_OFF': document.getElementById('led2').style.backgroundColor = 'transparent'; break;
            }
          };
        </script>
      </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void updateWebInterface() {
    server.handleClient();
    webSocket.loop();
}
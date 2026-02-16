#include "web_interface.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsServer.h> 
#include <WebServer.h>

extern bool leds_busy; 
extern void blink_leds(void);

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    if (String((char*)payload) == "START") {
      if (!leds_busy) {
        Serial.println("Web Button: clicked"); 
        blink_leds(); 
      }
    }
  }
}

void initWiFi() {
    WiFi.softAP(AP_SSID, AP_PASS);
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
        <meta charset="UTF-8"> <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          body { font-family: sans-serif; background: #1a1a2e; color: white; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; margin: 0; }
          .card { background: rgba(255, 255, 255, 0.05); padding: 30px; border-radius: 24px; text-align: center; backdrop-filter: blur(10px); border: 1px solid rgba(255,255,255,0.1); }
          .btn { background: linear-gradient(135deg, #00d2ff, #3a7bd5); border: none; color: white; padding: 15px 40px; font-size: 1.2rem; border-radius: 50px; cursor: pointer; transition: 0.3s; }
          .btn:disabled { background: #444; color: #888; cursor: not-allowed; }
        </style>
      </head>
      <body>
        <div class="card">
          <h1>ESP32 Leds Control</h1>
          <button id="mainBtn" class="btn" onclick="sendStart()">Запустити вогні</button>
        </div>
        <script>
          var gateway = `ws://${window.location.hostname}:81/`;
          var websocket = new WebSocket(gateway);
          function sendStart() {
            websocket.send('START');
            var btn = document.getElementById('mainBtn');
            btn.disabled = true;
            btn.innerText = 'Виконується...';
            setTimeout(() => { 
              btn.disabled = false; 
              btn.innerText = 'Запустити вогні'; 
            }, 1700);
          }
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
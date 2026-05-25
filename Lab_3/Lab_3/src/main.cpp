#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Wire.h>

const char* ssid = "POCO X3 PRO";      
const char* password = "qwertyuiop";       

WebServer server(80);

#define RAIN_SENSOR_PIN 34

unsigned long lastLogTime = 0;
const unsigned long logInterval = 10000;

unsigned long lastWiFiCheck = 0;
const unsigned long wiFiCheckInterval = 500;

bool serverStarted = false;

void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "Error: File index.html not found in LittleFS");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void handleApiData() {
  int rawValue = analogRead(RAIN_SENSOR_PIN);
  
  String json = "{\"value\":" + String(rawValue) + "}";
  server.send(200, "application/json", json);
}

void handleDownloadLog() {
  File file = LittleFS.open("/log.csv", "r");
  if (!file) {
    server.send(404, "text/plain", "The log.csv file has not yet been created or is missing.");
    return;
  }
  server.streamFile(file, "text/csv");
  file.close();
}

void logToCSV(int sensorValue) {
  File logFile = LittleFS.open("/log.csv", "a");
  if (!logFile) {
    Serial.println("[-] Error when trying to open log.csv file for writing");
    return;
  }
  
  int percentage = map(sensorValue, 4095, 0, 0, 100);
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;

  logFile.print(millis() / 1000);
  logFile.print(",");
  logFile.print(sensorValue);
  logFile.print(",");
  logFile.println(percentage);
  
  logFile.close();
  Serial.printf("[+] Log entry: %lu sec from start, indicator: %d (%d%%)\n", millis() / 1000, sensorValue, percentage);
}

void handleWiFiAndServer() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastWiFiCheck >= wiFiCheckInterval) {
    lastWiFiCheck = currentMillis;
    
    // Wi-Fi 1, Web 0
    if (WiFi.status() == WL_CONNECTED && !serverStarted) {
      Serial.println("\n[+] Wi-Fi connected successfully!");
      Serial.print("[+] The local IP address of your ESP32: ");
      Serial.println(WiFi.localIP());
      
      server.on("/", HTTP_GET, handleRoot);
      server.on("/api/data", HTTP_GET, handleApiData);
      server.on("/log.csv", HTTP_GET, handleDownloadLog);
      
      server.begin();
      serverStarted = true;
      Serial.println("[+] The web server has been successfully started.");
    }
    
    if (WiFi.status() != WL_CONNECTED && serverStarted) {
      Serial.println("\n[-] Wi-Fi connection lost! Web server temporarily stopped.");
      serverStarted = false;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  Serial.println("\nLaunch of IoT Rain Station");

  Wire.begin(21, 22); 
  Serial.println("[+] I2C bus successfully initialized on pins SDA=21, SCL=22.");

  pinMode(RAIN_SENSOR_PIN, INPUT);

  if (!LittleFS.begin(true)) {
    Serial.println("[-] Error while mounting LittleFS system!");
    return;
  }
  Serial.println("[+] LittleFS mounted successfully.");

  if (!LittleFS.exists("/log.csv")) {
    File logFile = LittleFS.open("/log.csv", "w");
    if (logFile) {
      logFile.println("Timestamp(s),RawAnalogValue,RainIntensity(%)"); 
      logFile.close();
      Serial.println("[+] A new log file /log.csv with headers was created.");
    }
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("[+] Wi-Fi connection request sent to background.");
}

void loop() {
  handleWiFiAndServer();
  
  if (serverStarted) {
    server.handleClient();
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastLogTime >= logInterval) {
    lastLogTime = currentMillis;
    
    int sensorValue = analogRead(RAIN_SENSOR_PIN);
    logToCSV(sensorValue);
  }
}
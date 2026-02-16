#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#define AP_SSID "esp_wifi"
#define AP_PASS "123456789"
//http://192.168.4.1/

void initWiFi();
void updateWebInterface();
void handleRoot();

#endif

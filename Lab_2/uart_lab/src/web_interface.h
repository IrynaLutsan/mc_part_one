#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#define AP_SSID "esp_wifi"      
//#define AP_SSID (isEsp1 ? "esp1_wifi" : "esp2_wifi")
#define AP_PASS "123456789"
//#define AP_PASS (isEsp1 ? "123456789" : "987654321")
//http://192.168.4.1/

void initWiFi(bool isAP = true);
void updateWebInterface();
void handleRoot();

#endif

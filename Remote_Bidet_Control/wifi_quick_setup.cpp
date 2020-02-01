#include "wifi_quick_setup.h"

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD" 

void setup_wifi(void){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

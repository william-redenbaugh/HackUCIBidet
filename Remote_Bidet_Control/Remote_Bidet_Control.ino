#include <M5Stack.h>
#include "m5_display_helper.h"
#include "wifi_quick_setup.h"
#include "mqtt_task.h"

// Display helper for dealing with animations. 
M5DisplayHelper display_helper; 
uint16_t color_cyan;
uint16_t color_purple;
uint16_t color_yellow;

// Button Stuff for dealing with stimuli. 
M5QuickReadButton quick_read; 

// ADAFRUIT IO VARIABLES BEGIN // 
// Communication information
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "wredenba"
#define AIO_KEY         "aio_VOvt36DtLob6R6509gy6dP4VYMQV"

// Objects to deal with actual connection and mqtt stuff. 
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);       
Adafruit_MQTT_Subscribe Bidet_Control = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/BidetControlSub");
Adafruit_MQTT_Publish BidetControlPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/BidetControlSub");
// Task that will help with background reading of mqtt stuff. 
MQTTReadTask mqtt_read_task; 
// ADAFRUIT IO VARIABLES END //

void setup() {
  Serial.println(115200);
  // Kinda lazy just sticking a bunch of my projects together
  // so I threw it all into another function. 
  m5_setup();
  mqtt.subscribe(&Bidet_Control);
  mqtt_read_task.begin(&mqtt, &Bidet_Control);

  color_cyan = concat_color(0, 63, 31);
  color_purple = concat_color(31, 0, 31);
  color_yellow = concat_color(31, 63, 0);
}

void loop() {
  uint8_t options = display_helper.show_options(&quick_read, " Bidet ", color_cyan, " AutoOff", color_purple, "  AutoOn  ", color_yellow);
  
  if(options == 1){
    BidetControlPublish.publish(";bidet_cycle");
  }
  else if(options == 2){
    BidetControlPublish.publish(";bidet_auto_on");
  }
  else if(options == 3){
    BidetControlPublish.publish(";bidet_auto_off");
  }
  mqtt_read_task.task();
  
}

/*
Author: William Redenbaugh
Last Edit Date: 2/1/2020
Notes:
Bidet main project file 
*/

#include "wifi_setup.h"
#include "mqtt_task.h"
#include "Bidet.h"

// ADAFRUIT IO VARIABLES BEGIN // 
// Communication information
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "username"
#define AIO_KEY         "key"

// Objects to deal with actual connection and mqtt stuff. 
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);       
Adafruit_MQTT_Subscribe Bidet_Control = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/BidetControlSub");
Adafruit_MQTT_Publish BidetControlPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/BidetControlPub");
Adafruit_MQTT_Publish MatrixControl = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/MatrixControlSub");
Adafruit_MQTT_Publish HeaatControl = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HeaatControlSub");

// Task that will help with background reading of mqtt stuff. 
MQTTReadTask mqtt_read_task; 
// ADAFRUIT IO VARIABLES END //

// BIDET VARIABLES BEGIN / 
#define BIDET_VALVE_GPIO 12

// Bidet manipulation object. 
Bidet bidet; 
bool toilet_reset = true; 
volatile int turbidity = 2500; 
bool auto_bidet = true; 
// BIDET VARIABLES END // 

void setup() {
  Serial.begin(115200);
  bidet.begin(BIDET_VALVE_GPIO);
  setup_wifi();
  
  // Setting up connected stuff. 
  // MQTT Stuff. 
  mqtt.subscribe(&Bidet_Control);
  mqtt_read_task.begin(&mqtt, &Bidet_Control);
  
  xTaskCreate(cloud_task, "Cloud Task", 20000, NULL, 40, NULL);  
  xTaskCreate(bidet_task, "Bidet Task", 20000, NULL, 40, NULL);
}

void loop() {
  vTaskDelete(NULL);
}

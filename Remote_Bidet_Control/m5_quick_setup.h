#ifndef _M5_QUICK_SETUP_H
#define _M5_QUICK_SETUP_H

#include "Arduino.h"
#include <M5Stack.h>
#include "RoboticsFramework.h"
#include "WiFi.h"

class M5QuickReadButton{
    public: 
      // Set's up reading gpio stuff
      void begin(void);

      // Saves the gpio 
      void read(void);

      // Waits for a button to be pressed. 
      uint8_t wait_button(void);
      uint8_t button_uuid = 0; 
      
      // Wait for the individual buttons
      void wait_a(void);
      void wait_b(void);
      void wait_c(void);

      bool a_last_pressed(void);
      bool b_last_pressed(void);
      bool c_last_pressed(void);
      
      DiscreteInput ButtonA;
      DiscreteInput ButtonB;
      DiscreteInput ButtonC;  

      EventGroupHandle_t wait_button_isr;
};

#endif

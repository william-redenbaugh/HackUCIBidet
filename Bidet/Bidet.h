#ifndef _BIDET_H
#define _BIDET_H

#include "Arduino.h"
#include "RoboticsFramework.h"
#include <driver/adc.h>

/*
Author: William Redenbaugh
Last Edit Date: 2/1/2020
Notes:
Small Bidet Class to deal with a couple of things. 
*/

// GPIO used for the turbidity sensor.
#define BIDET_TURBIDITY_GPIO ADC1_CHANNEL_5

class Bidet{
  public: 
    void begin(uint64_t pin);
    void open_valve(void);
    void close_valve(void);
    void cycle_valve(uint16_t del_ms);
    uint32_t read_turbidity(void);
    uint8_t bidet_status(void);
    
  private: 
    // Valve status. 
    DiscreteOutput bidet_valve; 
    uint16_t latest_turbidity = 0;
};

#endif 

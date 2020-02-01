/*
Author: William Redenbaugh
Last Edit Date: 2/1/2020
Notes:
Bidet class implementation.
*/

#include "Bidet.h"

#define READ_FILTER 30

void Bidet::begin(uint64_t pin){
  this->bidet_valve.setup(pin);
  this->close_valve();

  // Setting up the confit. 
  pinMode(33, INPUT);
  
}

void Bidet::open_valve(void){
  this->bidet_valve.set();
}

void Bidet::close_valve(void){
  this->bidet_valve.clear();
}

void Bidet::cycle_valve(uint16_t del_ms){
  this->bidet_valve.set();
  delay(del_ms);
  this->bidet_valve.clear();
}

uint8_t Bidet::bidet_status(void){
  return this->bidet_valve.state();
}

uint32_t Bidet::read_turbidity(void){
  return analogRead(33);
}

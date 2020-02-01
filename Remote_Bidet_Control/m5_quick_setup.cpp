#include "m5_quick_setup.h"

void button_a_interrupt(void *parameters){
  M5QuickReadButton *button_reference = (M5QuickReadButton*)parameters;
  if(button_reference->ButtonA.read()){
    // Let's us know that a button has been pressed. 
    xEventGroupSetBits(button_reference->wait_button_isr, BIT0);
    xEventGroupSetBits(button_reference->wait_button_isr, BIT1);
    button_reference->button_uuid = 1; 
  }
}

void button_b_interrupt(void *parameters){
    M5QuickReadButton *button_reference = (M5QuickReadButton*)parameters;
    if(button_reference->ButtonB.read()){
      // Let's us know that a button has been pressed. 
      xEventGroupSetBits(button_reference->wait_button_isr, BIT0);
      xEventGroupSetBits(button_reference->wait_button_isr, BIT2);
      button_reference->button_uuid = 2; 
    }
}

void button_c_interrupt(void *parameters){
    M5QuickReadButton *button_reference = (M5QuickReadButton*)parameters;
    if(button_reference->ButtonC.read()){
      // Let's us know that a button has been pressed. 
      xEventGroupSetBits(button_reference->wait_button_isr, BIT0);
      xEventGroupSetBits(button_reference->wait_button_isr, BIT3);
      button_reference->button_uuid = 3; 
    }
}

void M5QuickReadButton::begin(void){
  this->wait_button_isr = xEventGroupCreate();
  xEventGroupClearBits(this->wait_button_isr, BIT0);
  
  // Setting up our GPIO for the buttons 
  ButtonA.begin(BUTTON_A_PIN, 1, 0);
  ButtonB.begin(BUTTON_B_PIN, 1, 0);
  ButtonC.begin(BUTTON_C_PIN, 1, 0);
  
  // Setting up ISRs. So we don't need to deal with polling. 
  gpio_install_isr_service(0);
  gpio_isr_handler_add(gpio_num_t(BUTTON_A_PIN), button_a_interrupt,  (void*)this);
  gpio_isr_handler_add(gpio_num_t(BUTTON_B_PIN), button_b_interrupt,  (void*)this);
  gpio_isr_handler_add(gpio_num_t(BUTTON_C_PIN), button_c_interrupt,  (void*)this);

}

uint8_t M5QuickReadButton::wait_button(void){
   // If there are any set bits already due being pushed already, we gotta get rid of that bs. 
   xEventGroupClearBits(this->wait_button_isr, BIT0);
   
  // Sits and waits for the group to get what they want. 
  xEventGroupWaitBits(this->wait_button_isr, BIT0, true, true, portMAX_DELAY);
  xEventGroupClearBits(this->wait_button_isr, BIT0);
  uint8_t ret = this->button_uuid;
  // resets the unique identifier. 
  this->button_uuid = 0;
  return ret; 
}

void M5QuickReadButton::wait_a(void){
  // If there are any set bits already due being pushed already, we gotta get rid of that bs.
  xEventGroupClearBits(this->wait_button_isr, BIT1);
        
  // Sits and waits for the group to get what they want. 
  xEventGroupWaitBits(this->wait_button_isr, BIT1, true, true, portMAX_DELAY);
  xEventGroupClearBits(this->wait_button_isr, BIT1);
}

void M5QuickReadButton::wait_b(void){
  xEventGroupClearBits(this->wait_button_isr, BIT2);
        
  // Sits and waits for the group to get what they want. 
  xEventGroupWaitBits(this->wait_button_isr, BIT2, true, true, portMAX_DELAY);
  xEventGroupClearBits(this->wait_button_isr, BIT2);
}

void M5QuickReadButton::wait_c(void){
  
  // If there are any set bits already due being pushed already, we gotta get rid of that bs.
  xEventGroupClearBits(this->wait_button_isr, BIT3);
        
  // Sits and waits for the group to get what they want. 
  xEventGroupWaitBits(this->wait_button_isr, BIT3, true, true, portMAX_DELAY);
  xEventGroupClearBits(this->wait_button_isr, BIT3);
}

bool M5QuickReadButton::a_last_pressed(void){
  return ButtonA.get();
}

bool M5QuickReadButton::b_last_pressed(void){
  return ButtonB.get();
}

bool M5QuickReadButton::c_last_pressed(void){
  return ButtonC.get();
}

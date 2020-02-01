void bidet_task(void *parameters){
  uint16_t bidet_ticker = 0; 
  TickType_t last_bidet_time;
  for(;;){
    last_bidet_time = xTaskGetTickCount();
    
    if(auto_bidet){
      if(turbidity > 2290){
        if(!toilet_reset){
          HeaatControl.publish("cwhite");
        }
        toilet_reset = true;  
      }
      if(turbidity <= 2290){
        if(toilet_reset){
          toilet_reset = false;   
          uint8_t ran = random(0,4);
          MatrixControl.publish("cred");
          HeaatControl.publish("cred");
          switch(ran){
          case(0):
            MatrixControl.publish("m Someone is having a bad day :(");
            break;
          case(1):
            MatrixControl.publish("m Someone is having a shitty day :(");
            break;
          case(2):
            MatrixControl.publish("m Remembder to clean the toilet!"); 
            break;
          case(3):
            MatrixControl.publish("m Dayum son :0"); 
            break;
          default:
            MatrixControl.publish("Ohh shiiiiiittt");
            break;  
          }
          bidet.cycle_valve(4000);
        }
      }
    }
    
    if(mqtt_read_task.get_subscription_status(0)){
      String str = (char*)Bidet_Control.lastread;
      if(Bidet_Control.lastread[0] == ';'){
        if(str.equals(";bidet_on")){
          bidet.open_valve();
        }
        else if(str.equals(";bidet_off")){
          bidet.close_valve();
        }
        else if(str.equals(";bidet_cycle")){
          bidet.cycle_valve(3000);
        }
        else if(str.equals(";bidet_auto_on")){
          auto_bidet = true; 
        }
        else if(str.equals(";bidet_auto_off")){
          auto_bidet = false; 
        }
      }
    }
    
    // Half second tick total, allowing us to check for bideting stuff twice a second at max speed. 
    TickType_t del_tick = pdMS_TO_TICKS(100);
    if(last_bidet_time + del_tick > xTaskGetTickCount()){
      vTaskDelayUntil(&last_bidet_time, del_tick); 
    }
  }
}

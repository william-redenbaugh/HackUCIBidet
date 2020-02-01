/*
Author: William Redenbaugh
Last Edit Date: 2/1/2020
Notes:
Cloud integration to talk to the mqtt broker.  
*/

void cloud_task(void *parameters){
  (void*)parameters; 
  uint16_t network_ticker = 0; 
  TickType_t last_network_time;

  for(;;){
    // Check latest tick!
    last_network_time = xTaskGetTickCount();

    // Ticktockity
    network_ticker++; 

    // Reading the turbidity sensor. 
    turbidity = bidet.read_turbidity();
    
    // Every second, read mqtt stuff. 
    if(network_ticker&10){
      mqtt_read_task.task();
      // Just keeping tabs on the turbidity sensor once every second xD. 
    }

    // resets ticker counter every minute, also includes any minute periodic functions. 
    if(network_ticker == 40){
      // While the
      if(!toilet_reset){
        BidetControlPublish.publish(turbidity);   
      }
      network_ticker = 0; 
    }
    
    // Half second tick total, allowing us to check for networking stuff twice a second at max speed. 
    TickType_t del_tick = pdMS_TO_TICKS(100);
    if(last_network_time + del_tick > xTaskGetTickCount()){
      vTaskDelayUntil(&last_network_time, del_tick); 
    }
  }

  // Just in case :)
  vTaskDelete(NULL);
}

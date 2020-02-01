void m5_setup(void){
 // Initialize the M5Stack object
  M5.begin();
  // Set LCD brightness to 200/?
  M5.Lcd.setBrightness(200);

  M5.Power.begin();
  M5.Power.setPowerBoostKeepOn(true);
  M5.Power.setCharge(true);
  M5.Power.setPowerBoostKeepOn(true);

  // Setup the power. 
  M5.Lcd.printf("M5Stack Battery Setup\r\n");
  M5.Power.begin();

  quick_read.begin();

  setup_wifi();
  delay(50);
  
  //When connected set 
  M5.Lcd.print("WiFi connected! IP address: ");
  M5.Lcd.println(WiFi.localIP());
  delay(1000);
}

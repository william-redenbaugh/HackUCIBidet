#include "m5_display_helper.h"
#define TFT_GREY 0x5AEB

// So we can put the clock where ever we want on the screen.
#define ROUND_X_OFFSET 40

// so we can put the clock where ever we want on the screen
#define DIGITAL_Y_OFFSET 70

uint16_t cyan = 0x0000; 

uint16_t concat_color(uint8_t red, uint8_t green, uint8_t blue){
  uint16_t col = 0x0000; 

  // COLOR BITSHIFTING AND ERROR CHECKING BEGIN //
  if(red > 31)
    col |= 31 << 11; 
  else
    col |= red << 11;   
  if(green > 63)
    col |= 63 << 5; 
  else
    col |= green << 5;  
  if(blue > 31)
    col |=31;
  else
    col |=blue;
  return col; 

  // COLOR BITSHIFTING AND ERROR CHECKING END //
}

// FIX THIS!! //
uint16_t concat_color_scaled(uint8_t r, uint8_t g, uint8_t b){
  uint16_t col = 0x0000; 
  
  // Scales the colors, just loses resolution. 
  uint8_t red = (r >> 11), green = (g >> 10), blue = (b >> 11); 
  
  // COLOR BITSHIFTING AND ERROR CHECKING BEGIN //
  if(red > 31)
    col |= 31 << 11; 
  else
    col |= red << 11;   
  if(green > 63)
    col |= 63 << 5; 
  else
    col |= green << 5;  
  if(blue > 31)
    col |=31;
  else
    col |=blue;
  return col; 

  // COLOR BITSHIFTING AND ERROR CHECKING END //
}

void M5DisplayHelper::begin(void){
  cyan = concat_color(0, 63, 32);
}

void time_show_task(void *parameters){
  M5DisplayHelper *display_helper_ptr = (M5DisplayHelper*)parameters; 
  display_helper_ptr->current_animation = 1; 
  
  // Allows us to accurately count a seccond. 
  TickType_t single_seccond_delay = 1000 / portTICK_PERIOD_MS;
  TickType_t latest_tick;
  
  while(1){
   // Wait every seccond...
   latest_tick = xTaskGetTickCount();
   display_helper_ptr->add_seccond_display();

    // so long as it hasn't been a second yet...
    if(latest_tick  > xTaskGetTickCount() - single_seccond_delay){
      // we shall suspend the task until the alloted time has passed. 
      vTaskDelayUntil( &latest_tick, single_seccond_delay);
    }
  }
  vTaskDelete(NULL);  
}

void round_time_show_task(void *parameters){
  M5DisplayHelper *display_helper_ptr = (M5DisplayHelper*)parameters; 
  display_helper_ptr->current_animation = 2; 
  
  //M5.Lcd.drawPngFile(SD, "/blind_will.png");
    
  // Allows us to accurately count a seccond. 
  TickType_t single_seccond_delay = 1000 / portTICK_PERIOD_MS;
  TickType_t latest_tick;
  
  while(1){
   // Wait every seccond...
   latest_tick = xTaskGetTickCount();
   display_helper_ptr->round_show();

    // so long as it hasn't been a second yet...
    if(latest_tick  > xTaskGetTickCount() - single_seccond_delay){
      // we shall suspend the task until the alloted time has passed. 
      vTaskDelayUntil( &latest_tick, single_seccond_delay);
    }
  }
  vTaskDelete(NULL);  
}

void M5DisplayHelper::show_time(TimeModule *time_mdl_ptr){
  this->time_ptr = time_mdl_ptr;
  this->hh = this->time_ptr->hour; 
  this->ss = this->time_ptr->seccond; 
  this->mm = this->time_ptr->minute; 
  this->chk_time = 0;

  // Clear and draw a picture. 
  M5.Lcd.clear();
  //M5.Lcd.drawPngFile(SD, "/blind_will.png");
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_WHITE, cyan);
  M5.Lcd.fillScreen(cyan);

  // TIME DRAW BEGIN //
   // Update digital time
  int xpos = 0;
  int ypos = 7; // Top left corner ot clock text, about half way down
  int ysecs = ypos + 24;
  omm = mm;
  // Draw hours and minutes
  if (hh < 10) xpos += M5.Lcd.drawChar('0', xpos, ypos + DIGITAL_Y_OFFSET, 8); // Add hours leading zero for 24 hr clock
  xpos += M5.Lcd.drawNumber(hh, xpos, ypos + DIGITAL_Y_OFFSET, 8);             // Draw hours
  xcolon = xpos; // Save colon coord for later to flash on/off later
  xpos += M5.Lcd.drawChar(':', xpos, ypos - 8 + DIGITAL_Y_OFFSET, 8);
  if (mm < 10) xpos += M5.Lcd.drawChar('0', xpos, ypos + DIGITAL_Y_OFFSET, 8); // Add minutes leading zero
  xpos += M5.Lcd.drawNumber(mm, xpos, ypos + DIGITAL_Y_OFFSET, 8);             // Draw minutes
  xsecs = xpos; // Sae seconds 'x' position for later display updates
  oss = ss;
  xpos = xsecs;
  if (ss % 2) { // Flash the colons on/off
    M5.Lcd.setTextColor(TFT_WHITE, cyan);
    M5.Lcd.drawChar(':', xcolon, ypos - 8 + DIGITAL_Y_OFFSET, 8);     // Hour:minute colon
    xpos += M5.Lcd.drawChar(':', xsecs, ysecs + DIGITAL_Y_OFFSET, 6); // Seconds colon
  }
  else {
    M5.Lcd.drawChar(':', xcolon, ypos - 8 + DIGITAL_Y_OFFSET, 8);     // Hour:minute colon
    xpos += M5.Lcd.drawChar(':', xsecs, ysecs + DIGITAL_Y_OFFSET, 6); // Seconds colon
  }
  //Draw seconds
  if (ss < 10) xpos += M5.Lcd.drawChar('0', xpos, ysecs + DIGITAL_Y_OFFSET, 6); // Add leading zero
  M5.Lcd.drawNumber(ss, xpos, ysecs + DIGITAL_Y_OFFSET, 6);                     // Draw seconds
  // TIME DRAW END //
  xTaskCreate(time_show_task, "receiver helper task", 10000, (void*)this, 40, &this->digital_clock_task_handler);  
}

void M5DisplayHelper::add_seccond_display(void){

  this->chk_time++;
  
  // Every 120 seconds or so, we check to make sure that the clock still matches up
  if(this->chk_time >  120){
    this->chk_time = 0; 
    this->time_ptr->get();
    this->hh = this->time_ptr->hour; 
    this->ss = this->time_ptr->seccond; 
    this->mm = this->time_ptr->minute; 
  }
  // Adjust the time values by adding 1 second
  ss++;              // Advance second
  if (ss == 60) {    // Check for roll-over
    ss = 0;          // Reset seconds to zero
    omm = mm;        // Save last minute time for display update
    mm++;            // Advance minute
    if (mm > 59) {   // Check for roll-over
      mm = 0;
      hh++;          // Advance hour
      if (hh > 23) { // Check for 24hr roll-over (could roll-over on 13)
        hh = 0;      // 0 for 24 hour clock, set to 1 for 12 hour clock
      }
    }
  }


  // Update digital time
  int xpos = 0;
  int ypos = 7; // Top left corner ot clock text, about half way down
  int ysecs = ypos + 24;
  if (omm != mm) { // Redraw hours and minutes time every minute
    omm = mm;
    // Draw hours and minutes
    if (hh < 10) xpos += M5.Lcd.drawChar('0', xpos, ypos + DIGITAL_Y_OFFSET, 8); // Add hours leading zero for 24 hr clock
    xpos += M5.Lcd.drawNumber(hh, xpos, ypos + DIGITAL_Y_OFFSET, 8);             // Draw hours
    xcolon = xpos; // Save colon coord for later to flash on/off later
    xpos += M5.Lcd.drawChar(':', xpos, ypos - 8 + DIGITAL_Y_OFFSET, 8);
    if (mm < 10) xpos += M5.Lcd.drawChar('0', xpos, ypos + DIGITAL_Y_OFFSET, 8); // Add minutes leading zero
    xpos += M5.Lcd.drawNumber(mm, xpos, ypos + DIGITAL_Y_OFFSET, 8);             // Draw minutes
    xsecs = xpos; // Sae seconds 'x' position for later display updates
  }
  if (oss != ss) { // Redraw seconds time every second
    oss = ss;
    xpos = xsecs;

    if (ss % 2) { // Flash the colons on/off
      M5.Lcd.setTextColor(TFT_WHITE, cyan);
      M5.Lcd.drawChar(':', xcolon, ypos - 8 + DIGITAL_Y_OFFSET, 8);     // Hour:minute colon
      xpos += M5.Lcd.drawChar(':', xsecs, ysecs + DIGITAL_Y_OFFSET, 6); // Seconds colon
    }
    else {
      M5.Lcd.drawChar(':', xcolon, ypos - 8 + DIGITAL_Y_OFFSET, 8);     // Hour:minute colon
      xpos += M5.Lcd.drawChar(':', xsecs, ysecs + DIGITAL_Y_OFFSET, 6); // Seconds colon
    }

    //Draw seconds
    if (ss < 10) xpos += M5.Lcd.drawChar('0', xpos, ysecs + DIGITAL_Y_OFFSET, 6); // Add leading zero
    M5.Lcd.drawNumber(ss, xpos, ysecs + DIGITAL_Y_OFFSET, 6);                     // Draw seconds
  }
}

void M5DisplayHelper::end_digital(void){
  vTaskSuspend(this->digital_clock_task_handler);
  vTaskDelete(this->digital_clock_task_handler); 
}

void M5DisplayHelper::show_time_round(TimeModule *time_mdl_ptr){
  this->time_ptr = time_mdl_ptr;
  this->hh = this->time_ptr->hour; 
  this->ss = this->time_ptr->seccond; 
  this->mm = this->time_ptr->minute; 
  this->chk_time = 0; 
  
  // M5.Lcd.setRotation(0);
  
  
  //M5.Lcd.fillScreen(TFT_BLACK);
  //M5.Lcd.fillScreen(TFT_RED);
  //M5.Lcd.fillScreen(TFT_GREEN);
  M5.Lcd.fillScreen(cyan);
  //M5.Lcd.fillScreen(TFT_BLACK);
  //M5.Lcd.fillScreen(TFT_GREY);
  
  M5.Lcd.setTextColor(TFT_WHITE, cyan);
  
  // Draw clock face
  M5.Lcd.fillCircle(120 + ROUND_X_OFFSET, 120, 118, TFT_BLUE);
  M5.Lcd.fillCircle(120 + ROUND_X_OFFSET, 120, 110, TFT_GREY);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*114+120;
    yy0 = sy*114+120;
    x1 = sx*100+120;
    yy1 = sy*100+120;

    M5.Lcd.drawLine(x0 + ROUND_X_OFFSET, yy0, x1 +  + ROUND_X_OFFSET, yy1, TFT_WHITE);
  }

  // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*102+120;
    yy0 = sy*102+120;
    // Draw minute markers
    M5.Lcd.drawPixel(x0 + ROUND_X_OFFSET, yy0, TFT_WHITE);
    
    // Draw main quadrant dots
    if(i==0 || i==180) M5.Lcd.fillCircle(x0 + ROUND_X_OFFSET, yy0, 2, TFT_WHITE);
    if(i==90 || i==270) M5.Lcd.fillCircle(x0 + ROUND_X_OFFSET, yy0, 2, TFT_WHITE);
  }

  M5.Lcd.fillCircle(120 + ROUND_X_OFFSET, 121, 3, TFT_WHITE);
  
  xTaskCreate(round_time_show_task, "receiver helper task", 10000, (void*)this, 40, &this->analog_clock_task_handler);  
}

void M5DisplayHelper::round_show(void){
  this->chk_time++; 
  // Every 120 seconds or so, we check to make sure that the clock still matches up
  if(this->chk_time >  120){
    this->chk_time = 0; 
    this->time_ptr->get();
    this->hh = this->time_ptr->hour; 
    this->ss = this->time_ptr->seccond; 
    this->mm = this->time_ptr->minute; 
  }
  
  ss++;              // Advance second
  if (ss==60) {
    ss=0;
    mm++;            // Advance minute
    if(mm>59) {
      mm=0;
      hh++;          // Advance hour
      if (hh>23) {
        hh=0;
      }
    }
  }

  // Pre-compute hand degrees, x & y coords for a fast screen update
  sdeg = ss*6;                  // 0-59 -> 0-354
  mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
  hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
  hx = cos((hdeg-90)*0.0174532925);    
  hy = sin((hdeg-90)*0.0174532925);
  mx = cos((mdeg-90)*0.0174532925);    
  my = sin((mdeg-90)*0.0174532925);
  sx = cos((sdeg-90)*0.0174532925);    
  sy = sin((sdeg-90)*0.0174532925);

  if (ss==0 || initial) {
    initial = 0;
    // Erase hour and minute hand positions every minute
    M5.Lcd.drawLine(ohx + ROUND_X_OFFSET, ohy, 120 + ROUND_X_OFFSET, 121, TFT_GREY);
    ohx = hx*62+121;    
    ohy = hy*62+121;
    M5.Lcd.drawLine(omx + ROUND_X_OFFSET, omy, 120 + ROUND_X_OFFSET, 121, TFT_GREY);
    omx = mx*84+120;    
    omy = my*84+121;
  }

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    M5.Lcd.drawLine(osx + ROUND_X_OFFSET, osy, 120 + ROUND_X_OFFSET, 121, TFT_GREY);
    osx = sx*90+121;    
    osy = sy*90+121;
    M5.Lcd.drawLine(osx + ROUND_X_OFFSET, osy, 120 + ROUND_X_OFFSET, 121, TFT_RED);
    M5.Lcd.drawLine(ohx + ROUND_X_OFFSET, ohy, 120 + ROUND_X_OFFSET, 121, TFT_WHITE);
    M5.Lcd.drawLine(omx + ROUND_X_OFFSET, omy, 120 + ROUND_X_OFFSET, 121, TFT_WHITE);
    M5.Lcd.drawLine(osx + ROUND_X_OFFSET, osy, 120 + ROUND_X_OFFSET, 121, TFT_RED);

  M5.Lcd.fillCircle(120 + ROUND_X_OFFSET, 121, 3, TFT_RED);
}

void M5DisplayHelper::end_round(void){
  vTaskSuspend(this->analog_clock_task_handler);
  vTaskDelete(this->analog_clock_task_handler); 
}

void M5DisplayHelper::initgame(void){
  lpaddle_y = random(0, h - paddle_h);
  rpaddle_y = random(0, h - paddle_h);

  // ball is placed on the center of the left paddle
  ball_y = lpaddle_y + (paddle_h / 2);
  
  calc_target_y();

  midline();

  // M5.Lcd.fillRect(0,h-26,w,239,GREY);

  // M5.Lcd.setTextDatum(TC_DATUM);
  // M5.Lcd.setTextColor(WHITE,GREY);
  // M5.Lcd.drawString("TFT_eSPI example", w/2, h-26 , 4);
}

void M5DisplayHelper::midline(void){
  // If the ball is not on the line then don't redraw the line
  if ((ball_x<dashline_x-ball_w) && (ball_x > dashline_x+dashline_w)) return;

  // Quick way to draw a dashed line
  M5.Lcd.setAddrWindow(dashline_x, 0, dashline_w, h);
  
  for(int16_t i = 0; i < dashline_n; i+=2) {
    M5.Lcd.pushColor(WHITE, dashline_w*dashline_h); // push dash pixels
    M5.Lcd.pushColor(BLACK, dashline_w*dashline_h); // push gap pixels
  }
}

void M5DisplayHelper::lpaddle(void){
  if (lpaddle_d == 1) {
    M5.Lcd.fillRect(lpaddle_x, lpaddle_y, paddle_w, 1, BLACK);
  } 
  else if (lpaddle_d == -1) {
    M5.Lcd.fillRect(lpaddle_x, lpaddle_y + paddle_h - 1, paddle_w, 1, BLACK);
  }

  lpaddle_y = lpaddle_y + lpaddle_d;

  if (ball_dx == 1) lpaddle_d = 0;
  else {
    if (lpaddle_y + paddle_h / 2 == target_y) lpaddle_d = 0;
    else if (lpaddle_y + paddle_h / 2 > target_y) lpaddle_d = -1;
    else lpaddle_d = 1;
  }

  if (lpaddle_y + paddle_h >= h && lpaddle_d == 1) lpaddle_d = 0;
  else if (lpaddle_y <= 0 && lpaddle_d == -1) lpaddle_d = 0;

  M5.Lcd.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, WHITE);
}

void M5DisplayHelper::rpaddle(void){
  if (rpaddle_d == 1) {
    M5.Lcd.fillRect(rpaddle_x, rpaddle_y, paddle_w, 1, BLACK);
  } 
  else if (rpaddle_d == -1) {
    M5.Lcd.fillRect(rpaddle_x, rpaddle_y + paddle_h - 1, paddle_w, 1, BLACK);
  }

  rpaddle_y = rpaddle_y + rpaddle_d;

  if (ball_dx == -1) rpaddle_d = 0;
  else {
    if (rpaddle_y + paddle_h / 2 == target_y) rpaddle_d = 0;
    else if (rpaddle_y + paddle_h / 2 > target_y) rpaddle_d = -1;
    else rpaddle_d = 1;
  }

  if (rpaddle_y + paddle_h >= h && rpaddle_d == 1) rpaddle_d = 0;
  else if (rpaddle_y <= 0 && rpaddle_d == -1) rpaddle_d = 0;

  M5.Lcd.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, WHITE);
}

void M5DisplayHelper::calc_target_y(void){
  int16_t target_x;
  int16_t reflections;
  int16_t y;

  if (ball_dx == 1) {
    target_x = w - ball_w;
  } 
  else {
    target_x = -1 * (w - ball_w);
  }

  y = abs(target_x * (ball_dy / ball_dx) + ball_y);

  reflections = floor(y / h);

  if (reflections % 2 == 0) {
    target_y = y % h;
  } 
  else {
    target_y = h - (y % h);
  }
}

void M5DisplayHelper::ball(void){
  ball_x = ball_x + ball_dx;
  ball_y = ball_y + ball_dy;

  if (ball_dx == -1 && ball_x == paddle_w && ball_y + ball_h >= lpaddle_y && ball_y <= lpaddle_y + paddle_h) {
    ball_dx = ball_dx * -1;
    dly = random(5); // change speed of ball after paddle contact
    calc_target_y();

  } else if (ball_dx == 1 && ball_x + ball_w == w - paddle_w && ball_y + ball_h >= rpaddle_y && ball_y <= rpaddle_y + paddle_h) {
    ball_dx = ball_dx * -1;
    dly = random(5); // change speed of ball after paddle contact
    calc_target_y();
  } else if ((ball_dx == 1 && ball_x >= w) || (ball_dx == -1 && ball_x + ball_w < 0)) {
    dly = 5;
  }

  if (ball_y > h - ball_w || ball_y < 0) {
    ball_dy = ball_dy * -1;
    ball_y += ball_dy; // Keep in bounds
  }

  //M5.Lcd.fillRect(oldball_x, oldball_y, ball_w, ball_h, BLACK);
  M5.Lcd.drawRect(oldball_x, oldball_y, ball_w, ball_h, BLACK); // Less TFT refresh aliasing than line above for large balls
  M5.Lcd.fillRect(   ball_x,    ball_y, ball_w, ball_h, WHITE);
  oldball_x = ball_x;
  oldball_y = ball_y;
}

void pong_task(void *parameters){
  M5DisplayHelper *display_helper_ptr = (M5DisplayHelper*)parameters; 
  
  // Clear and draw a picture. 
  M5.Lcd.clear();
  display_helper_ptr->current_animation = 3; 
  
  // Setup the game!
  display_helper_ptr->initgame();
  
  for(;;){
    display_helper_ptr->run_pong();
  }
  vTaskDelete(NULL);
}

void M5DisplayHelper::run_pong(void){
  
  for(uint8_t i = 0; i < 4; i++){
    delay(10);
    lpaddle();
    rpaddle();
    midline();
    ball();
  }
}

void M5DisplayHelper::pong_beep(void){
  if(beep){
    beep = 0; 
    M5.Speaker.tone(-1, 50);
  } 
}

void M5DisplayHelper::start_pong(void){
  xTaskCreate(pong_task, "receiver helper task", 10000, (void*)this, 40, &this->pong_task_handler);
}

void M5DisplayHelper::end_pong(void){
  vTaskSuspend(this->pong_task_handler);
  vTaskDelete(this->pong_task_handler);   
}

uint8_t M5DisplayHelper::show_options(M5QuickReadButton *button, String option_one, uint16_t col_one, String option_two, uint16_t col_two, String option_three, uint16_t col_three){

  // Draw the background of the gui
  M5.Lcd.fillRect(0, 0, 107, 240, col_one);
  M5.Lcd.fillRect(106, 0, 107, 240, col_two);
  M5.Lcd.fillRect(213, 0, 107, 240, col_three);

  // Draw the text for the gui. 
  M5.Lcd.setTextColor(TFT_BLACK, col_one);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(5, 115);
  M5.Lcd.println(option_one);

  M5.Lcd.setTextColor(TFT_BLACK, col_two);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(112, 115);
  M5.Lcd.println(option_two);

  M5.Lcd.setTextColor(TFT_BLACK, col_three);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(219, 115);
  M5.Lcd.println(option_three);

  uint8_t num = button->wait_button();
  switch(num){
    case(1):
      M5.Lcd.fillRect(0, 0, 107, 240, TFT_WHITE);
      break;
    case(2):
      M5.Lcd.fillRect(106, 0, 107, 240, TFT_WHITE);
      break;
    case(3):
      M5.Lcd.fillRect(213, 0, 107, 240, TFT_WHITE);
      break;
  }
  return num;
}

int16_t M5DisplayHelper::get_number(M5QuickReadButton *button, String str, uint16_t col, int16_t min_num, int16_t max_num, int16_t start_num){
    // Text Setup
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(TFT_WHITE, col);

    // The number we eventually return 
    int16_t number = start_num;
    
    while(1){
      M5.Lcd.fillScreen(col);
      M5.Lcd.setCursor(10, 10);
      M5.Lcd.print(str);
      
      if(number >= 10 && number < 100){
        M5.Lcd.setCursor(150, 110);
        M5.Lcd.println(number);  
      }
      else if(number >= 100){
        M5.Lcd.setCursor(145, 110);
        M5.Lcd.println(number); 
      }
      else if(number < 10){
        M5.Lcd.setCursor(155, 110);
        M5.Lcd.println(number); 
      }

      // Wait for a button
      uint8_t sel = button->wait_button();
      if(sel == 1){
        number --; 
        if(number < min_num)
          number = max_num; 
      }
      if(sel == 2){
        return number; 
      }
      if(sel == 3){
        number++;
        if(number > max_num)
          number = min_num;  
      }
    }
}

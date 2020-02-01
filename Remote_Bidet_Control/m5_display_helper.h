#ifndef _M5_DISPLAY_HELPER_H
#define _M5_DISPLAY_HELPER_H

#include "Arduino.h"
#include <M5Stack.h>
#include "wifi_quick_setup.h"
#include "WiFiTimeModule.h"
#include "m5_quick_setup.h"

// Color manipulation algorithms
uint16_t concat_color(uint8_t red, uint8_t green, uint8_t blue);
uint16_t concat_color_scaled(uint8_t r, uint8_t g, uint8_t b);

class M5DisplayHelper{
  public: 
    uint16_t current_animation = 0; 
    
    void begin(void);

    // DIGITAL CLOCK FUNCTIONS BEGIN // 
    void show_time(TimeModule *time_mdl_ptr);
    void add_seccond_display(void);
    void end_digital(void);
    // DIGITAL CLOCK FUNCTIONS END //

    // ANALOG CLOCK FUNCTIONS BEGIN // 
    void show_time_round(TimeModule *time_mdl_ptr);
    void round_show(void);
    void end_round(void);
    // ANALOG CLOCK FUNCTIONS END // 

    
    // PONG FUNCTIONS BEGIN // 
    void initgame(void);
    void lpaddle(void);
    void rpaddle(void);
    void midline(void);
    void ball(void);
    void calc_target_y(void);

    void start_pong(void);
    void end_pong(void);
    void run_pong(void);

    void pong_beep(void);
    // PONG FUNCTIONS END // 

    // INTRO GUI FUNCTIONS BEGIN // 
    uint8_t show_options(M5QuickReadButton *button, String option_one, uint16_t col_one, String option_two, uint16_t col_two, String option_three, uint16_t col_three);
    int16_t get_number(M5QuickReadButton *button, String str, uint16_t col, int16_t min_num, int16_t max_num, int16_t start_num);
    // INTRO GUI FUNCTIONS END //
      
  private:

    
    // TIME VARIABLES BEGIN //
    volatile uint8_t hh = 1;
    volatile uint8_t mm = 43;
    volatile uint8_t ss = 20;
    TimeModule *time_ptr; 
    volatile uint8_t chk_time = 0; 
    // TIME VARIABLES END //

    // DIGITAL CLOCK VARIABLES BEGIN // 
    volatile byte omm = 99, oss = 99;
    volatile byte xcolon = 0, xsecs = 0;
    volatile unsigned int colour = 0;
    TaskHandle_t digital_clock_task_handler;
    // DIGITAL CLOCK VARIABLES END // 

    // ROUND CLOCK VARIABLES BEGIN // 
    volatile float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
    volatile float sdeg=0, mdeg=0, hdeg=0;
    volatile uint16_t  osx=120, osy=120, omx=120, omy=120, ohx=120, ohy=120;  // Saved H, M, S x & y coords
    volatile uint16_t  x0=0, x1=0, yy0=0, yy1=0;
    volatile boolean initial = 1;
    TaskHandle_t analog_clock_task_handler;
    // ROUND CLOCK VARIABLES END // 

    // PONG BACKGROUND BEGIN // 
    int16_t h = 240;
    int16_t w = 320;
    
    int dly = 5;
    
    int16_t paddle_h = 30;
    int16_t paddle_w = 4;
    
    int16_t lpaddle_x = 0;
    int16_t rpaddle_x = w - paddle_w;
    
    int16_t lpaddle_y = 0;
    int16_t rpaddle_y = h - paddle_h;
    
    int16_t lpaddle_d = 1;
    int16_t rpaddle_d = -1;
    
    int16_t lpaddle_ball_t = w - w / 4;
    int16_t rpaddle_ball_t = w / 4;
    
    int16_t target_y = 0;
    
    int16_t ball_x = 2;
    int16_t ball_y = 2;
    int16_t oldball_x = 2;
    int16_t oldball_y = 2;
    
    int16_t ball_dx = 1;
    int16_t ball_dy = 1;
    
    int16_t ball_w = 6;
    int16_t ball_h = 6;
    
    int16_t dashline_h = 4;
    int16_t dashline_w = 2;
    int16_t dashline_n = h / dashline_h;
    int16_t dashline_x = w / 2 - 1;
    int16_t dashline_y = dashline_h / 2;
    
    int16_t lscore = 12;
    int16_t rscore = 4;

    uint8_t beep = 0; 
    TaskHandle_t pong_task_handler;
    // PONG BACKGROUND END // 
   };

#endif

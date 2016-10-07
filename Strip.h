#include "Mav2Leds.h"

class Strip
{
  private:
    long timer1;     // milliseconds of on-time
    long timer2;    // milliseconds of off-time
    long timer3;    // milliseconds of off-time
    long timer4;    // milliseconds of off-time
  
    // These maintain the current state
    int stateIdx;
    long previousMillis;   // will store last time LED was updated
    CRGB* __leds1;
    CRGB* __leds2;
    int __led_idx;
    int __inv_led_idx;
    SysState __obj;
    byte __leds_mode;
    CRGB __leds_color;
    LedMode __led_mode = {0, 1, 2, 3, 4, 5, 6};
    Position __position;
    byte __pos;
    bool __odd = false;
    bool __bounceback = false;
    int __cycle6 = 0;
    int __cycle12 = 0;
    int __cycle24 = 0;
    int __steps = 255/12;
    int __inv_cycle12;
    
    void ParseMode();
    void Step1();
    void Step2();
    void Step3();
    void Step4();
    int GetPrevious(int __offset);
    
  public:
    Strip();
    void Attach(CRGB* leds1, CRGB* leds2, byte pos);
    void Update(SysState __SysState);
};
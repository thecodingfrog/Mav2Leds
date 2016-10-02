#include "Mav2Leds.h"

class Strip
{
  private:
    int ledState;                 // ledState used to set the LED
    unsigned long OnTime = 200;     // milliseconds of on-time
    unsigned long OffTime = 50;    // milliseconds of off-time
    unsigned long previousMillis;   // will store last time LED was updated
    CRGB* __leds1;
    CRGB* __leds2;
    CRGB* __leds3;
    CRGB* __leds4;
    LedMode __led_mode = {0, 1, 2, 3};
    byte __front_leds_mode;
    CRGB __front_leds_color;
    byte __rear_leds_mode;
    CRGB __rear_leds_color;
    SysState __sys_state;
    
    void ParseMode();
    void Show();
    void Hide();
    void ChaseFront(CRGB __color);
    void ChaseRear(CRGB __color);
    
  protected:
    int __inc = 0;
    int __dec = 5;
    
  public:
    Strip();
    void Attach(CRGB* leds1, CRGB* leds2, CRGB* leds3, CRGB* leds4);
    void Update(SysState __SysState);
};

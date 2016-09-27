#include "Mav2Leds.h"

class Strip
{
  private:
    int ledState;                 // ledState used to set the LED
    unsigned long OnTime = 50;     // milliseconds of on-time
    unsigned long OffTime = 50;    // milliseconds of off-time
    unsigned long previousMillis;   // will store last time LED was updated
    CRGB* __leds1;
    CRGB* __leds2;
    CRGB* __leds3;
    CRGB* __leds4;
    int __inc = 0;
    int __dec = 5;
    void Show();
    void Hide();
    
  public:
    Strip();
    void Attach(CRGB* leds1, CRGB* leds2, CRGB* leds3, CRGB* leds4);
    void Update(SysState __SysState);
};

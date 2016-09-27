#include "Arduino.h"
#include <FastLED.h>
#include "Strip.h"

Strip::Strip()
{
  previousMillis = 0;  
}

void Strip::Attach(CRGB* leds1, CRGB* leds2, CRGB* leds3, CRGB* leds4)
{
  __leds1 = leds1;
  __leds2 = leds2;
  __leds3 = leds3;
  __leds4 = leds4;
}

void Strip::Update(SysState __SysState)
{
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();

  __inc++;
  if (__inc > 5) __inc = 0;
  __dec--;
  if (__dec < 0) __dec = 5;
   
  if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
  {
    ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    Show();
  }
  else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
  {
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    Hide();
  }
}

void Strip::Show()
{
  /*__leds1[__led_idx] = CRGB::Green;
  __leds2[__led_idx] = CRGB::Green;
  __leds3[__led_idx] = CRGB::Green;
  __leds4[__led_idx] = CRGB::Green;
  FastLED.show();*/
}

void Strip::Hide()
{
  /*__leds1[__led_idx] = CRGB::Black;
  __leds2[__led_idx] = CRGB::Black;
  __leds3[__led_idx] = CRGB::Black;
  __leds4[__led_idx] = CRGB::Black;
  FastLED.show();*/
}

#include "Arduino.h"
#include <FastLED.h>
#include "Strobe.h"

Strobe::Strobe(long on, long off, long on2, long off2, long on3, long off3)
{
  OnTime = on;
  OffTime = off;
  OnTime2 = on2;
  OffTime2 = off2;
  OnTime3 = on3;
  OffTime3 = off3;
  
  ledState = LOW;
  stateIdx = 1;
  previousMillis = 0;  
}

void Strobe::Attach(CRGB* leds1, CRGB* leds2, CRGB* leds3, CRGB* leds4, int __idx)
{
  __FR_leds = leds1;
  __FL_leds = leds2;
  __RR_leds = leds3;
  __RL_leds = leds4;
  __led_idx = __idx;
  //fill_solid(__obj, 8, CRGB::Blue);
  //FastLED.show();
}

void Strobe::Update()
{
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();
   
  if((stateIdx == 2) && (currentMillis - previousMillis >= OnTime))
  {
    stateIdx = 3;
    ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    Hide();
  }
  else if ((stateIdx == 1) && (currentMillis - previousMillis >= OffTime))
  {
    stateIdx = 2;
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    Show();
  }
  else if((stateIdx == 4) && (currentMillis - previousMillis >= OnTime2))
  {
    stateIdx = 5;
    ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    Hide();
  }
  else if ((stateIdx == 3) && (currentMillis - previousMillis >= OffTime2))
  {
    stateIdx = 4;
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    Show();
  }
  
  else if((stateIdx == 6) && (currentMillis - previousMillis >= OnTime3))
  {
    stateIdx = 1;
    ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    Hide();
  }
  else if ((stateIdx == 5) && (currentMillis - previousMillis >= OffTime3))
  {
    stateIdx = 6;
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    Show();
  }
}

void Strobe::Show()
{
  __FR_leds[__led_idx] = CRGB::Green;
  __FL_leds[__led_idx] = CRGB::Green;
  __RR_leds[__led_idx] = CRGB::Red;
  __RL_leds[__led_idx] = CRGB::Red;
  FastLED.show();
}

void Strobe::Hide()
{
  __FR_leds[__led_idx] = CRGB::Black;
  __FL_leds[__led_idx] = CRGB::Black;
  __RR_leds[__led_idx] = CRGB::Black;
  __RL_leds[__led_idx] = CRGB::Black;
  FastLED.show();
}


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
  __leds1 = leds1;
  __leds2 = leds2;
  __leds3 = leds3;
  __leds4 = leds4;
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
  __leds1[__led_idx] = CRGB::Green;
  __leds2[__led_idx] = CRGB::Green;
  __leds3[__led_idx] = CRGB::Green;
  __leds4[__led_idx] = CRGB::Green;
  FastLED.show();
}

void Strobe::Hide()
{
  __leds1[__led_idx] = CRGB::Black;
  __leds2[__led_idx] = CRGB::Black;
  __leds3[__led_idx] = CRGB::Black;
  __leds4[__led_idx] = CRGB::Black;
  FastLED.show();
}


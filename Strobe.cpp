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

void Strobe::Attach(CRGB* __leds, int __idx)
{
  __obj = __leds;
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
  __obj[__led_idx] = CRGB::Green;
  FastLED.show();
}

void Strobe::Hide()
{
  __obj[__led_idx] = CRGB::Black;
  FastLED.show();
}


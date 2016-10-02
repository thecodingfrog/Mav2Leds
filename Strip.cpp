#include "Arduino.h"
#include <FastLED.h>
#include "Strip.h"

Strip::Strip()
{
  previousMillis = 0;
  __front_leds_color = CRGB::Yellow;
  __rear_leds_color = CRGB::Yellow;
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
  __sys_state = __SysState;
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();

  ParseMode();
   
  if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
  {
    ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    Hide();
  }
  else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
  {
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    
    Show();
  }
}

void Strip::ParseMode()
{
  if ( (__sys_state.battery / __sys_state.num_cells) <= (LOW_BATT_2 * 10) )
  {    // low battery detected, for front led color change, in this case bright white = full batt, orange is batt warning..
    __front_leds_mode = __led_mode.blink;
    __rear_leds_mode = __led_mode.blink;
    __front_leds_color = CRGB::OrangeRed;
    __rear_leds_color = CRGB::OrangeRed;
  }
  else
  {
    __front_leds_mode = __led_mode.chase;
    __rear_leds_mode = __led_mode.chase;
    __front_leds_color = CRGB::OrangeRed;
    __rear_leds_color = CRGB::OrangeRed;
  } 
}

void Strip::Show()
{
  /*if (__front_leds_mode == __led_mode.on || __front_leds_mode == __led_mode.blink)
  {
    fill_solid(__leds1, 6, __front_leds_color);
    fill_solid(__leds2, 6, __front_leds_color);
  }
  if (__rear_leds_mode == __led_mode.on || __rear_leds_mode == __led_mode.blink)
  {
    fill_solid(__leds3, 6, __rear_leds_color);
    fill_solid(__leds4, 6, __rear_leds_color);
  }*/
  
  __leds1[__inc] = __front_leds_color;
  __leds2[__inc] = __front_leds_color;
  __leds3[__inc] = __rear_leds_color;
  __leds4[__inc] = __rear_leds_color;
  /*if (__rear_leds_mode == __led_mode.chase)
  {
    __leds3[__inc] = __rear_leds_color;
    __leds4[__inc] = __rear_leds_color;
  }*/
  FastLED.show();
}

void Strip::Hide()
{
  /*if (__front_leds_mode == __led_mode.blink || __rear_leds_mode == __led_mode.blink)
  {
    for (int j = 0; j < NUM_LEDS_PER_STRIP - 2; j++)
    {
      if (__front_leds_mode == __led_mode.blink)
      {
        __leds1[j] = CRGB::Black;
        __leds2[j] = CRGB::Black;
      }
      if (__rear_leds_mode == __led_mode.blink)
      {
        __leds3[j] = CRGB::Black;
        __leds4[j] = CRGB::Black;
      }
    }
  }*/
  ChaseFront(CRGB::Black);
  ChaseRear(CRGB::Black);
  ++__inc;
  if (__inc > 5) __inc = 0;
  //__dec == 5 - __inc;
}

void Strip::ChaseFront(CRGB __color)
{
  __leds1[__inc] = __color;
  __leds2[__inc] = __color;
  FastLED.show();
}

void Strip::ChaseRear(CRGB __color)
{
  __leds3[__inc] = __color;
  //__leds4[__inc] = __color;
  FastLED.show();
}


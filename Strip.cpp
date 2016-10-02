#include "Arduino.h"
#include <FastLED.h>
#include "Strip.h"

Strip::Strip()
{
  timer1 = 50;
  timer2 = 50;
  timer3 = 50;
  
  stateIdx = 1;
  previousMillis = 0;  

  __led_idx = 0;
  __inv_led_idx = 5;
}

void Strip::Attach(CRGB* leds1, CRGB* leds2, byte pos)
{
  __leds1 = leds1;
  __leds2 = leds2;  
  __pos = pos;
}

void Strip::Update(SysState __sys_state)
{
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();
   
  if ((stateIdx == 1) && (currentMillis - previousMillis >= timer1))
  {
    stateIdx = 2;
    previousMillis = currentMillis;   // Remember the time
    ParseMode();
    Show();
  }
  else if((stateIdx == 2) && (currentMillis - previousMillis >= timer2))
  {
    stateIdx = 3;
    previousMillis = currentMillis;  // Remember the time
    Fade();
  }
  else if ((stateIdx == 3) && (currentMillis - previousMillis >= timer3))
  {
    stateIdx = 1;
    previousMillis = currentMillis;   // Remember the time
    Hide();
  }
}

void Strip::ParseMode()
{
  __odd = !__odd;
  ++__cycle;
  if (__cycle > 12) __cycle = 0;
  
  if ( (__sys_state.battery / __sys_state.num_cells) <= (LOW_BATT_2 * 10) )
  {    // low battery detected, for front led color change, in this case bright white = full batt, orange is batt warning..
    __leds_mode = __led_mode.blink;
    __leds_color = CRGB::OrangeRed;
  }
  else
  {
    if (__sys_state.system_state > 0 && __sys_state.system_state < 2) // MAV_STATE_BOOT
    {
      __leds_mode = __led_mode.blink;
      __leds_color = CRGB::Yellow;
    }
    else if (__sys_state.system_state == 2) // MAV_STATE_CALIBRATING
    {
      __leds_mode = __led_mode.on;
      __leds_color = (__odd) ? CRGB::Blue : CRGB::Red;
    }
    else if (__sys_state.system_state > 4) // MAV_STATE_CRITICAL and over  
    {
      __leds_mode = __led_mode.blink;
      __leds_color = CRGB::Yellow;
    }
    else if (__sys_state.system_state > 2 && __sys_state.system_state < 5) //   MAV_STATE_STANDBY and MAV_STATE_ACTIVE  
    {
      if (__sys_state.isArmed == 0) // disarmed: led chasing, if GPS 3D lock white color, if not 3D lock orange
      {
        if (__sys_state.gps_fix_type == 3) // 3D Fix
        {
          __leds_mode = __led_mode.blink;
          __leds_color = CRGB::Green;
        }        
        else
        {
          __leds_mode = __led_mode.blink;
          __leds_color = CRGB::Blue;
        }
      }
      else if (__sys_state.flight_mode_str == "stab" && __sys_state.isArmed == 1) // armed & manual flight: front leds white with increasing intensity, but if lowbatt is detected, it changes to orange
      {
        __leds_mode = (__pos == __position.front) ? __led_mode.chaseback : __led_mode.chase;
        if (__sys_state.gps_fix_type == 3) // 3D Fix
        {
          __leds_color = (__pos == __position.front) ? CRGB::Green : CRGB::Red;
        }
        else
        { 
          __leds_color = (__pos == __position.front) ? CRGB::Blue : CRGB::Red;
        }    
      }
      else if (__sys_state.flight_mode_str == "alth") // armed & alt hold without GPS: front 3 led on, (white) front 1st led and rear leds flashing (orange)
      {
        __leds_mode = (__pos == __position.front) ? __led_mode.chaseback : __led_mode.blink;
        if (__sys_state.gps_fix_type == 3) // 3D Fix
        {
          __leds_color = (__pos == __position.front) ? CRGB::Green : CRGB::Red;
        }
        else 
        {
          __leds_color = (__pos == __position.front) ? CRGB::Blue : CRGB::Red;
        }
      }
      else if (__sys_state.flight_mode_str == "phld") // armed & position hold: front leds on, (white) rear leds short flashing (green)
      {
        __leds_mode = __led_mode.blink;
        if (__sys_state.gps_fix_type == 3) // 3D Fix
        {
          __leds_color = (__pos == __position.front) ? CRGB::Green : CRGB::Red;
        }
        else
        {
          __leds_color = (__pos == __position.front) ? CRGB::Blue : CRGB::Red;
        }
      }
      else if (__sys_state.flight_mode_str == "loit") // LOITER
      {
        __leds_mode = __led_mode.chaseback;
        __leds_color = (__pos == __position.front) ? CRGB::Green : CRGB::Red;
      }
      else if (__sys_state.flight_mode_str == "land") // LAND
      {
        __leds_mode = __led_mode.chase;
        __leds_color = CRGB::Purple;
      }
      else if (__sys_state.flight_mode_str == "rtl") // RTL
      {
        __leds_mode = __led_mode.chase;
        __leds_color = (__cycle > 6) ? CRGB::Green : CRGB::Purple;
      }
      else if (__sys_state.flight_mode_str == "auto") // AUTO
      {
        __leds_mode = (__pos == __position.front) ? __led_mode.chase : __led_mode.chaseback;
        __leds_color = CRGB::Green;
      }
      else if (__sys_state.flight_mode_str == "tune") // AUTOTUNE
      {
        __leds_mode = __led_mode.blink;
        __leds_color = CRGB::Cyan;
      }
    }
    else // no valid signal, 1 red led
    {
      __leds_mode = __led_mode.blink;
      __leds_color = (__odd) ? CRGB::Red : CRGB::OrangeRed;
    }
  }

  if (__leds_mode == __led_mode.blink)
  {
    timer1 = 200;
    timer2 = 50;
    timer3 = 50;
  }
  else
  {
    timer1 = 50;
    timer2 = 50;
    timer3 = 50;
  }
}

void Strip::Show()
{
  if (__leds_mode == __led_mode.chase)
  {
    __leds1[__led_idx] = __leds_color;
    __leds2[__led_idx] = __leds_color;
  }
  if (__leds_mode == __led_mode.chaseback)
  {
    __leds1[__inv_led_idx] = __leds_color;
    __leds2[__inv_led_idx] = __leds_color;
  }
  if (__leds_mode == __led_mode.on || __leds_mode == __led_mode.blink)
  {
    fill_solid(__leds1, 6, __leds_color);
    fill_solid(__leds2, 6, __leds_color);
  }
  
  FastLED.show();
}

void Strip::Fade()
{
  if (__leds_mode == __led_mode.chase)
  {
    __leds1[__led_idx].fadeToBlackBy(230);
    __leds2[__led_idx].fadeToBlackBy(230);
  }
  if (__leds_mode == __led_mode.chaseback)
  {
    __leds1[__inv_led_idx].fadeToBlackBy(230);
    __leds2[__inv_led_idx].fadeToBlackBy(230);
  }
  FastLED.show();
}

void Strip::Hide()
{
  int __previous = GetPrevious(1);
  int __inv_previous = 5 - __previous;
  
  if (__leds_mode == __led_mode.chase)
  {
    __leds1[__previous] = CRGB::Black;
    __leds2[__previous] = CRGB::Black;
  }
  if (__leds_mode == __led_mode.chaseback)
  {
    __leds1[__inv_previous] = CRGB::Black;
    __leds2[__inv_previous] = CRGB::Black;
  }
  if (__leds_mode == __led_mode.blink)
  {
    fill_solid(__leds1, 6, CRGB::Black);
    fill_solid(__leds2, 6, CRGB::Black);
  }
  FastLED.show();

  ++__led_idx;
  if (__led_idx > 5) __led_idx = 0;
  __inv_led_idx = 5 - __led_idx;
}

int Strip::GetPrevious(int __offset)
{
  int __previous;
  __previous = __led_idx - __offset;
  if (__previous < 0) __previous = 5;
  return __previous;
}


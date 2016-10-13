#include "Arduino.h"
#include <FastLED.h>
#include "SysState.h"
#include "Strip.h"

Strip::Strip()
{
  timer1 = 50;
  timer2 = 50;
  timer3 = 50;
  timer4 = 50;
  
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
  __obj = __sys_state;
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();
   
  if ((stateIdx == 1) && (currentMillis - previousMillis >= timer1))
  {
    stateIdx = 2;
    previousMillis = currentMillis;   // Remember the time
    ParseMode();
    Step1();
  }
  else if((stateIdx == 2) && (currentMillis - previousMillis >= timer2))
  {
    stateIdx = 3;
    previousMillis = currentMillis;  // Remember the time
    Step2();
  }
  else if ((stateIdx == 3) && (currentMillis - previousMillis >= timer3))
  {
    stateIdx = 4;
    previousMillis = currentMillis;   // Remember the time
    Step3();
  }
  else if ((stateIdx == 4) && (currentMillis - previousMillis >= timer4))
  {
    stateIdx = 1;
    previousMillis = currentMillis;   // Remember the time
    Step4();
  }
}

void Strip::ParseMode()
{
  __odd = !__odd;

  ++__cycle6;
  if (__cycle6 > 6)
  {
    __cycle6 = 0;
  }
  
  ++__cycle12;
  if (__cycle12 > 12)
  {
    __bounceback = !__bounceback;
    __cycle12 = 0;
  }
  
  ++__cycle24;
  if (__cycle24 > 24)
  {
    //__bounceback = false;
    __cycle24 = 0;
  }
  
  if ((__obj.battery / __obj.num_cells) <= (LOW_BATT_2 * 10))
  {    // low battery detected, for front led color change, in this case bright white = full batt, orange is batt warning..
    __leds_mode = __led_mode.chain;
    __leds_color = CRGB::OrangeRed;
  }
  else
  {
    if (__obj.system_state < 2) // MAV_STATE_BOOT
    {
      __leds_mode = __led_mode.blink;
      __leds_color = CRGB::Yellow;
    }
    else if (__obj.system_state == 2) // MAV_STATE_CALIBRATING
    {
      __leds_mode = __led_mode.blink;
      __leds_color = (__odd) ? CRGB::Blue : CRGB::Red;
    }
    else if (__obj.system_state > 4 || __obj.has_error) // MAV_STATE_CRITICAL and over  
    {
      __leds_mode = __led_mode.blink;
      __leds_color = CRGB::Yellow;
    }
    else if (__obj.system_state > 2 && __obj.system_state < 5) //   MAV_STATE_STANDBY and MAV_STATE_ACTIVE  
    {
      if (__obj.is_armed == 0) // disarmed: led chasing, if GPS 3D lock white color, if not 3D lock orange
      {
        if (__obj.gps_fix_type == 3) // 3D Fix
        {
          __leds_mode = __led_mode.pulse;
          __leds_color = CRGB::Green;
        }
        else
        {
          __leds_mode = __led_mode.pulse;
          __leds_color = CRGB::Blue;
        }
      }
      else if (__obj.flight_mode_str == "stab" && __obj.is_armed == 1) // armed & manual flight: front leds white with increasing intensity, but if lowbatt is detected, it changes to orange
      {
        __leds_mode = (__pos == __position.front) ? __led_mode.chaseback : __led_mode.chase;
        if (__obj.gps_fix_type == 3) // 3D Fix
        {
          __leds_color = (__pos == __position.front) ? CRGB::Red : CRGB::Green;
        }
        else
        { 
          __leds_color = (__pos == __position.front) ? CRGB::Red : CRGB::Blue;
        }    
      }
      else if (__obj.flight_mode_str == "alth") // armed & alt hold without GPS: front 3 led on, (white) front 1st led and rear leds flashing (orange)
      {
        __leds_mode = (__pos == __position.front) ? __led_mode.chaseback : __led_mode.pulse;
        if (__obj.gps_fix_type == 3) // 3D Fix
        {
          __leds_color = (__pos == __position.front) ? CRGB::Red : CRGB::Green;
        }
        else 
        {
          __leds_color = (__pos == __position.front) ? CRGB::Red : CRGB::Blue;
        }
      }
      else if (__obj.flight_mode_str == "phld") // armed & position hold: front leds on, (white) rear leds short flashing (green)
      {
        __leds_mode = __led_mode.pulse;
        if (__obj.gps_fix_type == 3) // 3D Fix
        {
          __leds_color = (__pos == __position.front) ? CRGB::Red : CRGB::Green;
        }
        else
        {
          __leds_color = (__pos == __position.front) ? CRGB::Red : CRGB::Blue;
        }
      }
      else if (__obj.flight_mode_str == "loit") // LOITER
      {
        __leds_mode = (__pos == __position.front) ? __led_mode.chaseback : __led_mode.on;
        __leds_color = (__pos == __position.front) ? CRGB::Red : CRGB::Green;
      }
      else if (__obj.flight_mode_str == "land") // LAND
      {
        __leds_mode = __led_mode.bounce;
        __leds_color = CRGB::Purple;
      }
      else if (__obj.flight_mode_str == "rtl") // RTL
      {
        __leds_mode = __led_mode.bounce;
        __leds_color = (!__bounceback) ? CRGB::Green : CRGB::Purple;
      }
      else if (__obj.flight_mode_str == "auto") // AUTO
      {
        __leds_mode = (__pos == __position.front) ? __led_mode.chase : __led_mode.chaseback;
        __leds_color = CRGB::Green;
      }
      else if (__obj.flight_mode_str == "tune") // AUTOTUNE
      {
        __leds_mode = __led_mode.pulse;
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
    timer1 = 600;
    timer2 = 70;
    timer3 = 70;
    timer4 = 70;
  }
  else if (__leds_mode == __led_mode.pulse)
  {
    timer1 = 50;
    timer2 = 10;
    timer3 = 10;
    timer4 = 10;
  }
  else if (__leds_mode == __led_mode.chain)
  {
    timer1 = 200;
    timer2 = 10;
    timer3 = 200;
    timer4 = 10;
  }
  else if (__leds_mode == __led_mode.chase || __leds_mode == __led_mode.chaseback)
  {
    timer1 = 50;
    timer2 = 10;
    timer3 = 10;
    timer4 = 10;
  }
  else if (__leds_mode == __led_mode.bounce)
  {
    timer1 = 50;
    timer2 = 10;
    timer3 = 10;
    timer4 = 10;
  }
  else
  {
    timer1 = 50;
    timer2 = 50;
    timer3 = 50;
    timer4 = 10;
  }
}

void Strip::Step1()
{
  if (__leds_mode == __led_mode.chase)
  {
    for (int idx = 0; idx < 6; idx++)
    {
      if (__cycle12 - idx >= 0 && __cycle12 - idx <= 5)
      {
        __leds1[__cycle12 - idx] = __leds_color;
        __leds1[__cycle12 - idx].fadeToBlackBy(255/5 * idx);
        __leds2[__cycle12 - idx] = __leds_color;
        __leds2[__cycle12 - idx].fadeToBlackBy(255/5 * idx);
      }
    }
  }
  if (__leds_mode == __led_mode.chaseback)
  {
    __inv_cycle12 = 6 - __cycle12;
    for (int idx = 0; idx < 6; idx++)
    {
      if (__inv_cycle12 + idx >= 0 && __inv_cycle12 + idx <= 5)
      {
        __leds1[__inv_cycle12 + idx] = __leds_color;
        __leds1[__inv_cycle12 + idx].fadeToBlackBy(255/5 * idx);
        __leds2[__inv_cycle12 + idx] = __leds_color;
        __leds2[__inv_cycle12 + idx].fadeToBlackBy(255/5 * idx);
      }
    }
  }
  if (__leds_mode == __led_mode.on || __leds_mode == __led_mode.blink)
  {
    fill_solid(__leds1, 6, __leds_color);
    fill_solid(__leds2, 6, __leds_color);
  }
  if (__leds_mode == __led_mode.chain)
  {
    for (int i = 0; i < 7; i++)
    {
      __leds1[i] = (i % 2) ? __leds_color : CRGB::Black;
      __leds2[i] = (i % 2) ? __leds_color : CRGB::Black;
    }
  }
  if (__leds_mode == __led_mode.bounce && !__bounceback)
  {
    for (int idx = 0; idx < 6; idx++)
    {
      if (__cycle12 - idx >= 0 && __cycle12 - idx <= 5)
      {
        __leds1[__cycle12 - idx] = __leds_color;
        __leds1[__cycle12 - idx].fadeToBlackBy(255/5 * idx);
        __leds2[__cycle12 - idx] = __leds_color;
        __leds2[__cycle12 - idx].fadeToBlackBy(255/5 * idx);
      }
    }
  }
  if (__leds_mode == __led_mode.bounce && __bounceback)
  {
    __inv_cycle12 = 6 - __cycle12;
    for (int idx = 0; idx < 6; idx++)
    {
      if (__inv_cycle12 + idx >= 0 && __inv_cycle12 + idx <= 5)
      {
        __leds1[__inv_cycle12 + idx] = __leds_color;
        __leds1[__inv_cycle12 + idx].fadeToBlackBy(255/5 * idx);
        __leds2[__inv_cycle12 + idx] = __leds_color;
        __leds2[__inv_cycle12 + idx].fadeToBlackBy(255/5 * idx);
      }
    }
  }
  if (__leds_mode == __led_mode.pulse)
  {
    if ( __cycle24 <= 12)
    {
      if (__cycle24 == 1)
      {
        fill_solid(__leds1, 6, __leds_color);
        fill_solid(__leds2, 6, __leds_color);
      }
      fadeLightBy(__leds1, 6, __steps * __cycle24);
      fadeLightBy(__leds2, 6, __steps * __cycle24);
    }
    else
    {
      fill_solid(__leds1, 6, __leds_color);
      fill_solid(__leds2, 6, __leds_color);
      fadeLightBy(__leds1, 6, 255 - (__steps * abs(12 - __cycle24)));
      fadeLightBy(__leds2, 6,  255 - (__steps * abs(12 - __cycle24)));
    }
  }
  
  FastLED.show();
}

void Strip::Step2()
{
  if (__leds_mode == __led_mode.blink)
  {
    fill_solid(__leds1, 6, CRGB::Black);
    fill_solid(__leds2, 6, CRGB::Black);
  }
  FastLED.show();
}

void Strip::Step3()
{
  if (__leds_mode == __led_mode.blink)
  {
    fill_solid(__leds1, 6, __leds_color);
    fill_solid(__leds2, 6, __leds_color);
  }
  if (__leds_mode == __led_mode.chain)
  {
    for (int i = 0; i < 6; i++)
    {
      __leds1[i] = (i % 2) ? CRGB::Black : __leds_color;
      __leds2[i] = (i % 2) ? CRGB::Black : __leds_color;
    }
  }
  FastLED.show();
}

void Strip::Step4()
{
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


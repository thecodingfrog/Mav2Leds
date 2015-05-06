/* functions for the LPD8806 RGB ledstrip controller 
 * in this case the strips are cut in strands, multiple sets of 2 led eg 4 leds
 * the number of leds needs to be set in this file
 * 
 * For this implementation the strand is 8 leds, but cut in two pieces of four LED's. 
 * The strips are however connected using the SPI interface in-out so for the SPI bus it is still 8 leds
 * Using this on an quad-x, leds 0-3 on the front arms, leds 4-7 on the rear
 *
 * LED patterns are hard coded to the flightstatus number but use your imagination to write your own patterns 
 */

/*int FL[] = {0};
int FR[] = {8};
int FRONT[] = {FL[0],FR[0]};
int RL[] = {0};
int RR[] = {8};
int REAR[] = {RL[0],RR[0]};*/

/* First parameter is the number of LEDs in the strand.  The LED strips
 * are 32 LEDs per meter but you can extend or cut the strip.  Next two
 * parameters are SPI data and clock pins: */

Preserved preserved_leds = { 0 , 1 , 2 };

void RGBInitialize()
{
  for(int i = 0; i < 5 ; i++)
  {
    colorChaseAll(CRGB::White, 50);
    colorChaseBackAll(CRGB::White, 50);
  }
}

void circle()
{
  for(int i = 0; i < 10; i++)
  {
    for(int x = 0; x < NUM_STRIPS; x++)
    {
      if (x > 0)
      {
        for(int j = 0; j < NUM_LEDS_PER_STRIP; j++)
        {
          leds[x - 1][j] = CRGB::Black;
          FastLED.show();
        }
      }
      for(int j = 0; j < NUM_LEDS_PER_STRIP; j++)
      {
        leds[x][j] = CRGB::Blue;
      }
      FastLED.show();
      delay(100);
    }
    for(int j = 0; j < NUM_LEDS_PER_STRIP; j++)
    {
      leds[NUM_STRIPS - 1][j] = CRGB::Black;
      FastLED.show();
    }
  }
}

void RGBControl()
{
  if ( (m2h_vbat_A / m2h_num_cells) <= (LOW_BATT_2 * 10) )
  {    // low battery detected, for front led color change, in this case bright white = full batt, orange is batt warning..
    colorBlink(CRGB::Orange, -1, -1, 50, 3, CRGB::Orange, preserved_leds.external);
  }
  else
  {
  
    if (m2h_sysstat == 5) // MAV_STATE_CRITICAL
    {
      colorBlink(CRGB::Yellow, -1, -1, 50, 3, CRGB::Yellow, preserved_leds.none);
    }
    else if (isArmed == 0) // disarmed: led chasing, if GPS 3D lock white color, if not 3D lock orange
    {
      if (m2h_fix_type == 3)
        colorBlink(CRGB::Green, -1, -1, 50, 3, CRGB::Green, preserved_leds.none);
      else 
        colorBlink(CRGB::Blue, -1, -1, 50, 3, CRGB::Blue, preserved_leds.none);
    }
    else if (mode_str == "stab" && isArmed == 1) // armed & manual flight: front leds white with increasing intensity, but if lowbatt is detected, it changes to orange
    {
      if (m2h_fix_type == 3)
      {
        colorChaseBack(CRGB::Blue, 0, 1, 50, preserved_leds.external);
      }
      else
      { 
        colorChaseBack(CRGB::Green, 0, 1, 50, preserved_leds.external);
      }
      colorChase(CRGB::Red, 2, 3, 50, false);
    
    }
    else if (mode_str == "alth") // armed & alt hold without GPS: front 3 led on, (white) front 1st led and rear leds flashing (orange)
    {
      if (m2h_fix_type == 3)
      {
        colorChaseBack(CRGB::Blue, 0, 1, 50, preserved_leds.external);
      }
      else 
      {
        colorChaseBack(CRGB::Green, 0, 1, 50, preserved_leds.external);
      }
      colorBlink(CRGB::Red, 2, 3, 50, 3, preserved_leds.external);
    }
    else if (mode_str == "phld") // armed & position hold: front leds on, (white) rear leds short flashing (green)
    {
      if (m2h_fix_type == 3)
        colorBlink(CRGB::Green, -1, -1, 50, 3, CRGB::Red, preserved_leds.external);
      else
        colorBlink(CRGB::Blue, -1, -1, 50, 3, CRGB::Red, preserved_leds.external);
    }
    else if (mode_str == "loit") // LOITER
    {
      colorBlink(CRGB::Green, -1, -1, 50, 3, CRGB::Green, preserved_leds.internal);
    }
    else if (mode_str == "land") // LAND
    {
      colorChaseBack(CRGB::Green, -1, -1, 50);
    }
    else if (mode_str == "rtl") // RTL
    {
      colorChaseBack(CRGB::Green, -1, -1, 50, preserved_leds.internal);
    }
    else if (mode_str == "auto") // AUTO
    {
      colorChase(CRGB::Green, -1, -1, 50, preserved_leds.internal);
      colorChaseBack(CRGB::Green, -1, -1, 50, preserved_leds.internal);
    }
    else if (mode_str == "tune") // AUTOTUNE
    {
      colorBlink(CRGB::Purple, -1, -1, 50, 3, CRGB::Purple, preserved_leds.external);
    }
   
    /*case 11:      // lost signal: red / blue alternating
    {
      colorBlink(CRGB::Yellow, -1, -1, 50, 3, CRGB::Yellow, preserved_leds.none);
    }
    break;*/ 
  
    else // no valid signal, 1 red led
    {
      colorBlink(CRGB::White, -1, -1, 50, 3, CRGB::White, preserved_leds.none);
      
      //colorChaseBackAll(CRGB::Red, 50);
    }
  }   
}

/**
* Color single all strips
*/
void colorArmAll(CRGB c)
{
  clearstrips();
  
  colorArm(c, -1, -1);
}

/**
* Color single strip
*/
void colorArm(CRGB c, int idx1, int idx2)
{
  for(int x = 0; x < NUM_STRIPS; x++)
  {
    if (x == idx1 || x == idx2 || idx1 == -1) fill_solid(leds[x], NUM_LEDS_PER_STRIP, c);
  }
  FastLED.show();
}

/**
* Color single strip
*/
void colorArm(CRGB c, int idx1, int idx2, byte preserved)
{
  for(int x = 0; x < NUM_STRIPS; x++)
  {
    if (x == idx1 || x == idx2 || idx1 == -1)
    {
      for(int j = 0; j < NUM_LEDS_PER_STRIP; j++)
      {
        if (j + 1 >= 7 && preserved == preserved_leds.external)
        {
          
        }
        else if (j + 1 <= 1 && preserved == preserved_leds.internal)
        {
          
        }
        else
        {
          leds[x][j] = c;
        }
      }
    }
  }
  FastLED.show();
}

/**
* Color chase forward all strips
*/
void colorChaseAll(CRGB c, uint8_t wait)
{
  clearstrips();
  
  colorChase(c, -1, -1, wait);
}

/**
* Color chase forward
*/
void colorChase(CRGB c, int idx1, int idx2, uint8_t wait)
{
  colorChase(c, idx1, idx2, wait, true);
}

/**
* Color chase forward
*/
void colorChase(CRGB c, int idx1, int idx2, uint8_t wait, boolean cycle)
{
  for(int j = 0; j < NUM_LEDS_PER_STRIP; j++)  // turn all pixels off
  {
    for(int x = 0; x < NUM_STRIPS; x++)
    {
      if (x == idx1 || x == idx2 || idx1 == -1) leds[x][j] = c;
    }
    FastLED.show();
    delay(wait);
    if (cycle)
    {
      for(int x = 0; x < NUM_STRIPS; x++)
      {
        if (x == idx1 || x == idx2 || idx1 == -1) leds[x][j] = CRGB::Black;
      }
    }
  }
  
  FastLED.show(); // for last erased pixel
}

/**
* Color blink
*/
void colorBlink(CRGB c, int idx1, int idx2, uint8_t wait)
{
  colorBlink(c, idx1, idx2, wait, 1);
}

/**
* Color blink
*/
void colorBlink(CRGB c, int idx1, int idx2, uint8_t wait, int cycle)
{
  colorBlink(c, idx1, idx2, wait, cycle, CRGB::Black, preserved_leds.none);
}

/**
* Color blink
*/
void colorBlink(CRGB c, int idx1, int idx2, uint8_t wait, int cycle, byte preserved)
{
  colorBlink(c, idx1, idx2, wait, cycle, CRGB::Black, preserved);
}

/**
* Color blink
*/
void colorBlink(CRGB c, int idx1, int idx2, uint8_t wait, int cycle, CRGB c2, byte preserved)
{
  for(int x = 0; x < cycle; x++)
  {
    if (idx1 == -1 || idx2 == -1)
    {
      colorArm(c, 0, 1); // FRONT
      colorArm(c2, 2, 3); // REAR
    }
    else
    {
      colorArm(c, idx1, idx2);
    }
    FastLED.show();
    delay(wait);
    colorArm(CRGB::Black, idx1, idx2, preserved);
    FastLED.show(); // for last erased pixel
    delay(wait);
  }
  if (cycle > 1)
    {
      if (idx1 == -1 || idx2 == -1)
        delay(300);
      else
        delay(50);
    }
}

/**
* Color chase backward
*/
void colorChaseBackAll(CRGB c, uint8_t wait)
{
  clearstrips();
  
  colorChaseBack(c, -1, -1, wait, preserved_leds.none);
}

/**
* Color chase backward
*/
void colorChaseBack(CRGB c, int idx1, int idx2, uint8_t wait)
{  
  colorChaseBack(c, idx1, idx2, wait, preserved_leds.none);
}

/**
* Color chase backward
*/
void colorChaseBack(CRGB c, int idx1, int idx2, uint8_t wait, byte preserved)
{
  for(int j = NUM_LEDS_PER_STRIP - 1; j > -1; j--)  // turn all pixels off
  {
    for(int x = 0; x < NUM_STRIPS; x++)
    {
      if (x == idx1 || x == idx2 || idx1 == -1) leds[x][j] = c;
    }
    FastLED.show();
    delay(wait);
    for(int x = 0; x < NUM_STRIPS; x++)
    {
      if (x == idx1 || x == idx2 || idx1 == -1)
      {
        if (j + 1 >= 7 && preserved == preserved_leds.external)
        {
          
        }
        else if (j + 1 <= 1 && preserved == preserved_leds.internal)
        {
          
        }
        else
        {
          leds[x][j] = CRGB::Black;
        }
      }
    }
  }
  
  FastLED.show(); // for last erased pixel
}

/**
* Clear all strips
*/
void clearstrips()
{
  for(int x = 0; x < NUM_STRIPS; x++)  // turn all pixels off
  {
    fill_solid(leds[x], NUM_LEDS_PER_STRIP, CRGB::Black);
  }
  FastLED.show();
}


/**
* Clear single strip
* @param idx Strip index to be wiped
*/
void clearstrip(int idx)
{
  fill_solid(leds[idx], NUM_LEDS_PER_STRIP, CRGB::Black);
  FastLED.show();
}


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
#ifdef LPD8806RGB
/*int FL[] = {0};
int FR[] = {8};
int FRONT[] = {FL[0],FR[0]};
int RL[] = {0};
int RR[] = {8};
int REAR[] = {RL[0],RR[0]};*/

/* First parameter is the number of LEDs in the strand.  The LED strips
 * are 32 LEDs per meter but you can extend or cut the strip.  Next two
 * parameters are SPI data and clock pins: */

void RGBInitialize()
{
  for(int i = 0; i < 10 ; i++)
  {
    colorChase(CRGB::White, 50);
    colorChaseBack(CRGB::White, 50);
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
  if ( (m2h_vbat_A / m2h_num_cells) <= (LOW_BATT_2 * 10) ){    // low battery detected, for front led color change, in this case bright white = full batt, orange is batt warning..
    r=127;
    g=32;
    b=0;
  }else {
    r=127;
    g=127;
    b=127;
  } 
  switch (flMode) {
    case 0:            // all off
    {
      //clearStrip();
    }
    break;    

    case 1:            // disarmed: led chasing, if GPS 3D lock white color, if not 3D lock orange
    {
      if (m2h_fix_type == 3) 
        colorChase(CRGB::Green, 50);
      else 
        colorChase(CRGB::Blue, 50);
    }
    break;    

    case 2:            // armed & manual flight: front leds white with increasing intensity, but if lowbatt is detected, it changes to orange
    {
       /*strip.setPixelColor(3, strip.Color(r/8, b/8, g/8));  // front leds white     
       strip.setPixelColor(2, strip.Color(r/4, b/4, g/4));  // front leds white
       strip.setPixelColor(1, strip.Color(r/2, b/2, g/2));  // front leds white     
       strip.setPixelColor(0, strip.Color(r, b, g));        // front leds white     
       wipe2ndArm();
       strip.show();*/
    }
    break;    

    case 3:           // armed & alt hold without GPS: front 3 led on, (white) front 1st led and rear leds flashing (orange)
    {    
    /*for (int i=0; i < (strip.numPixels()/2); i++) 
    {
      strip.setPixelColor(i, strip.Color(r, b, g));  // front leds color depending on batt status

      if (ioCounter == 3 || ioCounter ==4){
       strip.setPixelColor(i+(strip.numPixels()/2), strip.Color(127, 0, 32));   // orange
       strip.setPixelColor(0, 0);                            // first front led off
      }else{
       strip.setPixelColor(i+(strip.numPixels()/2), 0);      // rear off
       strip.setPixelColor(0, strip.Color(127, 0, 32));      // first front led orange       
      }
    }
    strip.show();*/
    }
    break;
    
    case 4:           // armed & position hold: front leds on, (white) rear leds short flashing (green)
    {
    /*for (int i=0; i < (strip.numPixels()/2); i++) 
    {
      strip.setPixelColor(i, strip.Color(r, b, g));          // front leds color depending on batt status
      if (ioCounter <= 3)
       strip.setPixelColor(i+(strip.numPixels()/2), strip.Color(0, 0, 127));   // green
      else
       strip.setPixelColor(i+(strip.numPixels()/2), 0);      // rear off
    }
    strip.show();*/
    }
    break;     
    
    case 5:         // armed & mission mode: front and rear leds alternating slow flashing
    {

      /*for (int i=0; i < (strip.numPixels()/2); i++) 
    {
      if (ioCounter >= 5){
       strip.setPixelColor(i, 0);                            // front leds off
       strip.setPixelColor(i+(strip.numPixels()/2), strip.Color(0, 0, 127));   // green
      }else{
       strip.setPixelColor(i, strip.Color(r, b, g));         // front leds color depending on batt status
       strip.setPixelColor(i+(strip.numPixels()/2), 0);      // rear off
      }
    }
    strip.show();*/
    
    }
    break;    

    case 6:         // armed & DHV mode: front leds on, (white) rear leds fast flashing (green/cyan)
    {
    /* pattern like this
    * 0 1 2 3 4 5 6 7 8 9 10
    * G - C - G - C - G - C
    */
    /*for (int i=0; i < (strip.numPixels()/2); i++) 
    {
      strip.setPixelColor(i, strip.Color(r, b, g));                             // front leds color depending on batt status
      if (ioCounter == 0 || ioCounter == 4 || ioCounter ==8)
        strip.setPixelColor(i+(strip.numPixels()/2), strip.Color(0, 0, 127));   // rear green
      else if (ioCounter == 1 || ioCounter ==3 || ioCounter ==5 || ioCounter ==7)  
       strip.setPixelColor(i+(strip.numPixels()/2), strip.Color(0, 0, 0));   
      else
       strip.setPixelColor(i+(strip.numPixels()/2), strip.Color(0, 127, 127));  // rear cyan
    }
    strip.show();*/
    }
    break;    

    case 7:       // TBD (CareFree)
    {
    }
    break; 

    case 8:       // mission: Return to home 'blob' chasing
    {
      /*strip.setPixelColor(ioCounter-2, strip.Color(0, 0, 0));    // green
      strip.setPixelColor(ioCounter-1, strip.Color(8, 0, 16));   // green
      strip.setPixelColor(ioCounter, strip.Color(0, 0, 127));    // green
      strip.setPixelColor(ioCounter+1, strip.Color(8, 0, 16));   // green      
      strip.show();*/
    }
    break; 

    case 9:       // mission: circle
    {
    /*strip.setPixelColor(stripCounter, strip.Color(127, 127, 127));   // white
    strip.setPixelColor(stripCounter+4, strip.Color(127, 127, 127)); // white
    strip.setPixelColor(stripCounter-1, strip.Color(0, 0, 0));       // off
    strip.setPixelColor(stripCounter+4-1, strip.Color(0, 0, 0));     // off
    strip.show();

    stripCounter ++;
    if (stripCounter >3) stripCounter =0;*/
    }
    break; 

    case 10:       // mission: land
    {
    //colorWipe (strip.Color(127,127,127), 10);   // Landing lights : white
    }
    break; 
    
    case 11:      // lost signal: red / blue alternating
    {
    /*for (int i=0; i < (strip.numPixels()/2); i++) 
    {
      if (ioCounter == 2 || ioCounter ==3 || ioCounter ==6 || ioCounter ==7 || ioCounter ==10){
        strip.setPixelColor(i, strip.Color(0, 127, 0));   // front leds blue
        strip.setPixelColor(i+(strip.numPixels()/2), strip.Color(127, 0, 0));   // rear red
      }else{
        strip.setPixelColor(i, strip.Color(127, 0, 0));   // front leds red
        strip.setPixelColor(i+(strip.numPixels()/2), strip.Color(0, 127, 0));   // rear blue
      }
    }
    strip.show();*/
      
    }
    break;    

    case 12:      // TDB
    {
    }
    break;    
  
    default:        // no valid signal, 1 red led
      /*strip.setPixelColor(3, strip.Color(127, 0, 0));
      strip.setPixelColor(7, strip.Color(127, 0, 0));      
      strip.show();*/
    break;
    }
}

void colorChase(CRGB c, uint8_t wait)
{
  clearstrips();
  
  for(int j = 0; j < NUM_LEDS_PER_STRIP; j++)  // turn all pixels off
  {
    for(int x = 0; x < NUM_STRIPS; x++)
    {
      leds[x][j] = c;
    }
    FastLED.show();
    delay(wait);
    for(int x = 0; x < NUM_STRIPS; x++)
    {
      leds[x][j] = CRGB::Black;
    }
  }
  
  FastLED.show(); // for last erased pixel
}

void colorChaseBack(CRGB c, uint8_t wait)
{
  clearstrips();
  
  for(int j = NUM_LEDS_PER_STRIP - 1; j > -1; j--)  // turn all pixels off
  {
    for(int x = 0; x < NUM_STRIPS; x++)
    {
      leds[x][j] = c;
    }
    FastLED.show();
    delay(wait);
    for(int x = 0; x < NUM_STRIPS; x++)
    {
      leds[x][j] = CRGB::Black;
    }
  }
  
  FastLED.show(); // for last erased pixel
}

void clearstrips()
{
  for(int x = 0; x < NUM_STRIPS; x++)  // turn all pixels off
  {
    fill_solid(leds[x], NUM_LEDS_PER_STRIP, CRGB::Black);
  }
  FastLED.show();
}

void clearstrip(int idx)
{
  fill_solid(leds[idx], NUM_LEDS_PER_STRIP, CRGB::Black);
  FastLED.show();
}

#endif

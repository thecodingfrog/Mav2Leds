#include "LedController.h"

// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "wiring.h"
#endif

#include <FastLED.h>
#include <mthread.h>

CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];


LedController::LedController()
{
  FastLED.addLeds<LPD8806, FR, CLK, BRG>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<LPD8806, FL, CLK, BRG>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<LPD8806, RR, CLK, BRG>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<LPD8806, RL, CLK, BRG>(leds[2], NUM_LEDS_PER_STRIP);
}

LedController::~LedController()
{
}

bool LedController::loop()
{
    sleep_milli(50);
    
    return true;
}

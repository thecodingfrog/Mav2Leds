/*

 Copyright (c) 2015.  All rights reserved.
 An Open Source Arduino based on mav2hott
  Program    : Mav2Leds
  Version    : v1.0, 30.05.2015
  Author     : Jean-Louis PERSAT (jeanlouis.persat@free.fr)
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 http://www.gnu.org/licenses
 
*/
//////////////////////////////////////////////////////////////////////////
//  Description: 
// 
//  This Arduino sketch is used for listen to APM's or Pixhawk mavlink commands
//  change LED light patterns according to the mavlink messages.
//
//  Please note that the DigitalWriteFast library is used to speed up the 
//  IO. This library needs to be included in the Arduino library to function
//
//  If you use, redistribute this please mention original source.
//
/* **************************************************************************** */

/* ************************************************************ */
/* **************** MAIN PROGRAM - MODULES ******************** */
/* ************************************************************ */
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 

#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];})) 

#define MAVLINK10     // Are we listening MAVLink 1.0 or 0.9   (0.9 is obsolete now)
#define HEARTBEAT     // HeartBeat signal
//#define SERDB         // Output debug mavlink information to SoftwareSerial, cannot be used with HoTT output at the same time!
//#define DEBUGLED 12   // HottLoop debugLed

/* **********************************************/
/* ***************** INCLUDES *******************/
#include <SingleSerial.h> // MUST be first

// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "wiring.h"
#endif
#include "Mav2Leds.h"
#include <FastLED.h>
#include "Strobe.h"
#include "Strip.h"
#include "MAVLinkReader.h"

/* *************************************************/
/* ***************** DEFINITIONS *******************/
#define TELEMETRY_SPEED  57600    /* MAVLink telemetry speed.*/
SingleSerialPort(Serial);           /* Our Uart port for Mavlink*/

int messageCounter;
bool mavlink_active;
BetterStream *mavlink_comm_0_port;

SysState __SysState = { -1, "", 0, 0, 0, 0 };
Position __position = { 0, 1 };
MAVLinkReader __MAVLinkReader(__SysState);

Strobe strobe_led(50, 70, 50, 70, 50, 600);
Strobe strobe_led2(50, 70, 50, 600, 50, 70);
Strip strip_led_front;
Strip strip_led_rear;

CRGB __leds_FR[8];
CRGB __leds_FL[8];
CRGB __leds_RR[8];
CRGB __leds_RL[8];

void setup()
{
  FastLED.addLeds<NEOPIXEL, 5>(__leds_FR, 8);
  FastLED.addLeds<NEOPIXEL, 6>(__leds_FL, 8);
  FastLED.addLeds<NEOPIXEL, 7>(__leds_RR, 8);
  FastLED.addLeds<NEOPIXEL, 8>(__leds_RL, 8);
  strobe_led.Attach(__leds_FR, __leds_FL, __leds_RR, __leds_RL, 6);
  strobe_led2.Attach(__leds_FR, __leds_FL, __leds_RR, __leds_RL, 7);
  strip_led_front.Attach(__leds_FR, __leds_FL, __position.front);
  strip_led_rear.Attach(__leds_RR, __leds_RL, __position.rear);

  Serial.begin(TELEMETRY_SPEED);          /* Initialize Serial port, speed */
  mavlink_comm_0_port = &Serial;          /* setup mavlink port */
  /*for (int j = 5; j > -1; j--)
  {
    __leds_FR[j] = CRGB::Blue;
    __leds_FR[j].fadeToBlackBy((5 - j) * 51);
    FastLED.show();
  }*/
}

void loop()
{
  strobe_led.Update();
  strobe_led2.Update();
  strip_led_front.Update(__SysState);
  strip_led_rear.Update(__SysState);
  __MAVLinkReader.Update();
  /*fill_solid(leds_front, 8, CRGB::Green);
  fill_solid(leds_rear, 8, CRGB::Red);
  FastLED.show();
  delay(500);
  fill_solid(leds_front, 8, CRGB::Black);
  fill_solid(leds_rear, 8, CRGB::Black);
  FastLED.show();
  delay(500);*/
}

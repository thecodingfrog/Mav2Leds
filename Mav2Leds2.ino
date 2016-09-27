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
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <math.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "wiring.h"
#endif
#include "Mav2Leds.h"
#include "Strobe.h"
#include "Strip.h"
#include "MAVLinkReader.h"

/* *************************************************/
/* ***************** DEFINITIONS *******************/
#define TELEMETRY_SPEED  57600    /* MAVLink telemetry speed.*/
SingleSerialPort(Serial);           /* Our Uart port for Mavlink*/


static uint8_t hRX=7;              /* software serial port for HoTT OR! Debug */
static uint8_t hTX=8;              /* if using the JDrones board use 6 & 5 */

#ifdef SERDB
  SoftwareSerial dbSerial(hRX,hTX);    /* (rx port,tx port) */
#endif

#define DPL dbSerial.println 
#define DPN dbSerial.print

int messageCounter;
bool mavlink_active;
BetterStream *mavlink_comm_0_port;

Preserved preserved_leds = { 0, 1, 2, 3 };

CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];


//Strip led2(13, 50, 2000);
Strobe strobe_led(50, 70, 50, 70, 50, 600);
Strobe strobe_led2(50, 70, 50, 600, 50, 70);
SysState __SysState;// = { 0, "", 0, 0, 0, 0 };
MAVLinkReader __MAVLinkReader(__SysState);

/* **********************************************/
/* ***************** SETUP() *******************/

void setup()
{
  FastLED.addLeds<LPD8806, FR, CLK, BRG>(leds[0], NUM_LEDS_PER_STRIP);
  //FastLED.addLeds<LPD8806, FL, CLK, BRG>(leds[1], NUM_LEDS_PER_STRIP);
  //FastLED.addLeds<LPD8806, RR, CLK, BRG>(leds[2], NUM_LEDS_PER_STRIP);
  //FastLED.addLeds<LPD8806, RL, CLK, BRG>(leds[3], NUM_LEDS_PER_STRIP);
  strobe_led.Attach(leds[0], 6);
  strobe_led2.Attach(leds[0], 7);

  Serial.begin(TELEMETRY_SPEED);          /* Initialize Serial port, speed */
  mavlink_comm_0_port = &Serial;          /* setup mavlink port */
  
  #ifdef SERDB
    dbSerial.begin(57600);
    DPL("Debug Serial ready... ");
    DPL("Output only please.  ");
  #endif
  
  pinMode(HEARTBEAT_LED_PIN, OUTPUT);
  
  for(int i = 0; i < 10 ; i++)
  {
    digitalWrite(HEARTBEAT_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);               // wait for a second
    digitalWrite(HEARTBEAT_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
    delay(20);
  }

  
  //fill_solid(leds[0], NUM_LEDS_PER_STRIP, CRGB::Blue);
  //FastLED.show();
}

void loop()
{
  strobe_led.Update();
  strobe_led2.Update();
  //led2.Update();
  __MAVLinkReader.Update();
}

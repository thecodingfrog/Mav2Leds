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
#include <FastSerial.h>
#include <math.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <GCS_MAVLink.h>
#include <SoftwareSerial.h>
#include <digitalWriteFast.h>    /* Direct portmanipulation library to replace digitalWrite. This is faster and smaller in code */
#include <FastLED.h>
#include "Mav2Leds.h"            /* Configurations */


// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "wiring.h"
#endif

/* *************************************************/
/* ***************** DEFINITIONS *******************/
#define TELEMETRY_SPEED  57600    /* MAVLink telemetry speed.*/
FastSerialPort0(Serial);           /* Our Uart port for Mavlink*/

#define DPL dbSerial.println 
#define DPN dbSerial.print

static uint8_t hRX=7;              /* software serial port for HoTT OR! Debug */
static uint8_t hTX=8;              /* if using the JDrones board use 6 & 5 */

#ifdef SERDB
  SoftwareSerial dbSerial(hRX,hTX);    /* (rx port,tx port) */
#endif

int messageCounter;
static bool mavlink_active;

/* **********************************************/
/* ***************** SETUP() *******************/
void setup()
{
  FastLED.addLeds<WS2812, FR>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, FL>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, RR>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, RL>(leds[2], NUM_LEDS_PER_STRIP);
  
  Serial.begin(TELEMETRY_SPEED);          /* Initialize Serial port, speed */
  mavlink_comm_0_port = &Serial;          /* setup mavlink port */
  
  #ifdef SERDB
    dbSerial.begin(57600);
    DPL("Debug Serial ready... ");
    DPL("Output only please.  ");
  #endif
  
  for(int i = 0; i < 10 ; i++)
  {
    digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);               // wait for a second
    digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
    delay(20);
  }
  
  RGBInitialize();
  
}
/* * * * * * * * * END of setup() * * * * * * * * */

  
/* * * * * * * * *  MAIN LOOP * * * * * * * * * * */
void loop()
{
  //Run "timer" every 120 miliseconds
  if(millis() > mavLinkTimer + 100)
  {
    mavLinkTimer = millis();
    timerEvent();
  }
  #ifdef SERDB
    DPL("loop");
  #endif
  read_mavlink();
  //RGBControl();
}


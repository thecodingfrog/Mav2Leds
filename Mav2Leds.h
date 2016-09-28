/* This file contains defines and declarations for the Mavlink protocol, the mavlink decoder and LED sequencer */
#ifndef MAV2LEDS_H
#define MAV2LEDS_H

#define FR  5    // Rear right    port D5
#define RR  9    // Front right   port D9
#define FL  6    // Front left    port D6
#define RL  10    // Rear left     port D10
#define CLK  11    // Clock port
#define HEARTBEAT_LED_PIN 13     // Heartbeat LED if any
#define NUM_STRIPS 3
#define NUM_LEDS_PER_STRIP 8

typedef struct
{
  uint8_t flight_mode;
  String flight_mode_str;
  uint8_t gps_fix_type;
  float battery;
  byte isArmed;
  uint8_t system_state;
} SysState;

typedef struct
{
  byte none;
  byte internal;
  byte external;
  byte both;
} Preserved;

typedef struct
{
  byte on;
  byte blink;
  byte chase;
  byte chaseback;
} LedMode;

#endif

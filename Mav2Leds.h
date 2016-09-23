/* This file contains defines and declarations for the Mavlink protocol, the mavlink decoder and LED sequencer */
#ifndef MAV2LEDS_H
#define MAV2LEDS_H

#define ledPin 13     // Heartbeat LED if any

typedef struct
{
  uint8_t flight_mode;
  String flight_mode_str;
  uint8_t fix_type;
  float vbat_A;
  byte isArmed;
  uint8_t sysstat;
} SysState;

typedef struct
{
  byte none;
  byte internal;
  byte external;
  byte both;
} Preserved;

#endif

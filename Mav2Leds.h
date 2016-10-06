/* This file contains defines and declarations for the Mavlink protocol, the mavlink decoder and LED sequencer */
#ifndef MAV2LEDS_H
#define MAV2LEDS_H

#define FR  9    // Rear right    port D9
#define RR  5    // Front right   port D5
#define FL  10    // Front left    port D10
#define RL  6    // Rear left     port D6
#define CLK  11    // Clock port

#define MAVLINK10     // Are we listening MAVLink 1.0 or 0.9   (0.9 is obsolete now)
#define HEARTBEAT     // HeartBeat signal
//#define SERDB         // Output debug mavlink information to SoftwareSerial, cannot be used with HoTT output at the same time!
//#define DEBUGLED 12   // HottLoop debugLed
#define HEARTBEAT_LED_PIN 13     // Heartbeat LED if any
#define NUM_STRIPS 3
#define NUM_LEDS_PER_STRIP 8
#define LOW_BATT_2 3.3

typedef struct
{
  uint8_t flight_mode;
  String flight_mode_str;
  uint8_t gps_fix_type;
  float battery;
  byte isArmed;
  uint8_t system_state;
  int num_cells;
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
  byte pulse;
  byte chain;
  byte bounce;
} LedMode;

typedef struct
{
  byte front;
  byte rear;
} Position;

#endif

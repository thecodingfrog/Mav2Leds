/* This file contains defines and declarations for the Mavlink protocol, the mavlink decoder and LED sequencer */

/* Some basic defaults */
#define EN  1         // Enable value
#define DI  0         // Disable value

static unsigned long mavLinkTimer = 0;

#define NUM_STRIPS 3
#define NUM_LEDS_PER_STRIP 8
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];

#define FR  9    // Rear right    port D5
#define RR  5    // Front right   port D9
#define FL  10    // Front left    port D6
#define RL  6    // Rear left     port D10
#define CLK  11    // Clock port
#define ledPin 13     // Heartbeat LED if any
/* direct port manupulation instead of digitalwrite. faster and smaller in code */

#define LOW_BATT   3.35      /* low battery per cell for HoTT display */
#define LOW_BATT_2 3.3       /* low battery per cell for LED warning */
#define throttleMin 1100     /* Throttle min position for flighttimer */

/* Bearing calculation, define MAV to Home bearing (view from MAV use in OSD) or Home to MAV bearing (view from Home) */
#define BEARING_MAV2HOME
//#define BEARING_HOME2MAV
#define useMavHome           /* Set when using a special message in aq_mavlink to transmitt AQ's home coordinates. */

/* AutoQuad Mavlink status definitions, for other Mavlink flightcontrollers you can 'map' the correct status in this list */

//#define DISARMED 64                        /* MAV_STATE_STANDBY */
//#define ARMED 192                          /* mavlinkData.mode | MAV_MODE_FLAG_SAFETY_ARMED */
//#define STABILIZED 16                      /* MAV_MODE_FLAG_STABILIZE_ENABLED */
//#define ALT_HOLD ARMED + STABILIZED        /* mavlinkData.mode | MAV_MODE_FLAG_STABILIZE_ENABLED */
//#define POS_HOLD ARMED + STABILIZED + 1    /* mavlinkData.mode | MAV_MODE_FLAG_STABILIZE_ENABLED | MAV_MODE_FLAG_CUSTOM_MODE_ENABLE */
//#define MANUAL_MODE ARMED                  /* mavlinkData.mode | MAV_MODE_FLAG_SAFETY_ARMED */
//#define MISION_MODE ARMED + STABILIZED + 4 /* mavlinkData.mode | MAV_MODE_FLAG_STABILIZE_ENABLED | MAV_MODE_FLAG_AUTO_ENABLED */
//#define DVH_MODE ARMED + STABILIZED + 8    /* mavlinkData.mode | MAV_MODE_FLAG_STABILIZE_ENABLED | MAV_MODE_FLAG_GUIDED_ENABLED */
//#define CARE_FREE 251

#define MOTORS_ARMED 128

#define DISARMED     MAV_MODE_MANUAL_DISARMED
#define ARMED        MAV_MODE_MANUAL_ARMED
#define MANUAL_MODE  MAV_MODE_MANUAL_ARMED
#define ALT_HOLD     MAV_MODE_MANUAL_ARMED + MAV_MODE_FLAG_STABILIZE_ENABLED
#define POS_HOLD     MAV_MODE_MANUAL_ARMED + MAV_MODE_FLAG_STABILIZE_ENABLED + MAV_MODE_FLAG_CUSTOM_MODE_ENABLED
#define MISION_MODE  MAV_MODE_MANUAL_ARMED + MAV_MODE_FLAG_STABILIZE_ENABLED + MAV_MODE_FLAG_AUTO_ENABLED
#define DVH_MODE     MAV_MODE_MANUAL_ARMED + MAV_MODE_FLAG_STABILIZE_ENABLED + MAV_MODE_FLAG_GUIDED_ENABLED
#define CARE_FREE    251 /* no definition in common.h yet */

#define MAV_STATE_STANDBY  3   /* System is grounded and on standby. It can be launched any time. | */
#define	MAV_STATE_ACTIVE   4   /* System is active and might be already airborne. Motors are engaged. | */
#define	MAV_STATE_CRITICAL 5   /* System is in a non-normal flight mode. It can however still navigate. | */

#define WAYPOINT  16
#define ORBIT     1
#define RTH       20
#define LAND      21
#define TAKEOFF   22 
/* * * * * * * * * * * * * * * * * * * * * */

/* Global variables */
byte  r, g, b, r2, g2, b2;
byte ioCounter = 0;          /* Loop counter */
byte variocounter3s = 0;     /* vario m/3s counter */
byte variocounter10s = 0;    /* vario m/10s counter */

/* AutoQuad received values */
static float    m2h_vbat_A = 0;                 // Battery A voltage in milivolt
static boolean  m2h_vbat_set = 0;               // first arrival correct voltage
static uint8_t  m2h_num_cells = 0;              // number of cells
static uint8_t  m2h_fuel_procent = 0;

static uint16_t m2h_mode = 0;                   // Status mode (manual,stabilized etc)
static uint8_t  m2h_nav_mode = 0;               // Navigation mode
static uint8_t  flight_mode = 0;
static uint8_t  m2h_sysstat = 0;                // system status (active,standby,critical)

static uint8_t  m2h_fix_type = 0;               // GPS lock 0-1=no fix, 2=2D, 3=3D

static uint16_t throttle = 0;                  // throttle value

static uint8_t mavlinkHB_char = 0;             // MavLink HeartBeat character

/* MAVLink session control */
static boolean  mavbeat = 0;
static uint8_t  apm_mav_type;
static uint8_t  apm_mav_system; 
static uint8_t  apm_mav_component;
static boolean  mavlink_request = 0;

/* General states */
byte flMode;                                  /* Our current flight mode as defined */
byte isArmed = 0;
byte isArmedOld = 0;
byte isActive;

String mode_str;

struct Preserved
{
  byte none;
  byte internal;
  byte external;
  byte both;
};



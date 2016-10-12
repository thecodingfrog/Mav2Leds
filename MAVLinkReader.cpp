#include <GCS_MAVLink.h>
#include "SysState.h"
#include "MAVLinkReader.h"

#define MOTORS_ARMED 128
#define DISARMED     MAV_MODE_MANUAL_DISARMED
#define ARMED        MAV_MODE_MANUAL_ARMED
#define MANUAL_MODE  MAV_MODE_MANUAL_ARMED
#define ALT_HOLD     MAV_MODE_MANUAL_ARMED + MAV_MODE_FLAG_STABILIZE_ENABLED
#define POS_HOLD     MAV_MODE_MANUAL_ARMED + MAV_MODE_FLAG_STABILIZE_ENABLED + MAV_MODE_FLAG_CUSTOM_MODE_ENABLED
#define MISION_MODE  MAV_MODE_MANUAL_ARMED + MAV_MODE_FLAG_STABILIZE_ENABLED + MAV_MODE_FLAG_AUTO_ENABLED
#define DVH_MODE     MAV_MODE_MANUAL_ARMED + MAV_MODE_FLAG_STABILIZE_ENABLED + MAV_MODE_FLAG_GUIDED_ENABLED
#define CARE_FREE 251 /* no definition in common.h yet */
#define MAV_STATE_STANDBY 3 /* System is grounded and on standby. It can be launched any time. | */
#define MAV_STATE_ACTIVE 4   /* System is active and might be already airborne. Motors are engaged. | */
#define MAV_STATE_CRITICAL 5   /* System is in a non-normal flight mode. It can however still navigate. | */
#define WAYPOINT 16
#define ORBIT 1
#define RTH 20
#define LAND 21
#define TAKEOFF 22

MAVLinkReader::MAVLinkReader()
{
  previousMillis = 0;
}

void MAVLinkReader::Update()
{
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();
   
  if(currentMillis - previousMillis >= OnTime)
  {
    previousMillis = currentMillis;
    #ifdef SERDB
      DPL("MAVLinkReader::Update()");
    #endif

    /* this event is called @ 10Hz */
    __ioCounter ++ ;     /* update counter to use in all blink loops */
    if (__ioCounter > 10) __ioCounter = 0;

    /* 10Hz calls */
    /* do the LED stuff 
     * RGB controller LEDstrip or directly attached LEDs */
    HeartBeat();
  
    /* end 10Hz calls */
  
    /* 1Hz calls */
    if (__ioCounter == 1)
    {
      /* 1Hz routines between this line */
  
      CheckBattery();
  
      /* and this line */
    }
  }
}



void MAVLinkReader::HeartBeat()
{
  #ifdef HEARTBEAT
    if (__mavlink_active) digitalWrite(HEARTBEAT_LED_PIN, __ioCounter == 1 ? HIGH : LOW);  /* only HB if mavlink is active */
    __messageCounter++;
  #endif

  /* check if Mavlink is lost */
  if(__messageCounter >= 50 && __mavlink_active)
  {
    #ifdef SERDB  
      DPL("We lost MAVLink");
    #endif
    __mavlink_active = 0;
    __messageCounter = 0;
    mavlink_request = 1;
  }

  if(__messageCounter > 500) __messageCounter = 0;  /* to prevent overflow */
  #ifdef SERDB  
    if(__messageCounter == 500){
      DPL("(Still) no mavlink ??");
    }
  #endif
}

void MAVLinkReader::CheckBattery()
{
  
  if ((!__vbat_set) && (__vbat_A > 100))
  {
    if (__vbat_A >= 100)
    {
      __vbat_set = 1;
      if ((__vbat_A > 100) && (__vbat_A <127)) __num_cells = 3;
      if ((__vbat_A > 130) && (__vbat_A <169)) __num_cells = 4;
      if ((__vbat_A > 170) && (__vbat_A <220)) __num_cells = 5;
      __obj.num_cells = __num_cells;
    }
  }
  if ((__vbat_set) && (__vbat_A > 100))
  {                                /* fuel % gauge in 25% increments */
    if ((__vbat_A/__num_cells) < 33.5) __fuel_procent = 0;
    if ( ((__vbat_A/__num_cells) >= 33.5) && ((__vbat_A/__num_cells) < 37.5) ) __fuel_procent = 24;  /* one less or else the next segment is false */
    if ( ((__vbat_A/__num_cells) >= 37.5) && ((__vbat_A/__num_cells) < 38.4) ) __fuel_procent = 49;  /* one less or else the next segment is false */
    if ( ((__vbat_A/__num_cells) >= 38.4) && ((__vbat_A/__num_cells) < 39.5) ) __fuel_procent = 74;    
    if ((__vbat_A/__num_cells) >= 39.5) __fuel_procent = 100;  
  }
}

SysState MAVLinkReader::Read()
{
  mavlink_message_t msg;
  mavlink_status_t status;

  /*for(int n = 0; n < 3; n++)
  {
      request_mavlink_rates();//Three times to certify it will be readed
      delay(50);
  }*/
  
  // grabing data 
  while(Serial.available() > 0) { 
    uint8_t c = Serial.read();

    // trying to grab msg  
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
       __messageCounter = 0; 
       __mavlink_active = 1;
       switch(msg.msgid)           /* Handle msg */
    {
/**
* @param type Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
* @param autopilot Autopilot type / class. defined in MAV_AUTOPILOT ENUM
* @param base_mode System mode bitfield, see MAV_MODE_FLAGS ENUM in mavlink/include/mavlink_types.h
* @param custom_mode A bitfield for use for autopilot-specific flags.  - AutoQuad nav mode
* @param system_status System status flag, see MAV_STATE ENUM
*/
        case MAVLINK_MSG_ID_HEARTBEAT:
          {
            #ifdef SERDB
              DPL("MAVlink HeartBeat");
            #endif
            
            mavbeat = 1;
            apm_mav_system    = msg.sysid;
            apm_mav_component = msg.compid;
            apm_mav_type      = mavlink_msg_heartbeat_get_type(&msg);

            __mode = mavlink_msg_heartbeat_get_base_mode(&msg);
            __nav_mode = mavlink_msg_heartbeat_get_custom_mode(&msg);
            __flight_mode = (uint8_t)mavlink_msg_heartbeat_get_custom_mode(&msg);
            __sys_state = mavlink_msg_heartbeat_get_system_status(&msg);
            
            CheckFlightMode();
            //if (__mode == D__isArmed) __isArmed=0;
            //if ((__mode >= ARMED) || (__sys_state != 3)) __isArmed=1;
            if(isBit(mavlink_msg_heartbeat_get_base_mode(&msg),MOTORS_ARMED))
            {
              if(__isArmedOld == 0)
              {
                  CheckFlightMode();
                  __isArmedOld = 1;
              }    
              __isArmed = 1;  
            }
            else
            {
              __isArmed = 0;
              __isArmedOld = 0;
            }
            __obj.is_armed = __isArmed;
            __obj.severity = (__isArmed) ? 6 : __severity;
            __obj.system_state = __sys_state;

#ifdef SERDB            
              dbSerial.print("MAV: ");
              dbSerial.print((mavlink_msg_heartbeat_get_base_mode(&msg),DEC));
              dbSerial.println();
              dbSerial.print("Mode: ");
              dbSerial.print(__mode);
              dbSerial.println();
              dbSerial.print("flMode: ");
              dbSerial.print(__flMode);
              dbSerial.println();
              dbSerial.print("Nav mode: ");
              dbSerial.print(__nav_mode);
              dbSerial.println();
              dbSerial.print("Flight mode: ");
              dbSerial.print(__mode_str);
              dbSerial.println();
              dbSerial.print("SysStat: ");
              dbSerial.print(__sys_state);
              dbSerial.println();
              dbSerial.print("FIX: ");
              dbSerial.print(__gps_fix_type);
              dbSerial.println();
              dbSerial.print("BatVolt: ");
              dbSerial.print(__vbat_A/1E1);
              dbSerial.println();
              dbSerial.print("Armed: ");
              dbSerial.print(__isArmed);
                              
              dbSerial.println();
#endif 
          }
          break;
          
          case MAVLINK_MSG_ID_SYS_STATUS:
          { 
            __vbat_A = (mavlink_msg_sys_status_get_voltage_battery(&msg) / 100.0f);    // It will arive in mV, but Hott uses V * 100
            //m2l_battery_remaining_A = mavlink_msg_sys_status_get_battery_remaining(&msg);    // not used in HoTT 
            __obj.battery = __vbat_A;
          }
          break;
/*
 * @param fix_type 0-1: no fix, 2: 2D fix, 3: 3D fix. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.
 * @param lat Latitude in 1E7 degrees
 * @param lon Longitude in 1E7 degrees
 * @param alt Altitude in 1E3 meters (millimeters) above MSL
 * @param eph GPS HDOP horizontal dilution of position in cm (m*100). If unknown, set to: 65535
 * @param epv GPS VDOP horizontal dilution of position in cm (m*100). If unknown, set to: 65535
 * @param vel GPS ground speed (m/s * 100). If unknown, set to: 65535
 * @param cog Course over ground (NOT heading, but direction of movement) in degrees * 100, 0.0..359.99 degrees. If unknown, set to: 65535
 * @param satellites_visible Number of satellites visible. If unknown, set to 255
 */
        case MAVLINK_MSG_ID_GPS_RAW_INT:    // is send by AutoQad
          {      
            __gps_fix_type = mavlink_msg_gps_raw_int_get_fix_type(&msg);
            __obj.gps_fix_type = __gps_fix_type;
          }
          break;
/**
 * @return RC channel 1 value, in microseconds
 */
        case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
        {
           __throttle = mavlink_msg_rc_channels_raw_get_chan1_raw(&msg);
        }
        break;
        
        case MAVLINK_MSG_ID_STATUSTEXT:
          {
            //colorBlink(CRGB::Red, -1, -1, 50, 3, CRGB::Red, preserved_leds.none);
            __severity = mavlink_msg_statustext_get_severity(&msg);
            //mavlink_msg_statustext_get_text(&msg, severity_text);
            //checks_ok = (String(severity_text).indexOf("PreArm:") >= 0) ? 0:1;
            __obj.severity = __severity;
           #ifdef SERDB
             DPL(mavlink_msg_statustext_get_severity(&msg));
           #endif
          }
          break;
// * * * end messages * * *       
       default:
       //Do nothing
       break;
      }
    }
    //delayMicroseconds(138); //TODO
    //next one
  }
  // Update global packet drops counter
  packet_drops += status.packet_rx_drop_count;
  parse_error += status.parse_error;

   return __obj;
}

void MAVLinkReader::CheckFlightMode()
{
  //if (displayVersionDone)
  //{
    if (__flight_mode == 0) __mode_str = "stab"; //Stabilize
    else if (__flight_mode == 1) __mode_str = "acro"; //Acrobatic
    else if (__flight_mode == 2) __mode_str = "alth"; //Alt Hold
    else if (__flight_mode == 3) __mode_str = "auto"; //Auto
    else if (__flight_mode == 4) __mode_str = "guid"; //Guided
    else if (__flight_mode == 5) __mode_str = "loit"; //Loiter
    else if (__flight_mode == 6) __mode_str = "rtl"; //Return to Launch
    else if (__flight_mode == 7) __mode_str = "circ"; //Circle
    //else if (__flight_mode == 8) __mode_str = "posi"; //Position Hold (Old)
    else if (__flight_mode == 9) __mode_str = "land"; //Land
    else if (__flight_mode == 10) __mode_str = "oflo"; //OF_Loiter
    else if (__flight_mode == 11) __mode_str = "drif"; //Drift
    else if (__flight_mode == 13) __mode_str = "sprt"; //Sport
    else if (__flight_mode == 14) __mode_str = "flip"; //Flip
    else if (__flight_mode == 15) __mode_str = "tune"; //Tune
    else if (__flight_mode == 16) __mode_str = "phld"; //Position Hold (Earlier called Hybrid)
    
    if(__mode == 0)          __flMode = 0;    // d__isArmed - no data
    if(__mode == DISARMED)   __flMode = 1;    // d__isArmed
    if(__mode == ARMED)      __flMode = 2;    // Manual + stab
    if(__mode == ALT_HOLD)   __flMode = 3;    // Alt Hold
    if(__mode == POS_HOLD)   __flMode = 4;    // Position Hold
    if(__mode == DVH_MODE)   __flMode = 6;    // Stabilized DVH mode
    if(__mode == CARE_FREE)  __flMode = 7;    // CareFree mode
    if(__sys_state == MAV_STATE_CRITICAL) __flMode = 11;   // Signal lost
    if((__mode == MISION_MODE) && ((__nav_mode == WAYPOINT) || (__nav_mode == TAKEOFF))) __flMode = 5;    // Mission mode, waypoint or takeoff
    if((__mode == MISION_MODE) && (__nav_mode == RTH))   __flMode = 8;    // Return to Home
    if((__mode == MISION_MODE) && (__nav_mode == ORBIT)) __flMode = 9;    // Circle orbit
    if((__mode == MISION_MODE) && (__nav_mode == LAND))  __flMode = 10;   // Land

    __obj.flight_mode = __flight_mode;
    __obj.flight_mode_str = __mode_str;
  //}
}

// Checking if BIT is active in PARAM, return true if it is, false if not
byte MAVLinkReader::isBit(byte param, byte bitfield)
{
  if((param & bitfield) == bitfield) return 1;
  else return 0;  
}


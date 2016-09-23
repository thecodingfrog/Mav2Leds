#include <GCS_MAVLink.h>
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

MAVLinkReader::MAVLinkReader(SysState& __SysState)
{
  //__SysState = obj;
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
    ioCounter ++ ;     /* update counter to use in all blink loops */
    if (ioCounter > 10) ioCounter = 0;

    /* 10Hz calls */
    /* do the LED stuff 
     * RGB controller LEDstrip or directly attached LEDs */
    HeartBeat();
    //RGBControl();
  
    /* end 10Hz calls */
  
    /* 1Hz calls */
    if (ioCounter == 1)
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
    if (mavlink_active) digitalWriteFast(ledPin, ioCounter == 1 ? HIGH : LOW);  /* only HB is mavlink is active */
    messageCounter++;
    /* Mavlink on HoTT display heartbeat */
    if ((ioCounter == 1) && (mavlink_active))
    {
      mavlinkHB_char = char('+');      /* '+' to indicate mavlink received heart beat */
    }
    else
    {
      mavlinkHB_char = 0;
    }
  #endif

  /* check if Mavlink is lost */
  if(messageCounter >= 50 && mavlink_active)
  {
    #ifdef SERDB  
      DPL("We lost MAVLink");
    #endif
    mavlink_active = 0;
    messageCounter = 0;
    mavlink_request = 1;
  }

  if(messageCounter > 500) messageCounter = 0;  /* to prevent overflow */
  #ifdef SERDB  
    if(messageCounter == 500){
      DPL("(Still) no mavlink ??");
    }
  #endif
}

void MAVLinkReader::CheckBattery()
{
  
  if ((!m2l_vbat_set) && (m2l_vbat_A > 100))
  {
    if (m2l_vbat_A >= 100)
    {
      m2l_vbat_set = 1;
      if ((m2l_vbat_A > 100) && (m2l_vbat_A <127)) m2l_num_cells = 3;
      if ((m2l_vbat_A > 130) && (m2l_vbat_A <169)) m2l_num_cells = 4;
      if ((m2l_vbat_A > 170) && (m2l_vbat_A <220)) m2l_num_cells = 5;
    }
  }
  if ((m2l_vbat_set) && (m2l_vbat_A > 100))
  {                                /* fuel % gauge in 25% increments */
    if ((m2l_vbat_A/m2l_num_cells) < 33.5) m2l_fuel_procent = 0;
    if ( ((m2l_vbat_A/m2l_num_cells) >= 33.5) && ((m2l_vbat_A/m2l_num_cells) < 37.5) ) m2l_fuel_procent = 24;  /* one less or else the next segment is false */
    if ( ((m2l_vbat_A/m2l_num_cells) >= 37.5) && ((m2l_vbat_A/m2l_num_cells) < 38.4) ) m2l_fuel_procent = 49;  /* one less or else the next segment is false */
    if ( ((m2l_vbat_A/m2l_num_cells) >= 38.4) && ((m2l_vbat_A/m2l_num_cells) < 39.5) ) m2l_fuel_procent = 74;    
    if ((m2l_vbat_A/m2l_num_cells) >= 39.5) m2l_fuel_procent = 100;  
  }
}

void MAVLinkReader::Read()
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
       messageCounter = 0; 
       mavlink_active = 1;
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

            m2l_mode = mavlink_msg_heartbeat_get_base_mode(&msg);
            m2l_nav_mode = mavlink_msg_heartbeat_get_custom_mode(&msg);
            flight_mode = (uint8_t)mavlink_msg_heartbeat_get_custom_mode(&msg);
            m2l_sysstat = mavlink_msg_heartbeat_get_system_status(&msg);
            
            CheckFlightMode();
            //if (m2l_mode == DISARMED) isArmed=0;
            //if ((m2l_mode >= ARMED) || (m2l_sysstat != 3)) isArmed=1;
            if(isBit(mavlink_msg_heartbeat_get_base_mode(&msg),MOTORS_ARMED))
            {
              if(isArmedOld == 0)
              {
                  CheckFlightMode();
                  isArmedOld = 1;
              }    
              isArmed = 1;  
            }
            else
            {
              isArmed = 0;
              isArmedOld = 0;
            }           

#ifdef SERDB            
              dbSerial.print("MAV: ");
              dbSerial.print((mavlink_msg_heartbeat_get_base_mode(&msg),DEC));
              dbSerial.println();
              dbSerial.print("Mode: ");
              dbSerial.print(m2l_mode);
              dbSerial.println();
              dbSerial.print("flMode: ");
              dbSerial.print(flMode);
              dbSerial.println();
              dbSerial.print("Nav mode: ");
              dbSerial.print(m2l_nav_mode);
              dbSerial.println();
              dbSerial.print("Flight mode: ");
              dbSerial.print(mode_str);
              dbSerial.println();
              dbSerial.print("SysStat: ");
              dbSerial.print(m2l_sysstat);
              dbSerial.println();
              dbSerial.print("FIX: ");
              dbSerial.print(m2l_fix_type);
              dbSerial.println();
              dbSerial.print("BatVolt: ");
              dbSerial.print(m2l_vbat_A/1E1);
              dbSerial.println();
              dbSerial.print("Armed: ");
              dbSerial.print(isArmed);
                              
              dbSerial.println();
#endif 
          }
          break;
          
          case MAVLINK_MSG_ID_SYS_STATUS:
          { 
            m2l_vbat_A = (mavlink_msg_sys_status_get_voltage_battery(&msg) / 100.0f);    // It will arive in mV, but Hott uses V * 100
            //m2l_battery_remaining_A = mavlink_msg_sys_status_get_battery_remaining(&msg);    // not used in HoTT 
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
            m2l_fix_type = mavlink_msg_gps_raw_int_get_fix_type(&msg);            
          }
          break;
/**
 * @return RC channel 1 value, in microseconds
 */
        case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
        {
           throttle = mavlink_msg_rc_channels_raw_get_chan1_raw(&msg);
        }
        break;
        
        case MAVLINK_MSG_ID_STATUSTEXT:
          {
            //colorBlink(CRGB::Red, -1, -1, 50, 3, CRGB::Red, preserved_leds.none);
            //severity = mavlink_msg_statustext_get_severity(&msg);
            //mavlink_msg_statustext_get_text(&msg, severity_text);
            //checks_ok = (String(severity_text).indexOf("PreArm:") >= 0) ? 0:1;
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
}

void MAVLinkReader::CheckFlightMode()
{
  //if (displayVersionDone)
  //{
    if (flight_mode == 0) mode_str = "stab"; //Stabilize
    else if (flight_mode == 1) mode_str = "acro"; //Acrobatic
    else if (flight_mode == 2) mode_str = "alth"; //Alt Hold
    else if (flight_mode == 3) mode_str = "auto"; //Auto
    else if (flight_mode == 4) mode_str = "guid"; //Guided
    else if (flight_mode == 5) mode_str = "loit"; //Loiter
    else if (flight_mode == 6) mode_str = "rtl"; //Return to Launch
    else if (flight_mode == 7) mode_str = "circ"; //Circle
    //else if (flight_mode == 8) mode_str = "posi"; //Position Hold (Old)
    else if (flight_mode == 9) mode_str = "land"; //Land
    else if (flight_mode == 10) mode_str = "oflo"; //OF_Loiter
    else if (flight_mode == 11) mode_str = "drif"; //Drift
    else if (flight_mode == 13) mode_str = "sprt"; //Sport
    else if (flight_mode == 14) mode_str = "flip"; //Flip
    else if (flight_mode == 15) mode_str = "tune"; //Tune
    else if (flight_mode == 16) mode_str = "phld"; //Position Hold (Earlier called Hybrid)
    
    if(m2l_mode == 0)          flMode = 0;    // disarmed - no data
    if(m2l_mode == DISARMED)   flMode = 1;    // disarmed
    if(m2l_mode == ARMED)      flMode = 2;    // Manual + stab
    if(m2l_mode == ALT_HOLD)   flMode = 3;    // Alt Hold
    if(m2l_mode == POS_HOLD)   flMode = 4;    // Position Hold
    if(m2l_mode == DVH_MODE)   flMode = 6;    // Stabilized DVH mode
    if(m2l_mode == CARE_FREE)  flMode = 7;    // CareFree mode
    if(m2l_sysstat == MAV_STATE_CRITICAL) flMode = 11;   // Signal lost
    if((m2l_mode == MISION_MODE) && ((m2l_nav_mode == WAYPOINT) || (m2l_nav_mode == TAKEOFF))) flMode = 5;    // Mission mode, waypoint or takeoff
    if((m2l_mode == MISION_MODE) && (m2l_nav_mode == RTH))   flMode = 8;    // Return to Home
    if((m2l_mode == MISION_MODE) && (m2l_nav_mode == ORBIT)) flMode = 9;    // Circle orbit
    if((m2l_mode == MISION_MODE) && (m2l_nav_mode == LAND))  flMode = 10;   // Land
  //}
}

// Checking if BIT is active in PARAM, return true if it is, false if not
byte MAVLinkReader::isBit(byte param, byte bitfield)
{
  if((param & bitfield) == bitfield) return 1;
  else return 0;  
}


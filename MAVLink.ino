#define MAVLINK_COMM_NUM_BUFFERS 1
#define MAVLINK_USE_CONVENIENCE_FUNCTIONS

// this code was moved from libraries/GCS_MAVLink to allow compile
// time selection of MAVLink 1.0
//BetterStream	*mavlink_comm_0_port;
//BetterStream	*mavlink_comm_1_port;

//mavlink_system_t mavlink_system = {12,1,0,0};

#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"
#include "../GCS_MAVLink/include/mavlink/v1.0/ardupilotmega/mavlink.h" 

static int packet_drops = 0;
static int parse_error = 0;

void request_mavlink_rates()
{
  #ifdef SERDB
    DPL("Requesting rates");
  #endif
  const int maxStreams = 6;
  const uint8_t MAVStreams[maxStreams] = {MAV_DATA_STREAM_RAW_SENSORS,
                                          MAV_DATA_STREAM_RC_CHANNELS,
					  MAV_DATA_STREAM_POSITION,
                                          MAV_DATA_STREAM_ALL, 
                                          MAV_DATA_STREAM_EXTRA1,
                                          MAV_DATA_STREAM_RAW_CONTROLLER};
                                          
  const uint16_t MAVRates[maxStreams] = {0x02, 0x02, 0x05, 0x05, 0x02, 0x02};

  for (int i=0; i < maxStreams; i++) {
    	  mavlink_msg_request_data_stream_send(MAVLINK_COMM_0,
					       apm_mav_system, apm_mav_component,
					       MAVStreams[i], MAVRates[i], 1);
  }
}

void read_mavlink()
{
  Serial.println("read_mavlink()");
  mavlink_message_t msg; 
  mavlink_status_t status;
  
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

            m2h_mode = mavlink_msg_heartbeat_get_base_mode(&msg);
            m2h_nav_mode = mavlink_msg_heartbeat_get_custom_mode(&msg);
            flight_mode = (uint8_t)mavlink_msg_heartbeat_get_custom_mode(&msg);
            m2h_sysstat = mavlink_msg_heartbeat_get_system_status(&msg);
            
            CheckFlightMode();
            //if (m2h_mode == DISARMED) isArmed=0;
            //if ((m2h_mode >= ARMED) || (m2h_sysstat != 3)) isArmed=1;
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
              dbSerial.print(m2h_mode);
              dbSerial.println();
              dbSerial.print("flMode: ");
              dbSerial.print(flMode);
              dbSerial.println();
              dbSerial.print("Nav mode: ");
              dbSerial.print(m2h_nav_mode);
              dbSerial.println();
              dbSerial.print("Flight mode: ");
              dbSerial.print(mode_str);
              dbSerial.println();
              dbSerial.print("SysStat: ");
              dbSerial.print(m2h_sysstat);
              dbSerial.println();
              dbSerial.print("FIX: ");
              dbSerial.print(m2h_fix_type);
              dbSerial.println();
              dbSerial.print("BatVolt: ");
              dbSerial.print(m2h_vbat_A/1E1);
              dbSerial.println();
              dbSerial.print("Armed: ");
              dbSerial.print(isArmed);
                              
              dbSerial.println();
#endif 
          }
          break;
          
          case MAVLINK_MSG_ID_SYS_STATUS:
          { 
            m2h_vbat_A = (mavlink_msg_sys_status_get_voltage_battery(&msg) / 100.0f);    // It will arive in mV, but Hott uses V * 100
            //m2h_battery_remaining_A = mavlink_msg_sys_status_get_battery_remaining(&msg);    // not used in HoTT 
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
            m2h_fix_type = mavlink_msg_gps_raw_int_get_fix_type(&msg);            
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
    delayMicroseconds(138);
    //next one
  }
  // Update global packet drops counter
  packet_drops += status.packet_rx_drop_count;
  parse_error += status.parse_error;  
}

void timerEvent()
{
  Serial.println("timerEvent()");
  #ifdef SERDB
    DPL("timerEvent()");
  #endif
  
  /* this event is called @ 10Hz */
  ioCounter ++ ;     /* update counter to use in all blink loops */
  if (ioCounter > 10) ioCounter = 0;

  /* 10Hz calls */
  /* do the LED stuff 
   * RGB controller LEDstrip or directly attached LEDs */
  //RGBControl();

  heartBeat();

  /* end 10Hz calls */

  /* 1Hz calls */
  if (ioCounter == 1)
  {
    /* 1Hz routines between this line */

    checkBattery();

    /* and this line */
  }
  /* end 1Hz calls */
}

void CheckFlightMode()
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
    
    if(m2h_mode == 0)          flMode = 0;    // disarmed - no data
    if(m2h_mode == DISARMED)   flMode = 1;    // disarmed
    if(m2h_mode == ARMED)      flMode = 2;    // Manual + stab
    if(m2h_mode == ALT_HOLD)   flMode = 3;    // Alt Hold
    if(m2h_mode == POS_HOLD)   flMode = 4;    // Position Hold
    if(m2h_mode == DVH_MODE)   flMode = 6;    // Stabilized DVH mode
    if(m2h_mode == CARE_FREE)  flMode = 7;    // CareFree mode
    if(m2h_sysstat == MAV_STATE_CRITICAL) flMode = 11;   // Signal lost
    if((m2h_mode == MISION_MODE) && ((m2h_nav_mode == WAYPOINT) || (m2h_nav_mode == TAKEOFF))) flMode = 5;    // Mission mode, waypoint or takeoff
    if((m2h_mode == MISION_MODE) && (m2h_nav_mode == RTH))   flMode = 8;    // Return to Home
    if((m2h_mode == MISION_MODE) && (m2h_nav_mode == ORBIT)) flMode = 9;    // Circle orbit
    if((m2h_mode == MISION_MODE) && (m2h_nav_mode == LAND))  flMode = 10;   // Land
  //}
}


// Checking if BIT is active in PARAM, return true if it is, false if not
byte isBit(byte param, byte bitfield)
{
  if((param & bitfield) == bitfield) return 1;
  else return 0;  
}

void heartBeat()
{
  #ifdef HEARTBEAT
    if (mavlink_active) digitalWriteFast(ledPin, ioCounter == 1 ? HIGH : LOW);  /* only HB is mavlink is active */
    messageCounter++;
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

static void checkBattery()
{
  if ((!m2h_vbat_set) && (m2h_vbat_A > 100))
  {
    if (m2h_vbat_A >= 100)
    {
      m2h_vbat_set = 1;
      if ((m2h_vbat_A > 100) && (m2h_vbat_A <127)) m2h_num_cells = 3;
      if ((m2h_vbat_A > 130) && (m2h_vbat_A <169)) m2h_num_cells = 4;
      if ((m2h_vbat_A > 170) && (m2h_vbat_A <220)) m2h_num_cells = 5;
    }
  }
  if ((m2h_vbat_set) && (m2h_vbat_A > 100))
  {                                /* fuel % gauge in 25% increments */
    if ((m2h_vbat_A/m2h_num_cells) < 33.5) m2h_fuel_procent = 0;
    if ( ((m2h_vbat_A/m2h_num_cells) >= 33.5) && ((m2h_vbat_A/m2h_num_cells) < 37.5) ) m2h_fuel_procent = 24;  /* one less or else the next segment is false */
    if ( ((m2h_vbat_A/m2h_num_cells) >= 37.5) && ((m2h_vbat_A/m2h_num_cells) < 38.4) ) m2h_fuel_procent = 49;  /* one less or else the next segment is false */
    if ( ((m2h_vbat_A/m2h_num_cells) >= 38.4) && ((m2h_vbat_A/m2h_num_cells) < 39.5) ) m2h_fuel_procent = 74;    
    if ((m2h_vbat_A/m2h_num_cells) >= 39.5) m2h_fuel_procent = 100;  
  }
}


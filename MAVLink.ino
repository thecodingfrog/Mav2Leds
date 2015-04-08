#define MAVLINK_COMM_NUM_BUFFERS 1
#define MAVLINK_USE_CONVENIENCE_FUNCTIONS

// this code was moved from libraries/GCS_MAVLink to allow compile
// time selection of MAVLink 1.0
BetterStream	*mavlink_comm_0_port;
BetterStream	*mavlink_comm_1_port;

mavlink_system_t mavlink_system = {12,1,0,0};

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

void read_mavlink(){
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
              dbSerial.print("Sats: ");
              dbSerial.print(m2h_satellites_visible);
              dbSerial.println();
              dbSerial.print("BatVolt: ");
              dbSerial.print(m2h_vbat_A/1E1);
              dbSerial.println();

              dbSerial.print("Latitude: ");
              dbSerial.print(double(m2h_gps_lat/1E7) );
              dbSerial.print("  Longitude: ");
              dbSerial.print(double(m2h_gps_lon/1E7) );
              dbSerial.print("  Altitude: ");
              dbSerial.print(m2h_gps_alt/1E3);

              dbSerial.print("  Roll: ");
              dbSerial.print(m2h_roll);
              dbSerial.print("  Pitch: ");
              dbSerial.print(m2h_pitch);
              dbSerial.print("  Heading: ");
              dbSerial.print( ((uint16_t)(m2h_yaw*100)/100) );
              

              dbSerial.print("  Temp: ");
              dbSerial.print(m2h_temp/1E2);
              
              dbSerial.println();

              dbSerial.print("Home Latitude: ");
              dbSerial.print(m2h_gps_lat_home/1E7);
              dbSerial.print("  Home Longitude: ");
              dbSerial.print(m2h_gps_lon_home/1E7);
              dbSerial.print("  Ceiling: ");
              dbSerial.print(m2h_ceiling/1E3);
              dbSerial.print("  Distance home: ");
              dbSerial.print(home_distance_calc);
              dbSerial.print("  Armed: ");
              dbSerial.print(isArmed);
                              
              dbSerial.println();
#endif 
          }
          break;
          
          case MAVLINK_MSG_ID_SYS_STATUS:
          { 
            // AQ sends   (0,0,0,idle %, batt, batt remaining, drops, 0,0,0,0) 
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
            m2h_gps_lon = mavlink_msg_gps_raw_int_get_lon(&msg);    // latitude
            m2h_gps_lat = mavlink_msg_gps_raw_int_get_lat(&msg);    // longitude
            m2h_gps_alt = mavlink_msg_gps_raw_int_get_alt(&msg);    // gps altitude
            m2h_gps_vel = mavlink_msg_gps_raw_int_get_vel(&msg);    // speed in m/s
            m2h_gps_cog = mavlink_msg_gps_raw_int_get_cog(&msg);    // direction of movement
            m2h_satellites_visible = mavlink_msg_gps_raw_int_get_satellites_visible(&msg);            
          }
          break;

        case MAVLINK_MSG_ID_ATTITUDE:
          {
            m2h_pitch = ToDeg(mavlink_msg_attitude_get_pitch(&msg));
            m2h_roll = ToDeg(mavlink_msg_attitude_get_roll(&msg));
            m2h_yaw = ToDeg(mavlink_msg_attitude_get_yaw(&msg));
          }
          break;
/**
 * @brief Pack a scaled_pressure message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message was sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_boot_ms Timestamp (microseconds since UNIX epoch or microseconds since system boot)  -- AutoQuad
 * @param press_abs Absolute pressure (hectopascal)                             -- AutoQuad AQ_PRESSURE*0.01f
 * @param press_diff Differential pressure 1 (hectopascal)                      -- AutoQuad 0.0f
 * @param temperature Temperature measurement (0.01 degrees celsius)            -- AutoQuad
 * @return length of the message in bytes (excluding serial stream start sign)
 */          
        case MAVLINK_MSG_ID_SCALED_PRESSURE:
        {
           m2h_abs = mavlink_msg_scaled_pressure_get_press_abs(&msg);       // absolute presure          
           m2h_temp = mavlink_msg_scaled_pressure_get_temperature(&msg);    // internal pressure sensor temp
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
        
/**
* @param target_system System ID
 * @param latitude global position * 1E7
 * @param longitude global position * 1E7
 * @param altitude global position * 1000
 */
        case MAVLINK_MSG_ID_GLOBAL_POSITION_SETPOINT_INT:
          {
           m2h_gps_lat_home = mavlink_msg_set_global_position_setpoint_int_get_latitude(&msg);
           m2h_gps_lon_home = mavlink_msg_set_global_position_setpoint_int_get_longitude(&msg);
           m2h_ceiling = mavlink_msg_set_global_position_setpoint_int_get_altitude(&msg);
           if ( (m2h_gps_lat_home !=0) &&  (m2h_gps_lon_home !=0)) m2h_got_home = 1;
          }
          break;

          /* I know, totaly misused this message with other variables but the Android app doesn't take any other 'alien' message */
        case MAVLINK_MSG_ID_VFR_HUD:
          { 
           m2h_gps_lat_home = mavlink_msg_vfr_hud_get_airspeed(&msg);
           m2h_gps_lon_home = mavlink_msg_vfr_hud_get_groundspeed(&msg);
           m2h_ceiling = mavlink_msg_vfr_hud_get_alt(&msg);
           if ( (m2h_gps_lat_home !=0) &&  (m2h_gps_lon_home !=0)) m2h_got_home = 1;
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

/* This file contains conversion & calculation functions for values used for the HoTT protocol */

void timerEvent()
{  /* this event is called @ 10Hz */
  ioCounter ++ ;     /* update counter to use in all blink loops */
  if (ioCounter >10) ioCounter =0;

  /* 10Hz calls */
  /* do the LED stuff 
   * RGB controller LEDstrip or directly attached LEDs */
  RGBControl();

  heartBeat();

  /* end 10Hz calls */

  /* 1Hz calls */
  if (ioCounter == 1)
  {
    /* 1Hz routines between this line */

    checkBattery();
    updateFlightTimer();
    checkAltitude();
    checkHomeDistance();
    varioCalc();

    /* and this line */
  }
  /* end 1Hz calls */

  #ifndef useMavHome
    /* check if Home coordinates are set every 200 cycles = 20 sec*/
    counter ++;
    if ((counter > 200)&& (!m2h_got_home))
    {
      checkHomeSet();
      counter =0 ;
    }
  #endif

  /* Display version on display for 5 sec*/
  if (!displayVersionDone)
  {
    flMode = 13;
    if (millis() - p_cMillis >5000) displayVersionDone = 1;
  }
  /* end timer event */
}

void CheckFlightMode()
{
  if (displayVersionDone)
  {
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
  }
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
  if(messageCounter >= 50 && mavlink_active) {
  #ifdef SERDB  
      DPL("We lost MAVLink");
  #endif
    mavlink_active = 0;
    messageCounter = 0;
    mavlink_request = 1;
  }

  if(messageCounter >500) messageCounter = 0;  /* to prevent overflow */
  #ifdef SERDB  
    if(messageCounter == 500){
      DPL("(Still) no mavlink ??");
    }
  #endif
}

/* Converts unsigned long representation of GPS coordinate
 * Mavlink transmits the coordinates as coordinate * 1E7
 */
void convertLat (uint32_t in_coords)
{
  if(in_coords >= 0)
  {
    pos_NS = 0;
  } 
  else {
    pos_NS = 1;
  }
  pos_NS_dm = in_coords/100000;                 /* Get the first digits by integer divide by 100.000 and thus removing the floating point */
  pos_NS_sec = in_coords - (degMin*100000);      /* And deduct it from the original coords to get the sec */
}

void convertLon (uint32_t in_coords)
{
  if(in_coords >= 0)
  {
    pos_EW = 0;
  } 
  else
 {
    pos_EW = 1;
  }
  pos_EW_dm = in_coords/100000;                 /* Get the first digits by integer divide by 100.000 and thus removing the floating point */
  pos_EW_sec = in_coords - (degMin*100000);     /* And deduct it from the original coords to get the sec*/
}

/* run a flighttimer (min/sec) on the HoTT screen when motor is armed and throttle above min position */
static void updateFlightTimer()
{
  if ((isArmed) and (throttle >= throttleMin) )
  {
    seconds +=1;
    if (seconds > 59)
    {
      seconds = 0;
      minutes +=1;
    }
  }
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

static void checkAltitude()
{
  if (!m2h_ceiling)
  {
    if ( ((m2h_gps_alt/1E3) > height_max ) && (m2h_fix_type == 3) )
    {
      height_max = (m2h_gps_alt/1E3);
    }
  } 
  else
  {
    height_max = m2h_ceiling;
  }
}

static void varioCalc()
{
  variocounter3s ++;
  variocounter10s ++;

  /* m/s vario */
  alt_diff_1s = m2h_gps_alt - alt_start_1s;
  alt_start_1s = m2h_gps_alt;

  /* m/3s vario */
  if (variocounter3s >=3)
  {
    variocounter3s = 0;
    alt_diff_3s = m2h_gps_alt - alt_start_3s;
    if (alt_diff_3s >10) alt_diff_3s = 10;
    alt_start_3s = m2h_gps_alt;
  }

  /* m/10s vario */
  if (variocounter10s >=10)
  {
    variocounter10s = 0;
    alt_diff_10s = m2h_gps_alt - alt_start_10s;
    if (alt_diff_10s >15) alt_diff_10s = 15;
    alt_start_10s = m2h_gps_alt;
  }
}

static void checkHomeSet()
{
  /* AutoQuad needs to transmitt a special message for the home coordinates.
   * this check is in the event that the message is not send and we have a valid position signal
   * the first valid signal is then considered the home coordinate.*/
  if ((!m2h_got_home) && (m2h_fix_type == 3) && (m2h_gps_lat !=0) && (m2h_gps_lon !=0) && (m2h_gps_lat_home = 0) )
  {
    m2h_gps_lat_home = m2h_gps_lat;
    m2h_gps_lon_home = m2h_gps_lon;
    m2h_got_home = 1;
  }
}

static void checkHomeDistance()
{
  /* Home distance check, only run when AutoQuad has set home coordinates */
  //if ( (m2h_gps_lat_home != 0) &&  (m2h_gps_lon_home != 0) ){
  //  calcDist ( m2h_gps_lat_home/1e7, m2h_gps_lon_home/1e7, m2h_gps_lat/1e7, m2h_gps_lon/1e7);
  //}
  if (m2h_got_home) calcDist ( m2h_gps_lat_home/1e7, m2h_gps_lon_home/1e7, m2h_gps_lat/1e7, m2h_gps_lon/1e7);
}

/*************************************************************************
 * //Function to calculate the distance and bearing between two waypoints
 *************************************************************************/
void calcDist(float flat1, float flon1, float flat2, float flon2)
{
  float x = ( (flon2 - flon1) * DEG_TO_RAD) * cos ( ( (flat1 * DEG_TO_RAD) +(flat2 * DEG_TO_RAD) )/2);
  float y = ( (flat1 * DEG_TO_RAD) - (flat2 * DEG_TO_RAD) );
  home_distance_calc = sqrt(x*x + y*y) * NAV_EQUATORIAL_RADIUS;

  flat2 = flat2 * DEG_TO_RAD;
  flat1 = flat1 * DEG_TO_RAD;
  flon2 = flon2 * DEG_TO_RAD;
  flon1 = flon1 * DEG_TO_RAD;

  #ifdef BEARING_HOME2MAV
    bearing = atan2(sin(flon2-flon1)*cos(flat2), cos(flat1)*sin(flat2)-sin(flat1)*cos(flat2)*cos(flon2-flon1)), 2*M_PI;
    bearing = bearing * RAD_TO_DEG;  // convert from radians to degrees
    int bearing = bearing; //make it a integer now
    if(bearing<0) bearing+=360;   //if the heading is negative then add 360 to make it positive
    if(bearing>360) bearing-=360;
  #endif

  #ifdef BEARING_MAV2HOME
    bearing = atan2(sin(flon1-flon2)*cos(flat1), cos(flat2)*sin(flat1)-sin(flat2)*cos(flat1)*cos(flon1-flon2)), 2*M_PI;
    bearing = bearing * RAD_TO_DEG;  // convert from radians to degrees
    int bearing = bearing; //make it a integer now
    if(bearing<0) bearing+=360;   //if the heading is negative then add 360 to make it positive
    if(bearing>360) bearing-=360;
  #endif

  if (m2h_got_home) free_char1 = char ('.');
}

/* obsolete, use to request mavink streams 
 if ((mavlink_active) && (mavlink_request)){
 delay (500);
 request_mavlink_rates();
 delay (500);
 request_mavlink_rates();
 mavlink_request = 0;
 }
 */


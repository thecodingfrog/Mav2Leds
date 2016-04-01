/* This file contains conversion & calculation functions for values used for the HoTT protocol */

void timerEvent()
{
  #ifdef SERDB
    DPL("timerEvent()");
  #endif
  
  /* this event is called @ 10Hz */
  ioCounter ++ ;     /* update counter to use in all blink loops */
  if (ioCounter > 10) ioCounter = 0;

  /* 10Hz calls */
  /* do the LED stuff 
   * RGB controller LEDstrip or directly attached LEDs */
  heartBeat();
  RGBControl();

  /* end 10Hz calls */

  /* 1Hz calls */
  if (ioCounter == 1)
  {
    /* 1Hz routines between this line */

    checkBattery();

    /* and this line */
  }
  /* end 1Hz calls */
  
  /* Display version on display for 5 sec*/
  /*if (!displayVersionDone)
  {
    flMode = 13;
    if (millis() - p_cMillis >5000) displayVersionDone = 1;
  }*/
  /* end timer event */
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

/* obsolete, use to request mavink streams 
 if ((mavlink_active) && (mavlink_request)){
 delay (500);
 request_mavlink_rates();
 delay (500);
 request_mavlink_rates();
 mavlink_request = 0;
 }
 */


#include "Mav2Leds.h"

class MAVLinkReader
{
  private:
    long OnTime = 100;     // milliseconds of on-time
    unsigned long previousMillis;   // will store last time LED was updated

    byte __ioCounter;          /* Loop counter */
    bool __mavlink_active;
    int __messageCounter;
    /* AutoQuad received values */
    float    __vbat_A = 0;                 // Battery A voltage in milivolt
    boolean  __vbat_set = 0;               // first arrival correct voltage
    uint8_t  __num_cells = 0;              // number of cells
    uint8_t  __fuel_procent = 0;
    
    uint16_t __mode = 0;                   // Status mode (manual,stabilized etc)
    uint8_t  __nav_mode = 0;               // Navigation mode
    uint8_t  __flight_mode = 0;
    uint8_t  __sys_state = 0;                // system status (active,standby,critical)
    
    uint8_t  __gps_fix_type = 0;               // GPS lock 0-1=no fix, 2=2D, 3=3D
    
    uint16_t throttle = 0;                  // throttle value
    uint8_t severity = 0;
    char* severity_text;
    boolean checks_ok = 1;
    
    
    uint8_t mavlinkHB_char = 0;             // MavLink HeartBeat character
    
    /* MAVLink session control */
    boolean  mavbeat = 0;
    uint8_t  apm_mav_type;
    uint8_t  apm_mav_system; 
    uint8_t  apm_mav_component;
    boolean  mavlink_request = 0;
    
    /* General states */
    byte __flMode;                                  /* Our current flight mode as defined */
    byte __isArmed = 0;
    byte __isArmedOld = 0;
    byte __isActive;
    
    String __mode_str;
    int packet_drops = 0;
    int parse_error = 0;

    
    void HeartBeat();
    void CheckBattery();
    void CheckFlightMode();
    byte isBit(byte param, byte bitfield);
    
    SysState __obj;
    
  public:
    MAVLinkReader();
    void Update();
    SysState Read();
};

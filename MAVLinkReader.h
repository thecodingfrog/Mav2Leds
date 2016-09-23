#include "Mav2Leds.h"

class MAVLinkReader
{
  private:
    long OnTime = 100;     // milliseconds of on-time
    unsigned long previousMillis;   // will store last time LED was updated

    byte ioCounter;          /* Loop counter */
    bool mavlink_active;
    int messageCounter;
    /* AutoQuad received values */
    float    m2l_vbat_A = 0;                 // Battery A voltage in milivolt
    boolean  m2l_vbat_set = 0;               // first arrival correct voltage
    uint8_t  m2l_num_cells = 0;              // number of cells
    uint8_t  m2l_fuel_procent = 0;
    
    uint16_t m2l_mode = 0;                   // Status mode (manual,stabilized etc)
    uint8_t  m2l_nav_mode = 0;               // Navigation mode
    uint8_t  flight_mode = 0;
    uint8_t  m2l_sysstat = 0;                // system status (active,standby,critical)
    
    uint8_t  m2l_fix_type = 0;               // GPS lock 0-1=no fix, 2=2D, 3=3D
    
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
    byte flMode;                                  /* Our current flight mode as defined */
    byte isArmed = 0;
    byte isArmedOld = 0;
    byte isActive;
    
    String mode_str;
    int packet_drops = 0;
    int parse_error = 0;

    
    void HeartBeat();
    void CheckBattery();
    void CheckFlightMode();
    byte isBit(byte param, byte bitfield);
    
  public:
    MAVLinkReader(SysState& __SysState);
    void Update();
    void Read();
};

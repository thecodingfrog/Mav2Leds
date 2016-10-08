class SysState
{
  public:
    SysState();
    uint8_t flight_mode;
    String flight_mode_str;
    uint8_t gps_fix_type;
    float battery;
    byte is_armed;
    uint8_t system_state;
    int num_cells;
};

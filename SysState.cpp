#include "Arduino.h"
#include "SysState.h"

SysState::SysState()
{
  flight_mode = 0;
  flight_mode_str = "";
  gps_fix_type = 0;
  battery = 0;
  is_armed = 0;
  system_state = 0;
  num_cells = 0;
}


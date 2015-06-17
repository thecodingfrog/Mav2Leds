/*
  MavLinkJobs.h 
*/


#ifndef LedController_h
#define LedController_h

#include <mthread.h>

#define NUM_STRIPS 3
#define NUM_LEDS_PER_STRIP 8

#define FR  9    // Rear right    port D5
#define RR  5    // Front right   port D9
#define FL  10    // Front left    port D6
#define RL  6    // Rear left     port D10
#define CLK  11    // Clock port
#define ledPin 13     // Heartbeat LED if any

class LedController : public Thread
{
  public:
      LedController(void);
      virtual ~LedController();
  protected:
      bool loop();
};

#endif

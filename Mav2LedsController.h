/*
  Mav2LedsController.h 
*/
#ifndef Mav2LedsController_h

#define Mav2LedsController_h
#include "mthread.h"

// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "wiring.h"
#endif

class Mav2LedsController : public Thread
{
  public:
      Mav2LedsController(void);
      virtual ~Mav2LedsController();
  protected:
      bool loop();
};

extern void timerEvent(void);
extern void read_mavlink(void);

#endif

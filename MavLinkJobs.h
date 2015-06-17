/*
  MavLinkJobs.h 
*/
#ifndef MavLinkJobs_h

#define MavLinkJobs_h
#include "mthread.h"

// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "wiring.h"
#endif

class MavLinkJobs : public Thread
{
  public:
      MavLinkJobs(void);
      virtual ~MavLinkJobs();
  protected:
      bool loop();
};

extern void timerEvent(void);
extern void read_mavlink(void);

#endif

#include "MavLinkJobs.h"


MavLinkJobs::MavLinkJobs()
{
}

MavLinkJobs::~MavLinkJobs()
{
}

bool MavLinkJobs::loop()
{
    sleep_milli(100);
    read_mavlink();
    timerEvent();
    
    return true;
}

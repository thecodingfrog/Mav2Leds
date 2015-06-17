#include "Mav2LedsController.h"


Mav2LedsController::Mav2LedsController()
{
}

Mav2LedsController::~Mav2LedsController()
{
}

bool Mav2LedsController::loop()
{
    sleep_milli(100);
    read_mavlink();
    timerEvent();
    
    return true;
}

#include "Arduino.h"
#include "Strobe.h"

Strobe::Strobe(int pin, long on, long off, long on2, long off2, long on3, long off3)
{
  __ledPin = pin;
  pinMode(__ledPin, OUTPUT);     
    
  OnTime = on;
  OffTime = off;
  OnTime2 = on2;
  OffTime2 = off2;
  OnTime3 = on3;
  OffTime3 = off3;
  
  ledState = LOW;
  stateIdx = 1;
  previousMillis = 0;  
}

void Strobe::Update()
{
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();
   
  if((stateIdx == 2) && (currentMillis - previousMillis >= OnTime))
  {
    stateIdx = 3;
    ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    digitalWrite(__ledPin, ledState);  // Update the actual LED
  }
  else if ((stateIdx == 1) && (currentMillis - previousMillis >= OffTime))
  {
    stateIdx = 2;
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    digitalWrite(__ledPin, ledState);   // Update the actual LED
  }
  else if((stateIdx == 4) && (currentMillis - previousMillis >= OnTime2))
  {
    stateIdx = 5;
    ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    digitalWrite(__ledPin, ledState);  // Update the actual LED
  }
  else if ((stateIdx == 3) && (currentMillis - previousMillis >= OffTime2))
  {
    stateIdx = 4;
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    digitalWrite(__ledPin, ledState);   // Update the actual LED
  }
  
  else if((stateIdx == 6) && (currentMillis - previousMillis >= OnTime3))
  {
    stateIdx = 1;
    ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    digitalWrite(__ledPin, ledState);  // Update the actual LED
  }
  else if ((stateIdx == 5) && (currentMillis - previousMillis >= OffTime3))
  {
    stateIdx = 6;
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    digitalWrite(__ledPin, ledState);   // Update the actual LED
  }
}

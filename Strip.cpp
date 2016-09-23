#include "Arduino.h"
#include "Strip.h"

Strip::Strip(int pin2, unsigned long on2, unsigned long off2)
{
  ledPin2 = pin2;
  pinMode(ledPin2, OUTPUT);     
    
  OnTime2 = on2;
  OffTime2 = off2;
  
  ledState2 = LOW;
  previousMillis2 = 0;  
}

void Strip::Update()
{
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis2 = millis();
   
  if((ledState2 == HIGH) && (currentMillis2 - previousMillis2 >= OnTime2))
  {
    ledState2 = LOW;  // Turn it off
    previousMillis2 = currentMillis2;  // Remember the time
    digitalWrite(ledPin2, ledState2);  // Update the actual LED
  }
  else if ((ledState2 == LOW) && (currentMillis2 - previousMillis2 >= OffTime2))
  {
    ledState2 = HIGH;  // turn it on
    previousMillis2 = currentMillis2;   // Remember the time
    digitalWrite(ledPin2, ledState2);   // Update the actual LED
  }
}

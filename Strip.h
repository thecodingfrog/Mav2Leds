class Strip
{
  private:
    int ledPin2;      // the number of the LED pin
    unsigned long OnTime2;     // milliseconds of on-time
    unsigned long OffTime2;    // milliseconds of off-time
  
    // These maintain the current state
    int ledState2;                 // ledState used to set the LED
    unsigned long previousMillis2;   // will store last time LED was updated
    
  public:
    Strip(int pin2, unsigned long on2, unsigned long off2);
    void Update();
};

class Strobe
{
  private:
    long OnTime;     // milliseconds of on-time
    long OffTime;    // milliseconds of off-time
    long OnTime2;     // milliseconds of on-time
    long OffTime2;    // milliseconds of off-time
    long OnTime3;     // milliseconds of on-time
    long OffTime3;    // milliseconds of off-time
  
    // These maintain the current state
    int ledState;                 // ledState used to set the LED
    int stateIdx;
    long previousMillis;   // will store last time LED was updated
    CRGB* __FR_leds;
    CRGB* __FL_leds;
    CRGB* __RR_leds;
    CRGB* __RL_leds;
    int __led_idx;
    void Show();
    void Hide();
    
  public:
    Strobe(long on, long off, long on2, long off2, long on3, long off3);
    void Attach(CRGB* leds1, CRGB* leds2, CRGB* leds3, CRGB* leds4, int idx);
    void Update();
};

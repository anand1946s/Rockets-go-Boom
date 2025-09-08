// So this file is where we store the pin numbers etc
// when we need to change the pins just change number from here. (for example parachute trigger is set as 25)
// also the different flight logic is mentioned.this is NOT to be changed.
//add or modify pins accordingly
// sysytem health is also defined as 2 states as you can see
#ifndef CONFIG_H
#define CONFIG_H

enum FlightMode {
  IDLE,
  INIT,
  LAUNCH,
  RECOVERY,
  RETRIEVAL
};

enum SystemHealth{
  SYS_OK,
  SYS_HALT
};

const int PARA_PIN= 25 ;
const int PAY_PIN = 26;
const int SD_CS = 5;


#endif


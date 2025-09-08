
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


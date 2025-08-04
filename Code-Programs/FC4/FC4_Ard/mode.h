#ifndef MODE_H
#define MODE_H

#include "config.h"

void modeManager();
SysStatus checkSystemStatus();

bool initialize();
void debugging();
void countdown();
void launch();
void deploypayload();
void deployparachute();

#endif
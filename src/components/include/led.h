#ifndef LED_h
#define LED_h

#include "../include/unit.h"

Unit* leds_gen();
void  leds_update(const uint *counter);
void  leds_free();

#endif

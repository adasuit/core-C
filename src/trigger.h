#ifndef TRIGGER_H
#define TRIGGER_H

#include <gpiod.h>
#include <stdio.h>
#include <time.h>

#include "setup.h"

struct Counter {
    uint value;
    bool flag;
};

void button_value_update(struct Button *buttons, bool *run, bool *shift, int *count, uint *counter);
void counter_update(time_t *ntime, time_t *ltime, struct Counter *counter);

#endif

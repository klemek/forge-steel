#include "types.h"

#ifndef TIMER_H
#define TIMER_H

Timer create_timer(const unsigned int target);

bool inc_timer(Timer *timer);

double reset_and_count(Timer *timer);

#endif
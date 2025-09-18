#include "types.h"

#ifndef TIMER_H
#define TIMER_H

Timer timer_init(const unsigned int target);

bool timer_inc(Timer *timer);

double timer_reset(Timer *timer);

#endif /* TIMER_H */
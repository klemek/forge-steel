#include <sys/time.h>

#include "types.h"

#include "timer.h"

Timer timer_init(const unsigned int target) {
  Timer output;

  output.counter = 0;
  output.target = target;

  gettimeofday(&output.start, NULL);

  return output;
}

bool timer_inc(Timer *timer) {
  timer->counter += 1;
  return timer->counter >= timer->target;
}

double timer_reset(Timer *timer) {
  struct timeval stop;
  double secs, per_secs;

  gettimeofday(&stop, NULL);

  secs = (double)(stop.tv_usec - timer->start.tv_usec) / 1000000 +
         (double)(stop.tv_sec - timer->start.tv_sec);
  per_secs = (double)timer->counter / secs;

  timer->start = stop;
  timer->counter = 0;

  return per_secs;
}
#include <sys/time.h>

#include "types.h"

Timer create_timer(const unsigned int target) {
  Timer output = {
      .counter = 0,
      .target = target,
  };

  gettimeofday(&output.start, NULL);

  return output;
}

bool inc_timer(Timer *timer) {
  timer->counter += 1;
  return timer->counter >= timer->target;
}

double reset_and_count(Timer *timer) {
  struct timeval stop;
  double secs, per_secs;
  gettimeofday(&stop, NULL);
  secs = (double)(stop.tv_usec - timer->start.tv_usec) / 1000000 +
         (double)(stop.tv_sec - timer->start.tv_sec);
  per_secs = ((float)timer->counter) / secs;
  timer->start = stop;
  timer->counter = 0;
  return per_secs;
}
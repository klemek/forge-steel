#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "types.h"

#include "config.h"
#include "tempo.h"

static long now() {
  struct timeval now;

  gettimeofday(&now, NULL);

  return now.tv_sec * 1000 + now.tv_usec / 1000;
}

static void reset_tap_chain(Tempo *tempo, long t) {
  tempo->last_reset = t;
  tempo->last_tap = 0;
  tempo->taps_in_chain = 0;
  tempo->tap_duration_index = 0;
  tempo->last_tap_skipped = false;

  memset(tempo->tap_durations, 0, sizeof(tempo->tap_durations));
}

void tempo_init(Tempo *tempo) {
  long t;

  t = now();

  reset_tap_chain(tempo, t);
}

static bool is_chain_active(const Tempo tempo, long t) {
  return (tempo.last_tap + MAX_BEAT_LENGTH) > t &&
         (tempo.last_tap + (tempo.beat_length * BEATS_UNTIL_CHAIN_RESET)) > t;
}

static long get_average_tap_duration(const Tempo tempo) {
  unsigned int amount;
  long running_total;
  long average_tap_duration;

  amount = tempo.taps_in_chain - 1;
  if (amount > TOTAL_TAP_VALUES) {
    amount = TOTAL_TAP_VALUES;
  }

  running_total = 0;
  for (unsigned int i = 0; i < amount; i++) {
    running_total += tempo.tap_durations[i];
  }

  if (amount == 0) {
    amount = 1; // should never happen
  }

  average_tap_duration = running_total / amount;
  if (average_tap_duration < MIN_BEAT_LENGTH) {
    return MIN_BEAT_LENGTH;
  }

  return average_tap_duration;
}

static void add_tap_to_chain(Tempo *tempo, long t) {
  long duration;

  duration = t - tempo->last_tap;

  tempo->last_tap = t;

  tempo->taps_in_chain++;

  if (tempo->taps_in_chain == 1) {
    return;
  }

  if (tempo->taps_in_chain > 2 && !tempo->last_tap_skipped &&
      duration > tempo->beat_length * SKIPPED_TAP_THRESHOLD_LOW &&
      duration < tempo->beat_length * SKIPPED_TAP_THRESHOLD_HIGH) {
    duration = duration >> 1;
    tempo->last_tap_skipped = true;
  } else {
    tempo->last_tap_skipped = false;
  }

  tempo->tap_durations[tempo->tap_duration_index++] = duration;

  if (tempo->tap_duration_index == TOTAL_TAP_VALUES) {
    tempo->tap_duration_index = 0;
  }

  tempo->beat_length = get_average_tap_duration(*tempo);

  tempo->tempo = 60000.0 / tempo->beat_length;
}

void tempo_set(Tempo *tempo, float value) {
  long t;
  long progress;

  t = now();

  progress = (t - tempo->last_reset) % tempo->beat_length;

  tempo->tempo = value;
  tempo->beat_length = 60000.0 / value;

  reset_tap_chain(tempo, t - progress);
}

void tempo_tap(Tempo *tempo) {
  long t;

  t = now();

  if (!is_chain_active(*tempo, t)) {
    reset_tap_chain(tempo, t);
  }

  add_tap_to_chain(tempo, t);
}

double tempo_total(const Tempo *tempo) {
  long t;

  t = now();

  return (double)(t - tempo->last_reset) / (double)tempo->beat_length;
}

double tempo_progress(const Tempo *tempo, double modulo) {
  return fmod(tempo_total(tempo), modulo);
}
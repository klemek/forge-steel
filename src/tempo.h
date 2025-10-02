#include "types.h"

#ifndef TEMPO_H
#define TEMPO_H

Tempo tempo_init();

void tempo_tap(Tempo *tempo);

void tempo_set(Tempo *tempo, float value);

double tempo_progress(Tempo tempo, double modulo);

#endif /* TEMPO_H */
#include "types.h"

#ifndef TEMPO_H
#define TEMPO_H

void tempo_init(Tempo *tempo);

void tempo_tap(Tempo *tempo);

void tempo_set(Tempo *tempo, float value);

double tempo_total(Tempo *tempo);

double tempo_progress(Tempo *tempo, double modulo);

#endif /* TEMPO_H */
#include "types.h"

#ifndef STATE_H
#define STATE_H

StateConfig state_parse_config(ConfigFile config);

void state_randomize(SharedContext *context, StateConfig state_config,
                     unsigned int state_count);

void state_free_config(StateConfig state_config);

#endif /* STATE_H */
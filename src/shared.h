#include "types.h"

#ifndef SHARED_H
#define SHARED_H

SharedUint *shared_init_uint(const char *key, unsigned int initial_value);
void shared_close_uint(SharedUint *shared);

SharedBool *shared_init_bool(const char *key, bool initial_value);
void shared_close_bool(SharedBool *shared);

#endif /* SHARED_H */
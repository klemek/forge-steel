#include "types.h"

#ifndef SHARED_H
#define SHARED_H

SharedContext *shared_init_context(const char *key);
void shared_close_context(SharedContext *shared);

#endif /* SHARED_H */
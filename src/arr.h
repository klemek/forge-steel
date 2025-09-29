#include "types.h"

#ifndef ARR_H
#define ARR_H

unsigned int arr_uint_index_of(UintArray array, unsigned int value);

unsigned int arr_uint_remap_index(UintArray offsets, unsigned int *index);

#endif /* ARR_H */
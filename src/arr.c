#include "types.h"

#include "arr.h"

unsigned int arr_uint_index_of(UintArray array, unsigned int value) {
  for (unsigned int i = 0; i < array.length; i++) {
    if (array.values[i] == value) {
      return i;
    }
  }

  return ARRAY_NOT_FOUND;
}

unsigned int arr_uint_remap_index(UintArray offsets, unsigned int *index) {
  for (unsigned int i = offsets.length - 1; i > 0; i--) {
    if (*index >= offsets.values[i]) {
      *index -= offsets.values[i];
      return i;
    }
  }

  return 0;
}

#include "arr.h"

unsigned int arr_uint_index_of(UintArray array, unsigned int value) {
  unsigned int i;

  for (i = 0; i < array.length; i++) {
    if (array.values[i] == value) {
      return i;
    }
  }

  return ARRAY_NOT_FOUND;
}
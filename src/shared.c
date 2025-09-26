#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shared.h"
#include "types.h"

static void *open_shared(const char *key, size_t size, int *fd) {
  *fd = shm_open(key, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  ftruncate(*fd, size);

  return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);
}

static void close_shared(void *shared, size_t size, int fd) {
  munmap(shared, size);
  close(fd);
}

SharedUint *shared_init_uint(const char *key, unsigned int initial_value) {
  int shared_fd;
  SharedUint *shared;

  shared = open_shared(key, sizeof(SharedUint), &shared_fd);

  shared->fd = shared_fd;
  shared->value = initial_value;

  return shared;
}

void shared_close_uint(SharedUint *shared) {
  close_shared(shared, sizeof(SharedUint), shared->fd);
}

SharedBool *shared_init_bool(const char *key, bool initial_value) {
  int shared_fd;
  SharedBool *shared;

  shared = open_shared(key, sizeof(SharedBool), &shared_fd);

  shared->fd = shared_fd;
  shared->value = initial_value;

  return shared;
}

void shared_close_bool(SharedBool *shared) {
  close_shared(shared, sizeof(SharedBool), shared->fd);
}

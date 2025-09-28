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

SharedContext *shared_init_context(const char *key) {
  int shared_fd;
  SharedContext *shared;

  shared = open_shared(key, sizeof(SharedContext), &shared_fd);

  shared->fd = shared_fd;

  return shared;
}

void shared_close_context(SharedContext *shared) {
  close_shared(shared, sizeof(SharedContext), shared->fd);
}
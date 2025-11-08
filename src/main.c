#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"

#include "args.h"
#include "config.h"
#include "forge.h"
#include "main.h"
#include "rand.h"

int main(int argc, char **argv) {
  Parameters params;

  params = args_parse(argc, argv);

  puts(PACKAGE " " VERSION);

  set_seed((unsigned long)time(NULL));

  forge_run(params);

  return EXIT_SUCCESS;
}

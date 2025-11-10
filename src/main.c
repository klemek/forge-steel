#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"

#include "args.h"
#include "config.h"
#include "forge.h"
#include "main.h"
#include "rand.h"

static Parameters params;

int main(int argc, char **argv) {
  args_parse(&params, argc, argv);

  puts(PACKAGE " " VERSION);

  set_seed((unsigned long)time(NULL));

  forge_run(&params);

  return EXIT_SUCCESS;
}

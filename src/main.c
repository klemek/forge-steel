#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "config.h"
#include "forge.h"

int main(int argc, char **argv) {
  Parameters params;
  params = parse_args(argc, argv);
  puts(PACKAGE " " VERSION);
  forge_run(params);
  return EXIT_SUCCESS;
}

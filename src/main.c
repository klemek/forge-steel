#include "args.h"
#include "config.h"
#include "forge.h"
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char **argv) {
  parameters params;
  params = parse_args(argc, argv);
  puts(PACKAGE " " VERSION);
  forge_run(params);
  return 0;
}

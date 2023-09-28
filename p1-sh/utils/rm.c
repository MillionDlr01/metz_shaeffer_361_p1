#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void usage (void);

int
main (int argc, char *argv[])
{
  // parse filename
  if (optind == argc - 1) // this means a file name was passed
    {
      char *filename = argv[optind];
      if (remove (filename) != 0)
        {
          return EXIT_FAILURE;
        }
    }
  else
    {
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("rm, remove a file\n");
  printf ("usage: rm FILE\n");
}

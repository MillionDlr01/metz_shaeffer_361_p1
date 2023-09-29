#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage (void);

int
main (int argc, char *argv[])
{
  // parse filename
  char *filename = argv[optind];

  // open file
  FILE *file = fopen (filename, "r");
  if (file == NULL)
    {
      return EXIT_FAILURE;
    }

  // print contents
  char buffer [2048];
  memset(buffer, 0, 2048);
  while (fread(buffer, sizeof(char), 2047, file) != 0)  {
    printf("%s", buffer);
    memset(buffer, 0, 2048);
  }
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("cat, print the contents of a file\n");
  printf ("usage: cat FILE\n");
}

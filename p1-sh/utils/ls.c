#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage (void);

int
main (int argc, char *argv[])
{
  // cmd char
  int ch = 0, hiddenFlag = 0, sizeFlag = 0;

  // parse optional flag
  while ((ch = getopt (argc, argv, "as")) != -1)
    {
      switch (ch)
        {
        case 'a': 
          hiddenFlag = 1;
          break;
        case 's': 
          sizeFlag = 1;
          break;
        default:
          printf ("bad argument flag");
          usage ();
          return EXIT_FAILURE;
        }
    }

  // parse optional filename
  if (optind == argc) // this means no file name was passed, use current working directory
    {

    }
  else if (optind == argc - 1) // this means a file name has been passed
    {
      char *filename = argv[optind];
      // attempt to open file
      FILE *file = fopen (filename, "r");
      // check that the file was opened
      if (file == NULL)
        {
          printf ("file does not exist");
          usage ();
          return EXIT_FAILURE;
        }
    }
  else
    { // this means too many args have been passed.
      printf ("too many non flag args passed");
      usage ();
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("ls, list directory contents\n");
  printf ("usage: ls [FLAG ...] [DIR]\n");
  printf ("FLAG is one or more of:\n");
  printf ("  -a       list all files (even hidden ones)\n");
  printf ("  -s       list file sizes\n");
  printf ("If no DIR specified, list current directory contents\n");
}

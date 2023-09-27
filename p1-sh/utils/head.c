#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// You may assume that lines are no longer than 1024 bytes
#define LINELEN 1024
// No command line can be more than 100 characters
#define MAXLENGTH 100
/* newline, and carriage return to match whitespace between tokens */
#define WHITESPACE "\n\r"

static void usage (void);

int
main (int argc, char *argv[])
{
  // cmd char | number of lines to print (5 default)
  int ch = 0, numLines = 5;

  // parse optional flag
  while ((ch = getopt (argc, argv, "n:")) != -1)
    {
      switch (ch)
        {
        case 'n': // only option (sets numLines and nothing else happens)
          numLines = atoi (optarg);
          break;
        default:
          printf ("bad argument flag");
          usage ();
          return EXIT_FAILURE;
        }
    }

  // parse optional filename
  if (optind == argc) // this means no file nam was passed; read from stdin
    {
      // get buffer
      char buffer[LINELEN + 1];

      // print number of lines
      for (int i = 0; i < numLines; i++)
        {
          memset (buffer, 0, LINELEN + 1);
          fgets (buffer, MAXLENGTH, stdin);
          printf ("%s", buffer);
        }
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

      // print number of lines
      for (int i = 0; i < numLines; i++)
        {
          // make line buffer
          char buffer[LINELEN + 1];
          memset (buffer, 0, LINELEN + 1);
          // read line
          fgets (buffer, LINELEN, file);
          // print line
          printf ("%s", buffer);
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
  printf ("head, prints the first few lines of a file\n");
  printf ("usage: head [FLAG] FILE\n");
  printf ("FLAG can be:\n");
  printf ("  -n N     show the first N lines (default 5)\n");
  printf ("If no FILE specified, read from STDIN\n");
}

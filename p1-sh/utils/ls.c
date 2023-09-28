#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

static void usage (void);
static void printEntry (struct dirent *, int, int);

// No command line can be more than 100 characters
#define MAXLENGTH 256

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

  // parse optional path
  if (optind
      == argc) // this means no path was passed, use current working directory
    {
      // make directory obj of current directory
      DIR *directory = opendir (".");
      if (directory == NULL)
        {
          printf ("current directory not reachable.");
        }
      // struct for directory entries
      struct dirent *entry;

      while ((entry = readdir (directory)) != NULL)
        {
          printEntry (entry, hiddenFlag, sizeFlag);
        }
    }
  else if (optind == argc - 1) // this means a file name has been passed
    {
      // get path name argument
      char *path = argv[optind];
      // make directory obj based on path arg
      DIR *directory = opendir (path);
      if (directory == NULL)
        {
          printf ("provided directory not reachable.");
        }
      // struct for directory entries
      struct dirent *entry;

      while ((entry = readdir (directory)) != NULL)
        {
          printEntry (entry, hiddenFlag, sizeFlag);
        }
    }
  else
    { // this means too many args have been passed.
      printf ("too many non flag args passed\n");
      usage ();
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

static void
printEntry (struct dirent *entry, int hiddenFlag, int sizeFlag)
{

  if (hiddenFlag == 1)
    {
      // this branch will display hidden files
      if (sizeFlag == 1) // print size of file
        {
          struct stat st;
          stat(entry->d_name, &st);
          long size = st.st_blksize;
          printf ("%ld ", size);
        }
      // print name followed by a newline
      printf ("%s", entry->d_name);
      printf ("\n");
    }
  else
    {
      // this branch will exclude hidden files
      if (entry->d_name[0] != '.') // exclude files that begin with '.'
        {
          if (sizeFlag == 1) // print size of file
            {
              struct stat st;
              stat(entry->d_name, &st);
              long size = st.st_blksize;
              printf ("%ld ", size);
            }
          // print name followed by a newline
          printf ("%s", entry->d_name);
          printf ("\n");
        }
    }
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

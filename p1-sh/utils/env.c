#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void usage (void);

int
main (int argc, char *argv[], char *envp[])
{
  if (optind == argc) // no args passed, loop through envp and print
    {

      return EXIT_SUCCESS; // exit at the end of noarg operaion
    }
  // if no args passed
  // create new envp array

  // populate envp with the args

  // get program name

  // construct args

  // spawn process

  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("env, set environment variables and execute program\n");
  printf ("usage: env [name=value ...] PROG ARGS\n");
}


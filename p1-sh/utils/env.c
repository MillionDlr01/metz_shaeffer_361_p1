#include <errno.h>
#include <getopt.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void usage (void);

int
main (int argc, char *argv[], char *envp[])
{
  if (argc == 1) // no args passed, loop through envp and print
    {
      int i = 0;
      while (envp[i] != NULL)
        {
          printf ("%s\n", envp[i]);
          i++;
        }
      return EXIT_SUCCESS; // exit at the end of noarg operaion
    }
  // if no args passed
  // create new envp array
  int enviornmentVariableCount = 0, tmp = optind, envpIndex = 0;
  while (strchr (argv[optind], '=') != NULL)
    {
      enviornmentVariableCount++;
      optind++;
    }
  char *enviornment[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL, NULL, NULL, NULL };
  optind = tmp;
  // populate enviornment with the args
  while (strchr (argv[optind], '=') != NULL)
    {
      enviornment[envpIndex] = argv[optind];
      envpIndex++;
      optind++;
    }
  // get program name
  // path string for finding files in printentry
  char *program = argv[optind];
  // construct args
  int argsCount = argc - optind;
  char *arguments[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                        NULL, NULL, NULL, NULL, NULL, NULL };
  for (int i = 0; i < argsCount; i++)
    {
      arguments[i] = argv[optind];
      optind++;
    }
  // spawn process
  posix_spawn_file_actions_t file_actions;
  pid_t child;
  if (posix_spawn_file_actions_init (&file_actions) != 0)
    {
      return EXIT_FAILURE;
    }
  if (posix_spawn (&child, program, &file_actions, NULL, arguments,
                   enviornment)
      != 0)
    {
      printf ("spawn failed. err: %d\n", errno);
      return EXIT_FAILURE;
    }
  if (posix_spawn_file_actions_destroy (&file_actions) != 0)
    {
      return EXIT_FAILURE;
    }
  // wait for child process to run before returning
  waitpid (child, NULL, 0);
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("env, set environment variables and execute program\n");
  printf ("usage: env [name=value ...] PROG ARGS\n");
}

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
  char *tmpEnviornment[enviornmentVariableCount];
  optind = tmp;
  // populate enviornment with the args
  while (strchr (argv[optind], '=') != NULL)
    {
      tmpEnviornment[envpIndex] = argv[optind];
      envpIndex++;
      optind++;
    }
  char *const *enviornment = tmpEnviornment;
  // get program name
  // path string for finding files in printentry
  char path[255];
  getcwd (path, 256);
  char *program = argv[optind];
  program += 1;
  strncat(path, program, 100);
  printf("%s\n", path);
  // construct args
  int argsCount = argc - optind;
  char *tmpArguments[argsCount];
  for (int i = 0; i < argsCount; i++)
    {
      tmpArguments[i] = argv[optind];
      optind++;
    }
  char *const *arguments = tmpArguments;
  // spawn process
  posix_spawn_file_actions_t file_actions;
  pid_t child;
  if (posix_spawn_file_actions_init (&file_actions) != 0)
    {
      return EXIT_FAILURE;
    }
  // int pipefd[2];
  // pipe (pipefd);
  // if (posix_spawn_file_actions_addclose (&file_actions, pipefd[0]) != 0)
  //   {
  //     close (pipefd[0]);
  //     close (pipefd[1]);
  //     return EXIT_FAILURE;
  //   }
  // if (posix_spawn_file_actions_adddup2 (&file_actions, pipefd[1],
  //                                       STDOUT_FILENO)
  //     != 0)
  //   {
  //     close (pipefd[0]);
  //     close (pipefd[1]);
  //     return EXIT_FAILURE;
  //   }
  if (posix_spawn (&child, path, &file_actions, NULL, arguments,
                   enviornment)
      != 0)
    {
      printf ("spawn failed\n");
      // close (pipefd[0]);
      // close (pipefd[1]);
      return EXIT_FAILURE;
    }
  if (posix_spawn_file_actions_destroy (&file_actions) != 0)
    {
      // close (pipefd[0]);
      // close (pipefd[1]);
      return EXIT_FAILURE;
    }
  // close (pipefd[1]);
  // char buffer[2048];
  // memset (buffer, 0, 2048);
  // read (pipefd[0], buffer, 2047);
  // printf ("%s", buffer);
  // close (pipefd[0]);
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

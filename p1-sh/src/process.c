// The contents of this file are up to you, but they should be related to
// running separate processes. It is recommended that you have functions
// for:
//   - performing a $PATH lookup
//   - determining if a command is a built-in or executable
//   - running a single command in a second process
//   - running a pair of commands that are connected with a pipe
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hash.h"

// make sure to free the return value
char *
path_lookup (char *filename)
{ // return the absolute path of the file found in the list of PATH
  // char *path = hash_find ("PATH");
  char *path = hash_find ("PATH");
  char *curpath = calloc (128, sizeof (char));
  char *token;
  token = strtok (path, ":");
  while (token != NULL)
    {
      strcat (curpath, token);
      strcat (curpath, "/");
      strcat (curpath, filename);
      if (access (curpath, F_OK) == 0)
        {
          // file exists
          return curpath;
        }
      memset (curpath, 0, 128);
      token = strtok (NULL, ":");
    }

  return NULL;
}

bool
check_builtin (char *filename)
{ // return true if program is a built in
  size_t len = strlen (filename);
  return ((!strncmp (filename, "cd", 2) && len == 2)
          || (!strncmp (filename, "echo", 4) && len == 4)
          || (!strncmp (filename, "pwd", 3) && len == 3)
          || (!strncmp (filename, "quit", 4) && len == 4)
          || (!strncmp (filename, "which", 5) && len == 5)
          || (!strncmp (filename, "export", 6) && len == 6)
          || (!strncmp (filename, "unset", 5) && len == 5));
}

int
exec_cmd (char *filename)
{ // creates a new process to execute a command, returns status?
  return 0;
}

int
pipe_cmd (char *prgm1, char *prgm2)
{ // pipe output of program 1 to program 2's input, return status?
  return 0;
}

char *
collapse_args (char **args, size_t argc)
{ // combine the args into a single string separate by spaces
  char *buffer = calloc (101, sizeof (char));
  if (argc < 1)
    {
      buffer[0] = '\0';
      return buffer;
    }
  int ind = 1;
  strcat (buffer, args[0]);
  while (ind < argc)
    {
      strcat (buffer, " ");
      strcat (buffer, args[ind]);
      ind += 1;
    }

  fflush (stdout);
  return buffer;
}
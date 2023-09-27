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

char *
path_lookup (char *filename)
{ // return the absolute path of the file found in the list of PATH
  return "";
}

bool
check_builtin (char *filename)
{ // return true if program is a built in
  return true;
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
  int ind = 0;
  while (ind < argc)
    {
      strcat (buffer, args[ind]);
      strcat (buffer, " ");
      ind += 1;
    }
  return buffer;
}
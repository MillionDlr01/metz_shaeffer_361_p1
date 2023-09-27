#include <stdio.h>
#include <string.h>

#include "hash.h"

// Given a message as input, print it to the screen followed by a
// newline ('\n'). If the message contains the two-byte escape sequence
// "\\n", print a newline '\n' instead. No other escape sequence is
// allowed. If the sequence contains a '$', it must be an environment
// variable or the return code variable ("$?"). Environment variable
// names must be wrapped in curly braces (e.g., ${PATH}).
//
// Returns 0 for success, 1 for errors (invalid escape sequence or no
// curly braces around environment variables).
int
echo (char *message)
{
  // check for newline formats and no other escape

  // check for $ formats
  char *dol = strchr (message, '$');
  while (dol)
    {
      // do checks for ? or {}

      dol = strchr (dol + 1, '$');
    }
  printf ("%s", message);

  return 0;
}

// Given a key-value pair string (e.g., "alpha=beta"), insert the mapping
// into the global hash table (hash_insert ("alpha", "beta")).
//
// Returns 0 on success, 1 for an invalid pair string (kvpair is NULL or
// there is no '=' in the string).
int export (char *kvpair) { return 0; }

// Prints the current working directory (see getcwd()). Returns 0.
int
pwd (void)
{
  printf("%s", hash_find("CWD")); //prints the global var called CWD
  return 0;
}

// Removes a key-value pair from the global hash table.
// Returns 0 on success, 1 if the key does not exist.
int
unset (char *key)
{
  char *val = hash_find(key);
  if (!val || strncmp("CWD", key, 3) == 0 || strncmp("PATH", key, 4) == 0) {  //error if value is null, or key is CWD or PATH
    return 1;
  }
  hash_remove(key);
  return 0;
}

// Given a string of commands, find their location(s) in the $PATH global
// variable. If the string begins with "-a", print all locations, not just
// the first one.
//
// Returns 0 if at least one location is found, 1 if no commands were
// passed or no locations found.
int
which (char *cmdline)
{
  return 0;
}


int
cd (char *path)   //not sure why this wasn't stubbed already?
{
  if (!path) {
    return 1;
  }
  char cwd[256];
  snprintf(cwd, 256, "%s", hash_find("CWD"));
  hash_insert("CWD", strcat(cwd, path));  //TODO - improve this to actually remove dirs for .. backtracking
  return 0;
}
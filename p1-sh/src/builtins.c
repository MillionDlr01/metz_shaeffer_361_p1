#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

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
  char buffer[101]; // TODO - replace all white space with single space
  strncpy (buffer, message, 100);
  // check for newline formats and no other escape
  char *newl = strstr (buffer, "\\n");
  while (newl)
    {
      newl[0] = '\n';
      memmove (newl + 1, newl + 2, strlen (newl) - 1);
      newl = strstr (newl + 1, "\\n");
    }

  // check for $ formats
  /*char *dol = strchr (buffer, '$');
  char temp[101];
  while (dol)
    {
      // do checks for ? or {}
      if (strlen (dol) >= 2 && dol[1] == '?')
        {
          dol[0] = '0' + strtol(hash_find ("?"), NULL, 10);
          memmove (dol + 1, dol + 2, strlen (dol) - 1);
        }
      else if (strlen (dol) >= 2 && dol[1] == '{')
        {
          size_t len = 0;
          char *endbrace = strchr (dol, '}');
          char *dolt = dol + 2;
          while (dolt != endbrace)
            {
              len += 1;
              dolt += 1;
            }
          snprintf (temp, len, "%s", dol);
          char *res = hash_find (temp);
          // TODO - replace index with res
        }
      else
        {
          return 1;
        }
      dol = strchr (dol + 1, '$');
    }*/
  printf ("%s\n", buffer);

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
  char buffer[256];
  getcwd (buffer, 256);
  printf ("%s\n", buffer);
  return 0;
}

// Removes a key-value pair from the global hash table.
// Returns 0 on success, 1 if the key does not exist.
int
unset (char *key)
{
  char *val = hash_find (key);
  if (!val || strncmp ("PATH", key, 4) == 0)
    { // error if value is null, or key is CWD or PATH
      return 1;
    }
  hash_remove (key);
  return 0;
}

// Given a string of commands, find their location(s) in the $PATH global
// variable. If the string begins with "-a", print all locations, not just
// the first one.
//
// Returns 0 if at least one location is found, 1 if no commands were
// passed or no locations found.
int
which (char *cmdline) //I think we only do single commands, not a bunch like it says
{
  //if builtin: "cmd: dukesh built-in command" assuming cmd is command name
  //if begins with ./ check if it is executable
  //otherwise traverse PARTH for a file that matches
  if (!cmdline) {
    return 1;
  }

  if (!strncmp(cmdline, "cd", 2) || !strncmp(cmdline, "echo", 4) ||!strncmp(cmdline, "pwd", 3) ||!strncmp(cmdline, "quit", 4) ||!strncmp(cmdline, "which", 5) ||!strncmp(cmdline, "export", 6) ||!strncmp(cmdline, "unset", 5)) {
    printf("%s: dukesh built-in command", cmdline);
  } else if (strlen(cmdline) >= 2 && cmdline[0] == '.' && cmdline[1] == '/') {
    struct stat sb;
      if (stat(cmdline, &sb) == 0 && sb.st_mode & S_IXUSR) {  //test this
        print(cmdline);
        return 0;
      } else {
        return 1;
      }
  } else {

  }
  return 0;
}

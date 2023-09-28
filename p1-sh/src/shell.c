#include <assert.h>
#include <fcntl.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

#include "builtins.h"
#include "hash.h"
#include "process.h"

// No command line can be more than 100 characters
#define MAXLENGTH 100

/* Allow spaces, tabs, newline, and carriage return to match
   whitespace between tokens */
#define WHITESPACE " \t\n\r"

void run_child_process (char *, char **, size_t, char *);
char **tokenize_arguments (char *, char **, size_t *);
char **get_out_name (char *, char **, char **);
int setup_output_file (char *, bool);

void
shell (FILE *input)
{
  hash_init (100);
  hash_insert ("?", "0");
  char cwdbuf[256]; // use to store result of getcwd for default CWD
  getcwd (cwdbuf, 256);
  hash_insert ("CWD", cwdbuf);
  hash_insert ("PATH",
               "/cs/home/stu/metzza/cs361/metz_shaeffer_361_p1/p1-sh/utils,");
  char buffer[MAXLENGTH + 1];
  while (1)
    {
      printf ("$ ");
      memset (buffer, 0, MAXLENGTH + 1);
      if (fgets (buffer, MAXLENGTH, input) == NULL)
        {
          break;
        }
      if (input != stdin)
        {
          printf ("%s", buffer);
        }

      char *command = strtok (buffer, WHITESPACE);

      //     /* Get the array of arguments and determine the output file
      //        to use (if the line ends with "> output" redirection). */
      char *output = NULL;
      size_t argc;
      char **arg_list = tokenize_arguments (buffer, &output, &argc);

      if (arg_list == NULL)
        {
          perror ("-bash: syntax error\n");
          fflush (stdout);
          free (arg_list);
          continue;
        }

      arg_list[0] = command;
      if (!command)
        {
          printf ("no command\n");
          fflush (stdout);
          free (arg_list);
          continue;
        }

      if (!strncmp (command, "quit", 4))
        {
          fflush (stdout);
          free (arg_list);
          break;
        }
      else if (!strncmp (command, "cd", 2))
        {
          if (arg_list[1][0] == '/')
            {
              chdir (arg_list[1]);
            }
          else
            {
              char cwd[256];
              getcwd (cwd, 256);
              strcat (cwd, "/");
              strcat (cwd, arg_list[1]);
              chdir (cwd);
            }
        }
      else
        {
          fflush (stdout); // without this, the child process prints the
                           // command a second time
          run_child_process (command, arg_list, argc, output);

          //         free (arg_list);
        }
      fflush (stdout);
      free (arg_list);
    }

  printf ("\n");
  hash_destroy ();
}

/* Runs a command in child process this method creates. The command
   string should already be copied as the first argument in the
   list. If the user typed an output redirection ("> out" or
   ">out"), then output_file is the name of the file to create.
   Otherwise, output_file is NULL. Should never return.
   Use posix spawn for utility files, and just call builtins*/
void
run_child_process (char *command, char **arg_list, size_t argc,
                   char *output_file)
{
  if (check_builtin (command))
    {
      // fork builtin
      pid_t child = fork ();
      if (child == 0)
        {
          int out_fd = setup_output_file (output_file, true);
          if (!strncmp (command, "echo", 4))
            {
              char *collapsed = collapse_args (arg_list + 1, argc - 1);
              echo (collapsed);
              free(collapsed);
            }
          else if (!strncmp (command, "pwd", 3))
            {
              pwd ();
            }
          else if (!strncmp (command, "which", 5))
            {
              which (arg_list[1]);
            }

          if (out_fd > 0)
            {
              close (out_fd);
            }
          exit (0);
        }
      else
        {
          wait (NULL);
          return;
        }
    }
  else
    {
      // posix spawn utility
      // setup_output_file (output_file);
      posix_spawn_file_actions_t file_actions;
      pid_t child;
      int s = posix_spawn_file_actions_init (&file_actions);
      int out_fd = 0;
      if (s != 0)
        {
          return;
        }
      if (output_file)
        {
          out_fd = setup_output_file (output_file, false);
          s = posix_spawn_file_actions_adddup2 (&file_actions, out_fd, 1);
          if (s != 0)
            {
              return;
            }
        }

      s = posix_spawn (&child, command, &file_actions, NULL, arg_list,
                       NULL); // TODO - add envs
      if (s != 0)
        {
          return;
        }
      s = posix_spawn_file_actions_destroy (&file_actions);
      if (s != 0)
        {
          return;
        }
      // Parent code: read the value back from the pipe into a dynamically
      // allocated buffer. Wait for the child to exit, then return the
      // buffer.
      int status;
      waitpid (child, &status, 0);
      char statusString[3];
      snprintf (statusString, 3, "%d", status);
      hash_insert ("?", statusString);
      if (out_fd > 0)
        {
          close (out_fd);
        }
      if (status != 0)
        {
          return;
        }
      return;
    }

  /* OLD: Use execvp, because we are not doing a PATH lookup and the
     arguments are in a dynamically allocated array */

  /* Should never reach here. Print an error message, free up
     resources, and exit. */
  fprintf (stderr, "-bash-lite: %s: command not found\n", command);
  free (arg_list);
  exit (EXIT_FAILURE);
}

int
setup_output_file (char *file, bool dup)
{
  int out_fd = -1;
  /* If there is a specified output file, open it and redirect
     STDOUT to write to this file */
  if (file != NULL)
    {
      out_fd = open (file, O_RDWR | O_CREAT);
      if (out_fd < 0)
        {
          fprintf (stderr, "-bash-lite: failed to open file %s\n", file);
          exit (EXIT_FAILURE);
        }

      /* Make the file readable and redirect STDOUT */
      fchmod (out_fd, 0644);
      if (dup)
        {
          dup2 (out_fd, STDOUT_FILENO);
        }
      return out_fd;
    }
  return 0;
}

/* Given a command line (buffer), create the list of arguments to
   use for the child process. If the command line ends in an output
   redirection, update the output_file pointer to point to the name
   of the file to use. */
char **
tokenize_arguments (char *buffer, char **output_file,
                    size_t *n) // put number of args in n
{
  assert (buffer != NULL);
  assert (output_file != NULL);
  char *token = NULL;
  *n = 1;

  /* Allocate an initial array for 10 arguments; this can grow
     later if needed */
  size_t arg_list_capacity = 10;
  char **arguments = calloc (arg_list_capacity, sizeof (char *));
  assert (arguments != NULL);

  /* Leave the first space blank for the command name */
  size_t arg_list_length = 1;

  while ((token = strtok (NULL, WHITESPACE)) != NULL)
    {
      *n = *n + 1;
      /* If token starts with >, it is an output redirection. The
         rest of the line must be the file name. Need to pass both
         the rest of the token and the buffer, as there might not
         be a space before the file name. */
      if (token[0] == '>')
        return get_out_name (&token[1], output_file, arguments);

      /* If current argument array is full, double its capacity */
      if ((arg_list_length + 1) == arg_list_capacity)
        {
          arg_list_capacity *= 2;
          arguments = realloc (arguments, arg_list_capacity * sizeof (char *));
          assert (arguments != NULL);
        }

      /* Add the token to the end of the argument list */
      arguments[arg_list_length++] = token;
    }

  return arguments;
}

/* Determine the output file from either the token that begins with
   the '>' character (but that character was removed) or from the
   next token on the command line. Note that all tokens after the
   output file name will be ignored. */
char **
get_out_name (char *token, char **output_file, char **arguments)
{
  /* If token is not an empty string, it contains the output file
     name */
  if (strlen (token) != 0)
    {
      *output_file = token;
      return arguments;
    }

  /* Token is empty, so there was a space after the '>' symbol.
     There should be one token left that is the file name. */
  token = strtok (NULL, WHITESPACE);
  if (token == NULL)
    {
      /* This is an error, no file name was passed */
      free (arguments);
      return NULL;
    }

  /* The last token is the file name, so return it and the argument
     list */
  *output_file = token;
  return arguments;
}
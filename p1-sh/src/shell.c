#include <assert.h>
#include <fcntl.h>
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

void run_child_process (char *, char **, char *);
char **tokenize_arguments (char *, char **);
char **get_out_name (char *, char **, char **);

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
      memset (buffer, 0, MAXLENGTH + 1);
      // Print the cursor and get the next command entered
      printf ("$ ");
      if (fgets (buffer, MAXLENGTH, input) == NULL)
        break;

      /* Get the command for this line without any arguments */
      char *command = strtok (buffer, WHITESPACE);

      /* Get the array of arguments and determine the output file
         to use (if the line ends with "> output" redirection). */
      char *output = NULL;
      char **arg_list = tokenize_arguments (buffer, &output);

      /* If something went wrong, skip this line */
      if (arg_list == NULL)
        {
          perror ("-bash: syntax error\n");
          continue;
        }

      /* Copy the command name as the first argument */
      arg_list[0] = command;
      if (!command)
        {
          continue;
        }
      if (strlen (command) >= 4 && !strncmp (command, "quit", 4))
        {
          exit (EXIT_SUCCESS);
        }
      /* Create the child process and execute the command in it */
      pid_t child_pid = fork ();
      assert (child_pid >= 0);
      if (child_pid == 0)
        {
          run_child_process (command, arg_list, output);
        }

      if (input != stdin)
        {
          printf ("%s", buffer);
        }

      /* Parent waits for the child, then frees up allocated memory
       for the argument list and moves on to the next line */
      wait (NULL);
      free (arg_list);
    }
  printf ("\n");
  hash_destroy ();
}

/* Runs a command in an already created child process. The command
   string should already be copied as the first argument in the
   list. If the user typed an output redirection ("> out" or
   ">out"), then output_file is the name of the file to create.
   Otherwise, output_file is NULL. Should never return. */
void
run_child_process (char *command, char **arg_list, char *output_file)
{
  int out_fd = -1;

  /* If there is a specified output file, open it and redirect
     STDOUT to write to this file */
  if (output_file != NULL)
    {
      out_fd = open (output_file, O_RDWR | O_CREAT);
      if (out_fd < 0)
        {
          fprintf (stderr, "-bash-lite: failed to open file %s\n",
                   output_file);
          exit (EXIT_FAILURE);
        }

      /* Make the file readable and redirect STDOUT */
      fchmod (out_fd, 0644);
      dup2 (out_fd, STDOUT_FILENO);
    }
  exit (0);
  /* Use execvp, because we are not doing a PATH lookup and the
     arguments are in a dynamically allocated array */
  // execvp (command, arg_list);  //change from here to instead call our method

  /* Should never reach here. Print an error message, free up
     resources, and exit. */
  // fprintf (stderr, "-bash-lite: %s: command not found\n", command);
  // free (arg_list);
  // if (out_fd >= 0)
  //   close (out_fd);
  // exit (EXIT_FAILURE);
}

/* Given a command line (buffer), create the list of arguments to
   use for the child process. If the command line ends in an output
   redirection, update the output_file pointer to point to the name
   of the file to use. */
char **
tokenize_arguments (char *buffer, char **output_file)
{
  assert (buffer != NULL);
  assert (output_file != NULL);
  char *token = NULL;

  /* Allocate an initial array for 10 arguments; this can grow
     later if needed */
  size_t arg_list_capacity = 10;
  char **arguments = calloc (arg_list_capacity, sizeof (char *));
  assert (arguments != NULL);

  /* Leave the first space blank for the command name */
  size_t arg_list_length = 1;

  while ((token = strtok (NULL, WHITESPACE)) != NULL)
    {
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
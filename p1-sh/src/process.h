#ifndef __cs361_process__
#define __cs361_process__

// The contents of this file are up to you, but they should be related to
// running separate processes. It is recommended that you have functions
// for:
//   - performing a $PATH lookup
//   - determining if a command is a built-in or executable
//   - running a single command in a second process
//   - running a pair of commands that are connected with a pipe
char *path_lookup (char *filename);

bool check_builtin (char *filename);

int exec_cmd (char *filename);

int pipe_cmd (char *prgm1, char *prgm2);

char *collapse_args (char **args, size_t argc);

#endif

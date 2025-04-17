The project consists of several source files:

commands.c
This file contains everything related to executing both internal and external commands. For the -L option of pwd and cd, we use the environment variable via getenv() because it follows symbolic links; by contrast, the -P option does not follow them, so we use getcwd(). In the special case where a cd path doesn’t make sense, our solution is to simplify the given path (resolving . and ..) and, if it then becomes valid, switch to it; otherwise we perform a physical interpretation of the path. For external commands, we build an array of C‑strings to hold the command name plus its parameters and options, and pass that to execvp(). The exit command relies on a global variable return_value to hold the exit status of the most recently executed command, which we also display in the prompt.

star.c
In this file we handle pattern‑expansion for * and **. We represent the file tree with a TreeNode data structure and adapted functions we found online (createNode(), freeTree(), traverseDFS()) to meet our needs.

get_match_dirs() collects all directories and files matching a given single‑* pattern, using a helper match() function to test each name, and inserts matches into the tree.

get_arborescence() walks a directory and all its subdirectories recursively, adding every file or directory name to the tree.

Using that tree, get_paths_dbl_star() finds every path matching a ** pattern, solving the double‑star expansion correctly.

redirection.c
This file manages all I/O redirections. First, we detect what kind of redirection is requested (stdin, stdout, or stderr). Then, based on that result, we determine the exact mode (with or without truncation, in append mode, etc.) before performing the redirection using dup2().

Pipeline handling
We separated pipeline logic out of redirection.c because the function that executes pipelines calls launch() from the main file, slash.c. Our approach is:

Split the pipeline into two parts—first, the very first command; second, the remainder of the pipeline.

Call launch() (see its description in slash.c) on the first part, which in turn re‑invokes the pipeline function on the second part, and so on until the pipeline is complete.
We coordinate all this with fork(), pipe(), and redirections of standard input/output.

Signal handling
In main() (in slash.c), we install SIG_IGN handlers for SIGINT and SIGTERM so the shell itself ignores them. When we execute an external command, we reset both signals to the default handler (SIG_DFL) in the child process so that the command isn’t ignored.
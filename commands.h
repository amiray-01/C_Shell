#ifndef COMMANDS_H
#define COMMANDS_H

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096


int pwd (char* option);
int cd(char * path,char * option);
int cmdExterne(char* cmd);
int execute(char* cmd,int return_value,int avec_star);
char* build_prompt(char *pwd,char*prompt,int return_value);
#endif 

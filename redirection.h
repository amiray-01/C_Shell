#ifndef REDIRECTION_H
#define REDIRECTION_H

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

int redirect(char* file, char* type);
char * get_first_token(char * cmd);
int parseCmd(char* cmd,int ret_value);
int exec_cmd_with_redirections(char* cmd,char* first_token,int return_value);
int exec_pipeline(char* ligne_cmd,int return_value);
#endif 

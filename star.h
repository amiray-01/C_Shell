#ifndef STAR_H
#define STAR_H

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
#define MAX_CHILDREN 4062
#define STRLEN  4062

// Structure de données pour un noeud d'arbre
struct TreeNode {
    char *data;
    struct TreeNode *children[MAX_CHILDREN];
};

int execStar(char* cmd,int return_value);
char ** get_paths(char * path);
int get_length_argument_list(char** res);
void print_argument_list(char** res);

#endif 

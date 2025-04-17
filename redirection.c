#include "redirection.h"
#include "commands.h"
#include "star.h"

/* redirige l'entrée ou la sortie de `file` selon `type` */
int redirect(char* file, char* type){
    int fd_input = -1;
    int fd_output = -1;
    if(strcmp(type,"2>>") == 0){
        fd_output = open(file, O_WRONLY | O_APPEND);
        fd_input = 2;
    }else if(strcmp(type,"2>|") == 0){
        fd_output = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        fd_input = 2;
    }else if(strcmp(type,"2>") == 0){
        fd_output = open(file, O_RDONLY);
        if(fd_output != -1){
            goto error;
        }
        fd_output = open(file, O_WRONLY | O_CREAT , 0666);
        fd_input = 2;
    }else if(strcmp(type,">>") == 0){
        fd_output = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
        fd_input = 1;
    }else if(strcmp(type,">|") == 0){
        fd_output = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        fd_input = 1;
    }else if(strcmp(type,">") == 0){
        fd_output = open(file, O_WRONLY);                                       /* Termine avec une erreur */
        if(fd_output != -1){                                                    /* si file existe déjà */
            goto error;
        }
        // close(fd_output);
        fd_output = open(file, O_WRONLY | O_CREAT , 0666);
        fd_input = 1;
    }else if(strcmp(type,"<") == 0){
        fd_input= open(file, O_RDONLY);
        if (fd_input == -1){
            goto error;
        }
        fd_output  = STDIN_FILENO;
        dup2(fd_input,STDIN_FILENO);
        return 0;

    }else if(strcmp(type,"|") == 0){
        goto error;
    }
    if(fd_output == -1){
        goto error;
    }
    dup2(fd_output,fd_input);       /* On fait la redirection si tout se passe bien */
    return 0;
    error:
        return 1;
}

/* Recupere `file` et fait les redirections selon `type` */
int execRedirection(char* cpycmd,char* type){
    char * token_start = strstr(cpycmd,type);   /* pointeur vers le premier rencontrer   */
    int ret_value = 0;
    char* file = NULL;
    char* file_space;
    char** args = NULL;
    char * file_redirection = NULL;
    file_space = strdup (token_start + strlen(type));   /* duplique le nom du `file` */
    if(file_space[0] != ' ') goto error;
    file = strtok(file_space," ");                      /* supprime les espaces present avant la nom du `file` */
    file_redirection = file;
    int free_index= 0;
    if(strchr(file,'*') != NULL){                       /* si `file` est un joker */
        args = get_paths(file);                  /* recupere tout les fichiers qui match ce joker */
        if(get_length_argument_list(args)>1){           /* affiche un message d'erreur */
            printf("Error: ambiguous redirect\n");      /* si il y en a plusieurs */
            goto error;
        }
        memset(file_redirection,'\0',strlen(file_redirection));
        file_redirection = strdup(args[0]);
        free_index = 0;
        while( args[free_index] != NULL){
            free(args[free_index]);
            free_index++;
        }
        free(args);
        ret_value = redirect(file_redirection,type);
        free(file_space);
        free(file_redirection);
        return ret_value;
    }
    else {
        ret_value = redirect(file_redirection,type);
        free(file_space);
        return ret_value;
    }
    error :
        free_index = 0;
        if(args != NULL){
            while( args[free_index] != NULL){
                free(args[free_index]);
                free_index++;
            }
        }
        if(args != NULL) free(args);
        if (file != NULL) free(file);
        return 1;
}

/* Retourne si c'est une redirection de stdin, stdout ou stderr */
char * get_first_token(char * cmd){
    int len_debut_redirect1 = 0;
    int len_debut_redirect2 = 0;
    int len_debut_redirect3 = 0;
    char * debut_redirect1 = NULL;
    char * debut_redirect2 = NULL;
    char * debut_redirect3 = NULL;

    debut_redirect1 = strstr(cmd,">");
    if (debut_redirect1 == NULL) len_debut_redirect1 = 0;
    else len_debut_redirect1 = strlen(debut_redirect1); 

    debut_redirect2 = strstr(cmd,"<");
    if (debut_redirect2 == NULL) len_debut_redirect2 = 0;
    else len_debut_redirect2 = strlen(debut_redirect2);

    debut_redirect3 = strstr(cmd,"2>");
    if (debut_redirect3 == NULL) len_debut_redirect3 = 0;
    else len_debut_redirect3 = strlen(debut_redirect3);

    if ( len_debut_redirect1 > len_debut_redirect2 && len_debut_redirect1 > len_debut_redirect3){
        return  strdup(">");
    }

    else if ( len_debut_redirect2 > len_debut_redirect1 && len_debut_redirect2 > len_debut_redirect3){
        return strdup("<");
    }

    else if ( len_debut_redirect3 > len_debut_redirect1 && len_debut_redirect3 > len_debut_redirect2){
        return strdup("2>");
    }

    else return NULL;
}

/* `cmd` represente toutes les redirections de la commande initial */
/* La fonction recupere les type de redirection un à un et les effectue */
int parseCmd(char* cmd,int ret_value){
    char *cpycmd = strdup(cmd);
    char* token =NULL;
    char * type_token = get_first_token(cmd);
    if (type_token == NULL) {
        free(type_token);
        free(cpycmd);
        return 0;
    }
    /* recupere le type exacte de la redirection */
    if (strcmp(type_token,">")==0){
        if(strstr(cmd,">>") != NULL){
            token = strdup(">>");
        }
        else if(strstr(cmd,">|") != NULL){
            token = strdup(">|");
        }
        else if(strstr(cmd,">") != NULL){
            token = strdup(">");
        }
    }
    else if (strcmp(type_token,"2>")==0){
        if(strstr(cmd,"2>>") != NULL){
            token = strdup("2>>");
        }
        else if(strstr(cmd,"2>|") != NULL){
            token = strdup("2>|");
        }
        else if(strstr(cmd,"2>") != NULL){
            token = strdup("2>");
        }
    }
    else if (strcmp(type_token,"<") ==0){
        token = strdup("<");
    }

    else {
        free(cpycmd);
        free(token);
        free(type_token);
        return 0;
    }
    ret_value = execRedirection(cpycmd,token);      /* effectue la redirection avec le premier type trouvé  */
    if (ret_value == 1) goto error;
    ret_value = parseCmd(strstr(cmd,token) + strlen(token),ret_value);  /* appel recursif en se positionnant après le premier type rencontrer */
    free(token);
    free(type_token);
    free(cpycmd);
    return ret_value;
    error:
        free(token);
        free(type_token);
        free(cpycmd);
        return 1;
}

/* sépare la commande à éxecuter et les redirections à effectuer */
int exec_cmd_with_redirections(char* cmd,char* first_token,int return_value){
      char* redirections = strdup(strstr(cmd,first_token));         /* se place dans la première redirection trouvé */
      int ret_value = parseCmd(redirections,0);                     /* effectue la redirection */
      (strstr(cmd,first_token))[0] = '\0';                          /* recupère la commande à executer */
      free(redirections);
      if (ret_value == 1) return 1;
      return execute(cmd,return_value,1);
}



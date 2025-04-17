#include "commands.h"
#include "star.h"


int type = 0;   /* utilisé pour savoir si une execution a été interrompu par un signal */

/*fonction qui construit le prompt*/
char* build_prompt(char *pwd,char*prompt,int return_value){
    memset(prompt,'\0',((sizeof(char)*67)+1));
    if(type == 0){  //dans le cas ou le prompt est normal
       int size = strlen(pwd);
       if(size>25){   /* si le chemin est trop long on affiche seulement les 22 derniers caracteres */
        if(return_value == 0){  /* si le code de retour est 0 on affiche le prompt en vert sinon en rouge */
          sprintf(prompt,"\033[32m[%d]\033[00m\033[36m...%s\033[00m$ ",return_value,pwd+size-22);
        }
        else{
          sprintf(prompt,"\033[91m[%d]\033[00m\033[36m...%s\033[00m$ ",return_value,pwd+size-22);
        }
       }
       else{ /* sinon on affiche le chemin complet et on fait la meme chose  */
        if(return_value == 0){
          sprintf(prompt,"\033[32m[%d]\033[00m\033[36m%s\033[00m$ ",return_value,pwd);
        }
        else {
          sprintf(prompt,"\033[91m[%d]\033[00m\033[36m%s\033[00m$ ",return_value,pwd);
        }
       }
       return prompt;
    }
    else{ //dans la commande précédente on a envoyé un signal a une commande externe donc on affiche un prompt spécial
      int size = strlen(pwd);
       if(size>22){
        sprintf(prompt,"\033[91m[SIG]\033[00m\033[36m...%s\033[00m$ ",pwd+size-20);
       }else{
        sprintf(prompt,"\033[91m[SIG]\033[00m\033[36m%s\033[00m$ ",pwd);
       }
       return prompt;
    }
    
}

/*fonction qui test si la chaine s est une option*/
int isOption(char *s){
    if(strcmp(s,"-P") == 0 || strcmp(s,"-L") == 0) 
        return 1;
    return 0;
}

/* construit un path (chaine de caracteres) */
int push_string(char* buffer, char* str) { 
  size_t s1 = strlen(str), buf_size = strlen(buffer);
  if(buf_size + s1 + 1 >= MAX_ARGS_STRLEN) { 
    perror("Pas assez de place dans le buffer");
    return 1;
  }
  memmove(buffer + s1 + 1, buffer, buf_size); 
  buffer[0] = '/'; 
  memmove(buffer + 1, str, s1); 
  return 0;
}


int pwd(char* option){
  char* path = malloc(MAX_ARGS_STRLEN);
  if(path == NULL) goto error;
  memset(path,'\0',MAX_ARGS_STRLEN);
  if(path == NULL) goto error;
  if(option == NULL) {
    free(path);                   /* cas ou pwd est appelé sans option */
    return pwd("-L");             /* donc elle est intérprétée avec -L */
  }
  if(strcmp(option,"-P") == 0){                     /* cas ou pwd est appelé avec -P */
    //on recupere le chemin courant avec getcwd
    if(getcwd(path,MAX_ARGS_STRLEN) == NULL) goto error;
    strcat(path,"\n");
    write(1,path,strlen(path));
    free(path);
    return 0;
  }
  else if (strcmp(option,"-L") == 0){         /* cas ou pwd est appelé avec -L */
    //on recupere le chemin courant avec la variable d'environnement PWD dans le cas ou on doit suivre les liens symboliques (-L)
    char *pwd = getenv("PWD");
    memcpy(path,pwd,strlen(pwd)+1);
    strcat(path,"\n");
    write(1,path,strlen(path));
    free(path);
    return 0;
  }
  else if(option[0] == '-') goto error;     /* on est dans le cas ou c'est une option invalide */
  else {
    free(path);                     /* cas ou pwd (sans option) est suivie  */
    return pwd("-L");               /* de n'importe quoi (ex: pwd abcdefgh) */
  }
  error:
    free(path);
    return 1;
}

/* swap de deux string */
void swap(char **str1_ptr, char **str2_ptr){
  char *temp = *str1_ptr;
  *str1_ptr = *str2_ptr;
  *str2_ptr = temp;
}

/* Renvoie le path inverser */
char* reversePath(char *path){
  char* path2 = malloc(MAX_ARGS_STRLEN);
  char* newPath = malloc(sizeof(char)*MAX_ARGS_STRLEN);
  if (newPath == NULL || path2 == NULL) goto error;
  memset(path2,'\0',MAX_ARGS_STRLEN);
  strcpy(path2,path);
  memset(newPath,'\0',MAX_ARGS_STRLEN);
  char* str = strtok(path2,"/");
  while(str != NULL){ 
    push_string(newPath,str);
    str = strtok(NULL,"/");
  }
  free(path2);
  memset(path,'\0',MAX_ARGS_STRLEN);
  strcpy(path,newPath);
  free(newPath);
  return path;
  error:
    free(path2);
    free(newPath);
    return NULL;
}

/* renvoie l'interprétation logique de path */
char* clearPath(char *path){
  char* path2 = malloc(sizeof(char)*MAX_ARGS_STRLEN);
  char* newPath = malloc(sizeof(char)*MAX_ARGS_STRLEN);
  if (newPath == NULL || path2 == NULL) goto error;
  memset(path2,'\0',MAX_ARGS_STRLEN);
  //on recupere l'inverse du path car plus simple pour le parcourir
  strcpy(path2,reversePath(path));
  memset(newPath,'\0',MAX_ARGS_STRLEN);
  char* str = strtok(path2,"/");
  int nbDelElement = 0;
  while(str != NULL){
    if(strcmp(str,"..") == 0){ //si on rencontre .. on incremente le nombre d'element a supprimer
      nbDelElement++;
      str = strtok(NULL,"/");
    }
    else if(strcmp(str,".") == 0){ //si on rencontre . on ne fait rien
      str = strtok(NULL,"/");
    }
    else{
      if(nbDelElement == 0){ //si le nb d'element à suppriemr on ajoute l'element au path
        push_string(newPath,str);
        str = strtok(NULL,"/");
      } 
      else{ //si on a rencontre .. on decremente le nombre d'element a supprimer et on passe a l'element suivant
        if(nbDelElement>0) {
          nbDelElement--;
          str = strtok(NULL,"/");
	}
      }
    }
  }
  if(strlen(newPath) == 0){ //si le path est vide on ajoute un /
    newPath[0] = '/';
  }
  free(path2);
  memset(path,'\0',MAX_ARGS_STRLEN);
  strcpy(path,newPath);
  free(newPath);
  return path;
  error:
    free(path2);
    free(newPath);
    return NULL;
}

int cd (char * path,char * option){
  char *oldpwd = malloc(MAX_ARGS_STRLEN);
  char *pwd = malloc(MAX_ARGS_STRLEN);
  if (pwd == NULL || oldpwd == NULL) goto error;
  memset(oldpwd,'\0',MAX_ARGS_STRLEN);
  strcat(oldpwd,getenv("OLDPWD"));
  memset(pwd,'\0',MAX_ARGS_STRLEN);
  strcat(pwd,getenv("PWD"));  
  /*ici on a le pwd qui est set et le old pwd aussi*/
  if(strcmp(option , "-P") == 0){     /*cd avec l'option -P*/
    if(path == NULL){     /*cd sans argument*/
      if(chdir(getenv("HOME"))<0){  // il doit le renvoyer a HOME
        goto error;
      }
      swap(&oldpwd,&pwd); // on set old pwd à pwd et on met à jour pwd
      memset(pwd,'\0',MAX_ARGS_STRLEN);
      strcat(pwd ,getenv("HOME"));
    }
    else if(strcmp(path,"-") == 0){  /* cas ou l'argument est un - */
      if(chdir(oldpwd)<0){
        goto error;
      }
      swap(&oldpwd,&pwd);  //on swap oldpwd avec pwd
      memset(pwd,'\0',MAX_ARGS_STRLEN);
      if(getcwd(pwd,MAX_ARGS_STRLEN)==NULL) goto error;
    }
    else{                           /*cas ou c'est un path */
      if(chdir(path)<0) goto error;
      swap(&oldpwd,&pwd);   
      memset(pwd,'\0',MAX_ARGS_STRLEN);
      if(getcwd(pwd,MAX_ARGS_STRLEN)==NULL) goto error; //on set pwd à la valeur de getcwd
    }
  }
  else if(strcmp(option , "-L") == 0 || strcmp(option,"") == 0){     /*cd avec l'option -L ou sans option*/
    if(path == NULL){    /*cd sans argument*/
      if(chdir(getenv("HOME"))<0){  // il doit le renvoyer a HOME
        goto error;
      }
      swap(&oldpwd,&pwd);
      memset(pwd,'\0',MAX_ARGS_STRLEN);
      strcat(pwd,getenv("HOME")); //on set pwd à la valeur de HOME
    }
    else if(strcmp(path,"-") == 0){   
      if(chdir(oldpwd)<0){
        goto error;
      }
      swap(&oldpwd,&pwd);    //on swap oldpwd avec pwd sans utiliser getcwd afin de suivre les liens symboliques
    }
    else{
      if(path[0] == '/'){                     /* si path est un chemin absolue */
        swap(&oldpwd,&pwd);
        memset(pwd,'\0',MAX_ARGS_STRLEN);
        char* copy2path = malloc(sizeof(char)*MAX_ARGS_NUMBER);
        memset(copy2path,'\0',MAX_ARGS_STRLEN);
        strcat(copy2path,path);           // on gere le cas ou quand le path n'a pas de sens => interprétation physique
        strcat(pwd,clearPath(path));      // et c'est pour cela qu'on utilise clearPath
        if(chdir(pwd)<0){
          int ret_val =  cd(copy2path,"-P");
          free(pwd);
          free(oldpwd);
          free(copy2path);
          return ret_val;
        }
        free(copy2path);
      }
      else{
        memset(oldpwd,0,MAX_ARGS_STRLEN);     
        strcpy(oldpwd,pwd);                     
        strcat(pwd,"/");                       /* construit le chemin */
        strcat(pwd,path);                       /* absolue de path */
        pwd = clearPath(pwd);
        char* getpwd = malloc(MAX_ARGS_STRLEN);
        if(getpwd == NULL) { 
          free(getpwd);
          goto error;
        }
        memset(getpwd,'\0',MAX_ARGS_STRLEN);
        if(getcwd(getpwd,MAX_ARGS_STRLEN)==NULL) goto error;      /* construit le chemin */
        strcat(getpwd,"/");                                       /* logique de path stocké */
        strcat(getpwd,path);                                      /* dans getpwd*/
        getpwd = clearPath(getpwd);
        if(chdir(pwd)<0){                         /* si pwd n'a pas de sens */
          if(chdir(getpwd) < 0) {
            free(getpwd);
            goto error;
          }
          int ret_val =  cd(getpwd,"-P");
          free(getpwd);
          free(pwd);
          free(oldpwd);
          return ret_val;
        }
        free(getpwd);
      } 
    }
  }
  setenv("PWD",pwd,1);              /* met à jour les variables d'environnement */
  setenv("OLDPWD",oldpwd,1);
  free(pwd);
  free(oldpwd);
  return 0;
  error:
    free(pwd);
    free(oldpwd);
    return 1;
}

int cmdExterne(char* cmd){   /* execute une commande externe */
  type = 0;
  int ret_val = 0;
  char* cpycmd = strdup(cmd);
  char* cpy2cmd = cpycmd;
  char* argument_list[MAX_ARGS_NUMBER];
  int i =0;
  char* res = strtok(cpycmd," ");
  argument_list[i] = res;    
  while(res!=NULL){   /*on set la liste des arguments de la commande */
    i++;
    res = strtok(NULL," ");
    argument_list[i] = res;
  }
  pid_t child_pid = fork();
  if(child_pid == -1) goto error;
  if(child_pid == 0){                             /*dans le fils on set le handler a default */
      struct sigaction sa = {0};          /*afin que la commande externe n'ignore pas les signaux sigInt et sigterm*/
      sa.sa_handler = SIG_DFL;      
      sigaction(SIGINT,&sa,NULL);
      sigaction(SIGTERM,&sa,NULL);
      execvp(argument_list[0],argument_list);   //on execute la commande externe avec ses arguments
      exit(127);
  }
  else{
    int status=0;
    wait(&status);            //on attend la fin de l'execution de la commande externe
    if(WIFEXITED(status)){              
      ret_val = WEXITSTATUS(status);      //on retourne la valeur que la commande externe a retournée
    }

     if(WIFSIGNALED(status)){   //si la commande externe a été tuée par un signal on set le type à 1 pour adapter le prompt et on retourne 255
        type = 1;
        ret_val = 255;
      }
    
  }

  free(cpy2cmd);
  return ret_val;
  error:
      return 1;
}

int execute(char* cmd,int return_value,int avec_star){   /* execute une commande interne ou externe */
    type = 0;
    int ret_value = return_value;
    char cpycmd[MAX_ARGS_STRLEN];
    memset(cpycmd,'\0',sizeof(char)*strlen(cmd)+1);
    strcpy(cpycmd,cmd);
    char* res = strtok(cpycmd," ");
    if(res == NULL) {
        return ret_value;
    }

    if(strcmp(res,"cd") == 0){
        char *arg1 = strtok(NULL," ");
        char *arg2 = strtok(NULL," ");
        if(arg1 == NULL){                   /* cd sans argument (sans path et sans option) */
            ret_value = cd(NULL,"");          
        }
        else if(!isOption(arg1)){           /* cd sans option mais avec un path */
              ret_value = cd(arg1,"");
        }
        else{
            ret_value = cd(arg2,arg1);      /* cd avec option et path */
        }
    }else if(strcmp(res,"pwd") == 0){
        char *arg1 = strtok(NULL," ");
        ret_value = pwd(arg1);              /* cd avec potentiellement l'option */
    }else if(strcmp(res,"exit") == 0){
        res = strtok(NULL," ");
        if(res==NULL){                      /* exit sans argument */
              exit(return_value);
        }
        int to_int= atoi(res);
        if(to_int == 0 ){
            if(strcmp(res,"0") == 0){       /* exit avec 0 */
                exit(0);
            }else {
                exit(return_value);         /* exit avec la valeur de retour de l'ancienne */
            }                               /* commande executé (ex de ce cas: exit 0) */
        }
        exit(to_int);                       /* exit avec la valeur donnée */
        }
    else{   
        if(strchr(cmd,'*') == NULL || avec_star == 0){  /* si la commande ne contient pas de *  */
            ret_value = cmdExterne(cmd);
        }
        else{
            ret_value = execStar(cmd,return_value);       /* si la commande contient un * */
        }
    }
    return ret_value;
}

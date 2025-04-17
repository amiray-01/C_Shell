#include "commands.h"
#include "redirection.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

int return_value;

int exec_pipe(char * cmd,int return_value);

/* execute la commande de l'input */
int launch (char * cmd){
      if(cmd == NULL){              /* aucune commande taper */
            exit(return_value);
      }
      if (strlen(cmd) == 0){
            return return_value;
      }
      int ret_value = 0;
      char* first_token = get_first_token(cmd);
      if((strstr(cmd," | ") != NULL ) ){                          /* l'input est une pipeline */
            ret_value = exec_pipe(cmd,return_value);
      }
      else if (first_token != NULL) {                             /* l'input est une redirection */
           ret_value = exec_cmd_with_redirections(cmd,first_token,return_value);
      }
      else{                                                       /* l'input une commande ordinaire */
            ret_value = execute(cmd,return_value,1);
      }       
      free(first_token); 
      return ret_value;      
      error :
            free(first_token);
            return 1;
}

/* execute la pipeline */
int exec_pipe(char * cmd,int return_value){
      char* cmd1 = strdup(cmd);
      char* cmd2 = NULL;
      char* tmp;
      if(strstr(cmd1," | ") != NULL){                 /* test si on est arriver à la fin de la pipeline */
            strstr(cmd1," | ")[0] = '\0';             /* cmd1 contient une commande de la pipeline à executer */
            tmp = strstr(cmd, " | ") + strlen(" | ");
            cmd2 = strdup(tmp);                       /* cmd2 contient le reste de la pipeline */
      }
      else{cmd2 = NULL; tmp = NULL;}

      int pipe_fd[2];
      if(pipe(pipe_fd) == -1 ){
            goto error;
      }
      int pid = fork();
      if (pid == -1){
            goto error;
      }
      if (pid == 0){
            close(pipe_fd[0]);
            dup2(pipe_fd[1],1);
            return_value = launch(cmd1);              /* execute la commande `cmd1` */
            close(pipe_fd[1]);
            free(cmd1);
            if(cmd2 != NULL) free(cmd2);
            exit(return_value);
      }
      else{
            if (strlen(cmd2) != 0){
                  close(pipe_fd[1]);
                  dup2(pipe_fd[0],0);
                  return_value = launch(cmd2);        /* execute la(les) commande(s) `cmd2` */
                  close(pipe_fd[0]);
            }
      }
      wait(NULL);
      free(cmd1);
      free(cmd2);
      return return_value;
      error:
            free(cmd1);
            if (cmd2 != NULL) free(cmd2);
            return 1;
}

int main (){
      struct sigaction sa = {0};
      sa.sa_handler = SIG_IGN;
      sigaction(SIGINT,&sa,NULL);
      sigaction(SIGTERM,&sa,NULL);
      char* input;
      rl_outstream = stderr;
      char* prompt = malloc((sizeof(char)*67)+1);
      setenv("PWD",getenv("PWD"),1);
      if(prompt == NULL) EXIT_FAILURE;
      while(1){
            input = readline(build_prompt(getenv("PWD"),prompt,return_value));
            add_history(input);
            int stdin_copy = dup(0);
            int stdout_copy = dup(1);
            int stderr_copy = dup(2);
            return_value = launch(input);
            dup2(stdin_copy,0);
            dup2(stdout_copy,1);
            dup2(stderr_copy,2);
            close(stdin_copy);
            close(stdout_copy);
            close(stderr_copy);
            free(input);
      }
      free(prompt);
}

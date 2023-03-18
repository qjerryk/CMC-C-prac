#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define BUFFER 1000


char *read_command(int *is_end){
    int cnt = 0, curr_buff_size = BUFFER;
    char c;
    int quotes_flag = 0;
    int apo_flag = 0;
    char *command = (char*)calloc(curr_buff_size, sizeof(char));
    if (!command){
        *is_end = 1;
        return command;
    }
    while (((c = getchar()) != ' ' || quotes_flag || apo_flag ) && (c != '\t' || quotes_flag || apo_flag) && (c != '|' || quotes_flag || apo_flag) && (c != EOF) && (c != '\n')){
        if (c == '"' && !quotes_flag){//double quotes
            quotes_flag = 1;
            continue;
        }else if (c == '"' && quotes_flag){
            quotes_flag = 0;
           // c = getchar();
            continue;
        }
        if (c == '\'' && !apo_flag){
            apo_flag = 1;
            continue;
        }else if (c == '\'' && apo_flag){
            apo_flag = 0;
            continue;
            //break;
        }
        if (cnt >= curr_buff_size){
            curr_buff_size+=BUFFER;
            command = (char*)realloc(command, curr_buff_size);
            if (command == NULL){
                fprintf(stderr, "Not enough memory");
                exit(1);
            }
        }
        (command)[cnt] = c;
        cnt++;
    }

    if (c == '|' ){
        *is_end = 1;
    }

    if (cnt == curr_buff_size){
        ++curr_buff_size;
        command = (char*)realloc(command, curr_buff_size);
        if (command == NULL){
            fprintf(stderr, "Not  enough memory");
            exit(1);
        }
    }
    (command)[cnt] = '\0';

    if (c == '\n' && (apo_flag || quotes_flag)){
        fprintf(stderr, "Syntax error\n");
        exit(0);
    }

    if (c == EOF || command == NULL || c == '\n'){
        *is_end = 2;
    }

    return command;
}

char **get_argv(int *argc, int *is_read){
    int cnt = 0, curr_buff_size = BUFFER;
    int is_end = 0;
    *argc = 0;
    char **argv = (char**)calloc(BUFFER, sizeof(char*));
    for (;;){
        if (cnt >= curr_buff_size){
            curr_buff_size+=BUFFER;
            argv = (char**)realloc(argv, sizeof(char*));
            if (!argv){
                return NULL;
            }
        }
        argv[cnt] = read_command(&is_end);
        if (argv[cnt] && (argv[cnt][0] == '\0' || !strlen(argv[cnt]))){
            free(argv[cnt]);
            --cnt;
        }
        if (is_end){
            *is_read = is_end;
            cnt+=1;
            (*argc)++;
            break;
        }
        cnt++;
        *argc = cnt;
    }
    argv[cnt] = NULL;
    return argv;
}
    

void convey(void){
    pid_t pid;
    char **argv = NULL;
    int argc = 0, status;

    int is_read = 0;
    argv = get_argv(&argc, &is_read);
    if (argv == NULL || is_read == 2) {
        fprintf(stderr, "Syntax error\n");
    }
    int **fd = (int **) calloc(2, sizeof(int *));

      if (!fd){
          perror("Not enough memory");
          exit(0);
      }

      fd[0] = (int*) calloc(2, sizeof(int));
      fd[1] = (int*) calloc(2, sizeof(int));

      if (pipe(fd[0]) == -1){
          perror("pipe");
          exit(0);
      }
      if ((pid = fork()) == -1){
          perror("fork");
          exit(0);
      }
      if (!pid){//first execute
          if (is_read != 2) {
              dup2(fd[0][1], 1);
              close(fd[0][1]);
          }
          if (argv[0] == NULL || argv[0][0] == '\0'){
              fprintf(stderr, "Syntax error\n");
              exit(-1);
          }
          execvp(argv[0], argv);
          fprintf(stderr, "Command %s not found\n", argv[0]);
          exit(-1);
      }// father
      waitpid(pid, &status, 0);
      if (WEXITSTATUS(status)){
          exit(-1);
      }
      if (pipe(fd[1]) == -1){
           perror("pipe");
           exit(-1);
      }
      argv = get_argv(&argc, &is_read);
      for (pid_t new_pid = fork();;){
          if (new_pid < 0){
              perror("fork");
              exit(-1);
          }
          if (!new_pid){//son
              dup2(fd[0][0], 0);
              if (is_read != 2) {
                  dup2(fd[1][1], 1);
              }
              close(fd[0][1]);
              close(fd[0][0]);
              close(fd[1][1]);
              close(fd[1][0]);
              if (argv[0] == NULL || argv[0][0] == '\0'){
                  fprintf(stderr, "Syntax error\n");
                  exit(-1);
              }
              execvp(argv[0], argv);
              fprintf(stderr, "Command %s not found\n", argv[0]);
              exit(-1);
          }
          // father
          close(fd[0][0]);
          close(fd[0][1]);
          waitpid(new_pid, &status, 0);
          if (WEXITSTATUS(status)) {
              close(fd[0][1]);
              close(fd[0][0]);
              close(fd[1][1]);
              close(fd[1][0]);
              exit(0);
          }
          fd[0][0] = fd[1][0];
          fd[0][1] = fd[1][1];

          if (argv == NULL || is_read == 2) {
              break;
          }
          argv = get_argv(&argc, &is_read);
          if (argv == NULL){
              exit(0);
          }
          if (pipe(fd[1]) == -1){
              perror("pipe");
              exit(-1);
          }
          new_pid = fork();

      }
}

int main(){
    convey();
    return 0;
    
}
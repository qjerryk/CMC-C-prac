#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define BUFFER 1000


char read_word(char **word, int *quot_se){
    int cnt = 0, curr_buff_size = BUFFER;
    char c;
    int quotes_flag = 0;
    int apo_flag = 0;
    while (((c = getchar()) != ' ' || quotes_flag || apo_flag ) && (c != '\t' || quotes_flag || apo_flag) && (c != '|' || quotes_flag || apo_flag) && (c != EOF) && (c != '\n')
    && (c != '<' || quotes_flag || apo_flag) && (c != '>' || quotes_flag || apo_flag)){
        if (c == '"' && !quotes_flag){//double quotes
            quotes_flag = 1;
            continue;
        }else if (c == '"' && quotes_flag){
            quotes_flag = 0;
           // c = getchar();
            break;
        }
        if (c == '\'' && !apo_flag){
            apo_flag = 1;
            continue;
        }else if (c == '\'' && apo_flag){
            apo_flag = 0;
          //  c = getchar();
            break;
        }
        if (cnt >= curr_buff_size){
            curr_buff_size+=BUFFER;
            *word = (char*)realloc(*word, curr_buff_size);
            if (word == NULL){
                fprintf(stderr, "Not enough memory");
                exit(1);
            }
        }
        (*word)[cnt] = c;
        cnt++;
    }

    // printf("%c\n", c);
    if (cnt == curr_buff_size){
        ++curr_buff_size;
        *word = (char*)realloc(*word, curr_buff_size);
        if (word == NULL){
            fprintf(stderr, "Not  enough memory");
            exit(1);
        }
    }
    (*word)[cnt] = '\0';

    if (c == '\n' && (apo_flag || quotes_flag)){
        *quot_se = 1;
        //fprintf(stderr, "Syntax error\n");
        //exit(0);
    }
    return c;
}

int get_argv(char*** argv, int *quot_se){
    int curr_buff_size = BUFFER, cnt = 0;
    char is_sep;
    *quot_se = 0;
    for (;;){
        //printf("cnt = %d\n", cnt);
        if (cnt >= curr_buff_size){
            curr_buff_size+=BUFFER;
            *argv = (char**)realloc(*argv, curr_buff_size);
            if (*argv == NULL){
                fprintf(stderr, "Not enough memory\n");
                exit(0);
            }
        }
        (*argv)[cnt]=(char*)calloc(curr_buff_size, sizeof(char));
        is_sep = read_word(&((*argv)[cnt]), quot_se);
        if (*quot_se){
            if (!cnt){
                fprintf(stderr, "Syntax error\n");
                exit(0);
            }
            break;
        }
        /*   if (!strcmp((*argv)[cnt], "wc")){
               printf("----%c\n", is_sep);
           }*/
        if (strlen((*argv)[cnt]) == 0 && is_sep != '\n' && is_sep != EOF){
            free((*argv)[cnt]);
            cnt--;
            //break;
        }
        if (is_sep == '|' || is_sep == '<' || is_sep == '>'){
            cnt++;
            (*argv)[cnt] = (char*)malloc(2*sizeof(char));
            switch (is_sep) {
                case '|':
                    (*argv)[cnt] = "|";
                    break;
                case '<':
                    (*argv)[cnt] = "<";
                    break;
                case '>':
                    (*argv)[cnt] = ">";
                default:
                    ;
            }

        }
        if (is_sep == EOF){
            break;
        }
        cnt++;
    }
/*
    for (int i = 0; i < cnt; i++){
        fflush(stdout);
        printf("- %s\n", (*argv)[i]);
    }*/
    return cnt;

}


int get_commands_number(int argc, char **argv){
    int commands_number = 1;
    int i;
    for (i = 0; i < argc; ++i){
        if (!strcmp("|", argv[i])){
            ++commands_number;
        }
    }
    return commands_number;
}


void get_separators_ind(char **argv, int **seps, int argc, int commands_number){
    *seps = (int*)calloc(commands_number, sizeof(int));
    if (!(*seps)){
        fprintf(stderr, "Error");
        exit(0);
    }
    (*seps)[0] = -1;
    int i = 1, ind;
    for(ind = 0; i < commands_number && ind < argc; ++ind){
        if (!strcmp("|", argv[ind])){
            (*seps)[i]=ind;
            ++i;
        }
    }
    /* if (i != commands_number){
         fprintf(stderr, "Wrong input");
         exit(0);
     }*/
}

int syntax_checker(char **argv, int argc){
    int ind, is_div = 0;
    for (ind = 0; ind < argc; ind++){
        if (strstr(argv[ind], "||")){
            return 1;
        }
    }
    if (!strcmp(argv[0], "|")){
        fprintf(stderr, "Syntax error\n");
        exit(0);
    }
    /*  for (ind = 0; ind < argc; ind++){
          if (!strcmp(argv[ind], "|")){
              is_div = 1;
              *lst_se = ind;
              continue;
          }
          if (strlen(argv[ind])){
              is_div = 0;
          }
      }
      if (is_div){
          *lst_se;
          //fprintf(stderr, "Syntax error\n");
          //exit(0);
      }*/

    if (argc == 0){
        return 1;
    }
    for (ind = 1; ind < argc; ind++){
        if (!strcmp(argv[ind], "|") && !strcmp(argv[ind-1], "|")) {
            // fprintf(stderr, "Syntax error\n");
            //exit(0);
        }
    }
    return 0;
}




void free_argv(char ***argv, int argc){
    for (int j = 0; j < argc; j++){
        free((*argv)[j]);
    }
    free(*argv);
}

void close_fds(int N, int ***fd){
    int i;
    for (i = 0; i < N; ++i){
        close((*fd)[i][0]);
        close((*fd)[i][1]);
    }
}


int main(void) {
    int *separators; // array for indexes for separators
    char **argv = (char **) calloc(BUFFER, sizeof(char *));
    int quot_se = 0;
    int argc = get_argv(&argv, &quot_se); // get argv from stdio
    int commands_number = get_commands_number(argc, argv);
    get_separators_ind(argv, &separators, argc, commands_number); // array for separators to know where the end of the command

    if (syntax_checker(argv, argc)){
        fprintf(stderr, "Syntax error\n");
        free_argv(&argv, argc);
        return 1;
    }
     for (int i = 0; i < argc; i++) {
           fflush(stdout);
           printf("%s\n", argv[i]);
     }
    /* for (int i = 0; i < commands_number; i++){
          printf("%d\n", separators[i]);
      }*/
  //  return 0;
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    int **fd = (int **) calloc(commands_number-1, sizeof(int *)); // file descriptors
    for (int i = 0; i < commands_number-1; ++i) {
        fd[i] = (int *) calloc(2, sizeof(int));
    }
    if (fd == NULL) {
        fprintf(stderr, "Error");
        return 1;
    }

    /* for (int i = 0; i < commands_number-1; ++i) { // making pipelines
         if (pipe(fd[i]) == -1) {
             for (int j = 0; j <= i; ++j) {
                 close(fd[j][0]);
                 close(fd[j][1]);
             }
             fprintf(stderr, "Error");
             return 1;
         }
     }*/
    if (commands_number - 1 > 0)
        if (pipe(fd[0]) == -1){
            fprintf(stderr, "Pipe error");
        }
    for (int i = 1; i < commands_number; i++) { // commands_number - 1 is separators_number
        //free(argv[separators[i]]);
        argv[separators[i]] = NULL;   // we need to insert null for execvp
    }
    argv[argc] = NULL;
    //printf("separators %p", argv[separators[1]]);

    if (commands_number == 1){
        execvp(argv[0], argv);
        fprintf(stderr, "Command %s not found\n", argv[0]);
        free_argv(&argv, argc);
        return 0;
    }

    int ind, pid;


    if ((pid = fork()) < 0) {
        fprintf(stderr, "Error");
        for (int j  = 0; j < commands_number - 1; j++){
            close(fd[j][0]);
            close(fd[j][1]);
        }
        free_argv(&argv, argc);
        return 1;
    }
    if (!pid) { // son and the first execute
        dup2(fd[0][1], 1);
        for (int j = 0; j < commands_number-1; j++) {
            close(fd[j][0]);
            close(fd[j][1]);
        }
        //  fprintf(stderr, "GGGGGGG\n");
        if (argv[separators[0]+1] == NULL || !strcmp(argv[separators[0]+1], "\"") || !strcmp(argv[separators[0]+1], "\'")){
            fprintf(stderr, "Syntax error\n");
            exit(-1);
        }
        for (int j = 0; argv[separators[0]+1+j] != NULL; j++){
            if (argv[separators[0]+1+j][strlen(argv[separators[0]+1+j])-1] == '\'' || argv[separators[0]+1+j][strlen(argv[separators[0]+1+j])-1] == '\"'){
                fprintf(stderr, "Syntax error\n");
                exit(0);
            }
        }
        execvp(argv[separators[0] + 1], argv + separators[0] + 1);
        if (!strcmp(argv[separators[0]+1], " ") || !strcmp(argv[separators[0]+1], "\n") || !strcmp(argv[separators[0]+1], "")){
            fprintf(stderr, "Syntax error\n");
            exit(-1);
        }
        fprintf(stderr, "Command %s not found\n", argv[separators[ind] + 1]);
        free_argv(&argv, argc);
        exit(-1);
    }
    if (pid) { // the father
        int status;
        wait(&status);
        if (WEXITSTATUS(status)){
            free_argv(&argv, argc);
            close(fd[0][0]);
            close(fd[0][1]);
            return 1;
        }
        if (commands_number - 1 >= 2)
            if(pipe(fd[1]) == -1){
                for (int j = 0; j < 2; j++){
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
                fprintf(stderr, "Pipe error");
                return 0;
            }
        int new_pid;
        for (ind = 1, new_pid = fork(); ind < commands_number;  ++ind) {
            // fflush(stdout);
            // printf("%d", ind);
            if (new_pid < 0) {
                fprintf(stderr, "Error");
                for (int j = 0; j < ind; j++){
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
                free_argv(&argv, argc);
                return 1;
            }
            if (!new_pid ) {
                //printf("%d\n", getpid());
                dup2(fd[ind - 1][0], 0);
                if (ind != commands_number - 1){

                    //fprintf(stderr, "Yoy\n");
                    dup2(fd[ind][1], 1);
                    close(fd[ind][1]);
                }

                //for (int j = 0; j < ind-1; ++j) {
                close(fd[ind-1][0]);
                close(fd[ind-1][1]);

                //}
                //            fprintf(stderr,"GETPID = %d\n", getpid());
                //   fprintf(stderr,"%s\n", argv[separators[ind]+1]);
                // fflush(stderr);
                if (quot_se || argv[separators[ind]+1] == NULL || !strcmp(argv[separators[ind]+1], "\"") || !strcmp(argv[separators[ind]+1], "\'")){
                    fprintf(stderr, "Syntax error\n");
                    exit(-1);
                }
                for (int j = 0; argv[separators[ind]+1+j] != NULL; j++){
                    if (!strcmp(argv[separators[ind]+1+j], "\"") || !strcmp(argv[separators[ind]+1+j], "\'")){
                        fprintf(stderr, "Syntax error\n");
                        exit(-1);
                    }
                }
                execvp(argv[separators[ind] + 1], argv + separators[ind] + 1);
                if (!strcmp(argv[separators[ind]+1], " ") || !strcmp(argv[separators[ind]+1], "\n") || !strcmp(argv[separators[ind]+1], "")){
                    fprintf(stderr, "Syntax error\n");
                    exit(-1);
                }
                fprintf(stderr, "Command %s not found\n", argv[separators[ind] + 1]);
                exit(4);

                free_argv(&argv, argc);
                return 0;
                //need to add обработку ошибок
                //printf("Error opa\n");
                // exit(0);
            }

            int status;
            //for (int j = 0; j < ind-1; ++j){
            close(fd[ind-1][0]);
            close(fd[ind-1][1]);

            waitpid(new_pid, &status, 0);
            if (WEXITSTATUS(status)) {
                return 0;
            }
            if (ind + 1 != commands_number - 1) {
                //  pipe(fd[ind]);
                pipe(fd[ind + 1]);
            }
            new_pid = fork();
        }
    }

    return 0;
}
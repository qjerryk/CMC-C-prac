

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>



void separ_array(char **argv, int **seps, int argc){
    *seps = (int*)calloc(2, sizeof(int));
    if (!(*seps)){
        fprintf(stderr, "Error");
        exit(0);
    }
    int i = 0, ind;
    for(ind = 0;i<2 && ind < argc;++ind){
        if (!strcmp("|", argv[ind])){
            (*seps)[i]=ind;
            ++i;
        }
    }
    if (i != 2){
        fprintf(stderr, "Wrong input");
        exit(0);
    }
}


int main(int argc, char **argv){
    int *separators;
    separ_array(argv, &separators, argc);
    for (int i = 0; i < 2; i++){
        argv[separators[i]]=NULL;
    }
    int fd1[2];// fd[0] - read fd[1] - write
    int fd2[2];
    int pid;
    if (pipe(fd1) == -1){
        fprintf(stderr, "Can't create pipe");
        return 1;
    }
    if (pipe(fd2) == -1){
        fprintf(stderr, "Can't create pipe");
        close(fd1[0]);
        close(fd1[1]);
        return 1;
    }
    if ((pid = fork()) < 0){
        close(fd1[0]);
        close(fd2[0]);
        close(fd1[1]);
        close(fd2[1]);
        fprintf(stderr, "Error");
        return 1;
    }
    if (!pid){//son
        dup2(fd1[1], 1); // write to stdout
        close(fd1[1]);//fd[1] is useless now because we use 0
        close(fd1[0]);
        close(fd2[0]);
        close(fd2[1]);
        execvp(argv[1], argv+1); // the first exec
    }else {//father
        wait(NULL);
        pid = fork();
        if (pid < 0) {
            close(fd1[0]);
            close(fd2[0]);
            close(fd1[1]);
            close(fd2[1]);
            fprintf(stderr, "Error");
            return 1;
        }
        if (!pid) {
            dup2(fd1[0], 0);
            dup2(fd2[1], 1);
            close(fd2[1]);
            close(fd2[0]);
            close(fd1[0]);
            close(fd1[1]);
            execvp(argv[separators[0] + 1], argv + separators[0] + 1);
            exit(1);
        }

        pid_t  pid1;
       if ((pid1 = fork()) == 0) {
         dup2(fd2[0], 0);
         close(fd1[1]);
         close(fd1[0]);
         close(fd2[0]);
         close(fd2[1]);
         wait(NULL);
         execvp(argv[separators[1] + 1], argv + separators[1] + 1);
         exit(1);
       }
       if (pid1 < 0){
         close(fd1[0]);
         close(fd2[0]);
         close(fd1[1]);
         close(fd2[1]);
         fprintf(stderr, "Error");
         return 1;
       }

    }
    close(fd1[0]);
    close(fd2[0]);
    close(fd1[1]);
    close(fd2[1]);
    free(separators);
    wait(NULL);
    wait(NULL);
    return 0;
}

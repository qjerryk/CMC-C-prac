
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SON1 1
#define SON2 2
int ball1, ball2;

void job(int fd[], struct sembuf curr, struct sembuf next, struct sembuf prnext, int semid, char process[], int flag){

    while(ball1 >= ball2){
        semop(semid, &curr, 1);
        read(fd[0], &ball1, sizeof(int));
        read(fd[0], &ball2, sizeof(int));
       // read(fd[0], &flag, sizeof(int));
        if (ball2 - ball1 < 1) {
            printf("%s %d %d\n", process, ball1, ball2);
            fflush(stdout);
        }
        --ball1;
        ++ball2;
        //printf("%s %d %d\n", process, ball1, ball2);
        write(fd[1], &ball1, sizeof(int));
        write(fd[1], &ball2, sizeof(int));
        if (flag == SON2) {
            if (!strcmp(process, "father")) {
              flag = SON1;
              //flag = (flag + 1) % 2;
            }
           // write(fd[1], &flag , sizeof(int));
            semop(semid, &next, 1);
        }else if(flag == SON1){
            if (!strcmp(process, "father")) {
              flag = SON2;
              //flag = (flag + 1) % 2;
            }
         //   write(fd[1], &flag , sizeof(int));
            semop(semid, &prnext, 1);
        }
    }
}

int main(){
    pid_t pid1, pid2;
    int fd[2];
    if (pipe(fd) < 0){
        perror("pipe");
        return 0;
    }

    key_t key = ftok(".", 'i');
    scanf("%d %d", &ball1, &ball2);
    int semid = semget(key, 4, 0666 | IPC_CREAT);

    if (semid == -1){
        perror("semget");
        return 0;
    }

    struct sembuf upson1 = {1, 1, 0},
                  upson2 = {2, 1, 0},
                  updad = {0, 1, 0},
                  downson1  = {1, -1, 0},
                  downson2 = {2, -1, 0},
                  downdad = {0, -1, 0};


    int flag = SON2;
    if ((pid1 = fork()) < 0){
        perror("fork");
        return 0;
    }
    if (!pid1){

        job(fd, downson1, updad, updad, semid, "son1", SON1);
        close(fd[0]);
        close(fd[1]);
        _exit(0);
    }

    if ((pid2 = fork()) < 0){
        perror("fork");
        return 0;
    }

    if (!pid2){

        job(fd, downson2, updad, updad, semid, "son2", SON2);
        close(fd[0]);
        close(fd[1]);
        _exit(0);
    }



    write(fd[1], &ball1, sizeof(int));
    write(fd[1], &ball2, sizeof(int));
   // write(fd[1], &flag, sizeof(int));
    semop(semid, &upson1, 1);

    job(fd, downdad, upson2, upson1, semid, "father", flag);
    kill(pid1, SIGKILL);
    kill(pid2, SIGKILL);


    wait(NULL);
    wait(NULL);
    close(fd[0]);
    close(fd[1]);
    semctl(semid, 0, IPC_RMID, 0);
    return 0;
}


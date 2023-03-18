
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
struct msgbuf{
    long type;
    char msg[4];
};

int main(){
    int part1, part2, part3, part4;
    pid_t pid1, pid2, pid3;
    int fd = open("qdwqdwqdwqdwqdwqdwqdwqdwqdwq.txt", O_CREAT);
    if (fd == -1){
        perror("fd");
        exit(0);
    }
    close(fd);
    //char *s;
    key_t key = ftok("qdwqdwqdwqdwqdwqdwqdwqdwqdwq.txt", '_');
    if (key < 0) {
      perror("key");
      return 0;
    }
    int msgid = msgget(key, IPC_CREAT | IPC_EXCL  | 0644);
    if (msgid < 0){
        perror("msgid");
        exit(0);
    }



    if ((pid1 = fork()) < 0){
        perror("fork");
        exit(0);
    }
    struct msgbuf message;
    if (!pid1){//son1
        while(1){
            msgrcv(msgid, &message, 4*sizeof(char), 1, 0);
            int network = message.msg[0];
            printf("A %x\n", network);
            fflush(stdout);
        }
    }

    if ((pid2 = fork()) < 0){
        perror("fork");
        exit(0);
    }
    if (!pid2){//son2
        while(1){
            msgrcv(msgid, &message, 4*sizeof(char), 2, 0);
            int network = (message.msg[0] & ((1 << 7) - 1)) << 8 | message.msg[1];
            printf("B %x\n", network);
            fflush(stdout);
        }
    }

    if ((pid3 = fork()) < 0){
        perror("fork");
        exit(0);
    }
    if (!pid3){//son3
        while(1){
            msgrcv(msgid, &message, 4*sizeof(char), 3, 0);
            int network = ((message.msg[0] & ((1 << 6) - 1)) << 8 | message.msg[1]) << 8 | message.msg[2];
            printf("C %x\n", network);
            fflush(stdout);
        }
    }
    while(scanf("%d.%d.%d.%d", &part1, &part2, &part3, &part4) == 4){
        if (part1 < 128){
            message.type = 1;
        }
        if (part1 > 127 && part1 < 192){
            message.type = 2;
        }
        if (part1 >= 192){
            message.type = 3;
        }
        message.msg[0] = part1;
        message.msg[1] = part2;
        message.msg[2] = part3;
        message.msg[3] = part4;
        msgsnd(msgid, &message, 4*sizeof(char), 0);
    }
    kill(pid1,  SIGKILL);
    kill(pid2, SIGKILL);
    kill(pid3, SIGKILL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    msgctl(msgid, IPC_RMID, 0);
    remove("qdwqdwqdwqdwqdwqdwqdwqdwqdwq.txt");
    return 0;
}

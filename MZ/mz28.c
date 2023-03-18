#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <wait.h>

int main(void) {
  pid_t  pid1, pid2, pid3;
  key_t key = ftok(".", 42);
  key_t semkey = ftok(".", 7);
  if (key < 0 || semkey < 0) {
    perror("ftok");
    return 0;
  }
  size_t memory_size;
  scanf("%ld", &memory_size);
  int shmid = shmget(key,  memory_size + 1, IPC_CREAT | IPC_EXCL | 0644);
  if (shmid  < 0) {
    perror("shmget");
    return 0;
  }
  unsigned char*address = shmat(shmid, 0, 0);
  int semid = semget(semkey, 3, 0644 | IPC_CREAT);
  if (semid < 0) {
    perror("semget");
    return 0;
  }
  address[memory_size] = memory_size;

  struct sembuf upson1 = {0, 1, 0},
                downson1 = {0, -1, 0},
                upson2 = {1, 1, 0},
                downson2 = {1, -1, 0},
                upson3 = {2, 1, 0},
                downson3 = {2, -1, 0};



  if ((pid1 = fork()) < 0) {
    perror("fork error\n");
    _exit(1);
  }
  if (!pid1) { // son1
    //int shmid = shmget(key,  memory_size + 1, 0644);
    unsigned char*add1 = shmat(shmid, NULL, 0);
    if (add1 == NULL) {
      perror("shamt");
      return 0;
    }
    size_t cnt = 0;
    while (add1[memory_size]) {
      semop(semid, &downson1, 1);
      //printf("add1 cnt %d\n", add1[memory_size]);
      add1[memory_size-add1[memory_size]] = 'a'+ (cnt  % 26);
      cnt++;
      add1[memory_size]--;
      semop(semid, &upson2, 1);
    }
    _exit(0);
  }
  if ((pid2 = fork()) < 0) {
    perror("fork error\n");
    _exit(1);
  }
  if (!pid2) { // son2
    //int shmid = shmget(key,  memory_size + 1, IPC_CREAT | IPC_EXCL | 0644);
    unsigned char*add2 = shmat(shmid, NULL, 0);
    if (add2 == NULL) {
      perror("shamt");
      return 0;
    }
    size_t cnt = 0;
    while (add2[memory_size]) {
      semop(semid, &downson2, 1);
      //printf("add2 cnt %d\n", add2[memory_size]);
      add2[memory_size-add2[memory_size]] = '0'+ (cnt  % 10);
      cnt++;
      add2[memory_size]--;
      semop(semid, &upson3, 1);
    }
    _exit(0);
  }
  if ((pid3 = fork()) < 0){
    perror("fork error\n");
    _exit(1);
  }
  if (!pid3){ // son3
   // int shmid = shmget(key,  memory_size + 1, IPC_CREAT | IPC_EXCL | 0644);
    unsigned char*add3 = shmat(shmid, NULL, 0);
    if (add3 == NULL) {
      perror("shamt");
      return 0;
    }
    size_t cnt = 0;
    while (add3[memory_size]) {
      semop(semid, &downson3, 1);
      //printf("add3 cnt %d\n", add3[memory_size]);
      add3[memory_size-add3[memory_size]] = 'A'+ (cnt  % 26);
      cnt++;
      add3[memory_size]--;
      semop(semid, &upson1, 1);
    }
    _exit(0);
  }
  semop(semid, &upson1, 1);
  while(address[memory_size]);
  kill(pid1, SIGKILL);
  kill(pid2, SIGKILL);
  kill(pid3, SIGKILL);
  wait(NULL);
  wait(NULL);
  wait(NULL);
  address[memory_size] = '\0';
  printf("%s", address);
  shmctl(shmid, IPC_RMID, 0);
  semctl(semid, 0, IPC_RMID, 0);
  return 0;
}
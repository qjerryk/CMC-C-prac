/*В программе создаются процесс-отец, процесс-сын1 и процесс-сын2.
 * Необходимо реализовать игру пинг-понг между этими тремя процессами используя ОДИН неименованный канал.
 * Синхронизация через сигналы. Начинает игру процесс-отец.
 * Он читает со стандартного ввода два целых числа типа int и через канал посылает их процессу-сыну2.
 * Процесс-сын2 печатает на стандартный вывод значение полученных чисел, уменьшает первое число на 1,
 * а второе увеличивает на 1 и посылает процессу-сыну1, процесс-сын1 также печатает значение двух чисел, уменьшает первое на 1,
 * а второе увеличивает на 1 и посылает процессу-отцу, процесс-отец печатает числа и уменьшает первое на 1, а второе увеличивает на 1 и посылает процессу-сыну2 и так далее.
 * Игра пинг-понг останавливается, когда первое число становится меньше второго числа.
 * После буферизированной печати не забываем ставить fflush(stdout)*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>



int ball1, ball2;
int fd[2];
pid_t pid[3];
volatile sig_atomic_t flag = 0;

void sig_handler(int signum){
    flag = 1;
}


void job(int ind, sigset_t mask){
    while (ball1 >= ball2){
        sigsuspend(&mask);
        read(fd[0], &ball1, sizeof(int));
        read(fd[0], &ball2, sizeof(int));
        read(fd[0], &ind, sizeof(int));
        if (flag) {
            flag = 0;
            if (ball2 - ball1 < 1) {
                if (ind == 2)
                    printf("father %d %d\n", ball1, ball2);
                else if (ind == 1) {
                    printf("son2 %d %d\n", ball1, ball2);
                } else {
                    printf("son1 %d %d\n", ball1, ball2);
                }
                fflush(stdout);
            }
            --ball1;
            ++ball2;
            ind = (ind + 2) % 3;
            write(fd[1], &ball1, sizeof(int));
            write(fd[1], &ball2, sizeof(int));
            write(fd[1], &ind, sizeof(int));
            signal(SIGUSR1, sig_handler);
            kill(pid[ind], SIGUSR1);

        }
    }
}

int main(){

    if (pipe(fd) == -1) {
        perror("pipe");
    }

    int tmp = 1;
    sigset_t mask;
    sigset_t old_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);
    signal(SIGUSR1, sig_handler);
    signal(SIGINT, sig_handler);
    scanf("%d %d", &ball1, &ball2);
    pid[2] = getpid();
    pid[0] = fork();
    if (!pid[0]){
        kill(getppid(), SIGUSR1);
        job(1, old_mask);
        _exit(0);
    }
    pid[1] = fork();
    if (!pid[1]){
        kill(getppid(), SIGUSR1);
        job(1, old_mask);
        _exit(0);
    }



    write(fd[1], &ball1, sizeof(int));
    write(fd[1], &ball2, sizeof(int));
    write(fd[1], &tmp, sizeof(int));

    sigsuspend(&old_mask);
    sigsuspend(&old_mask);

    kill(pid[1], SIGUSR1);
    job(1, old_mask);

    wait(NULL);
    wait(NULL);
    close(fd[0]);
    close(fd[1]);
    return 0;
}
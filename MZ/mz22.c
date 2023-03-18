/*На стандартном потоке ввода задается целое число N (N > 0). Это число читает процесс отец. Требуется создать N процессов-сыновей.

Каждый процесс-сын выводит на стандартный поток вывода строку son n, где n порядковый номер этого сына. В конце строки выводится \n. Для вывода можно использовать средства высокого уровня,(printf) но помните, что printf использует буффер, поэтому не забывайте его сбрасывать. (функция fflush(stdout))

Кроме того каждый из процессов-сыновей выводит в стандартный поток ошибок свой PID и PPID в следующем формате: "PID = %d PPID = %d\n" и тоже не забываем про сброс буфера.

Процесс-родитель должен завершаться самым последним из всех процессов.*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){
    int N;
    int i, pid;
    scanf("%d", &N);
    for (i = 0, pid = fork(); i < N; i++, pid = fork()){
        int status;
        if (pid < 0){
            fprintf(stderr, "Error");
        }
        if (!pid){
            fflush(stdout);
            printf("son %d\n", i+1);
            fflush(stderr);
            fprintf(stderr, "PID = %d PPID = %d\n", getpid(), getppid());
            wait(&status);
            break;
        }
        wait(&status);
    }
    return 0;
}
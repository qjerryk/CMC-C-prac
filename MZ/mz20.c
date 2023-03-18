/*Написать программу, которая копирует из текстового файла не более 50 символов каждой строки. То есть если в строке меньше или равно 50 символов, строка копируется целиком. (символ '\n' в эти 50 символов не входит), если строка больше 50 символов, то копируются только первые 50, остальные игнорируются. Результат записать в другой файл. Имена файлов передаются через параметры командной строки.

Для копирования использовать низкоуровневые функции для работы с файлами (open, read, write, close)

В случаи возникновения любых нештатных ситуаций в поток ошибок вывести "Error".

Строки в текстовом файле это последовательность ASCII символов заканчивающаяся символом перевода строки '\n'.

Реализовать копирование блоками по N байт, где N некоторая константа. Допустимо читать из файла блоком одного размера, а записывать блоком другого размера, но недопустимо, чтобы размер блока равнялся 1 либо был кратен 50 или его делителям

*/
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define N 20
#define LINE_SIZE 50
int main(int argc, char **argv){
    if (argc < 3){
        fprintf(stderr, "Error\n");
        return 1;
    }
    int fd_read = open(argv[1], O_RDONLY);
    int fd_write = open(argv[2], O_WRONLY | O_TRUNC);
    char str[N];
    int do_we_need_to_complete = 0;
    int n_bytes = 1, cnt_curr = 0;
    if (fd_read == -1 || fd_write == -1){
        fprintf(stderr, "Error\n");
		if (fd_read != -1){
			close(fd_read);
		}
		if (fd_write != -1){
			close(fd_write);
		}
        return 2;
    }
    while ((n_bytes = read(fd_read, str, N))){
        int i = 0;
		char line[LINE_SIZE+1];
		//printf("\n%s\n", str);
        while (i < n_bytes){
            if (cnt_curr <= LINE_SIZE && !do_we_need_to_complete){
                if (str[i] == '\n'){
					int z, flag_newline = 1;
					line[cnt_curr] = '\0';
					//printf("\n cnt: %d \n", cnt_curr);
					//printf("%s", line);
					//printf(" cnt: %d\n", cnt_curr);
					for (z = 0; z < cnt_curr && line[0] != '\0' && cnt_curr != 1;  z++){
				//		putchar(line[z]);
						if (line[z] == '\0')
							flag_newline = 0;
					}
					if (flag_newline){
						//cnt_curr++;
						line[cnt_curr] = '\n';
					}
					//if (cnt_curr) 
						write(fd_write, line, cnt_curr+1);
					
				//	if (line[0] == '\0')
				//		printf("%s", line+1);
                    cnt_curr = 0;
					line[0]='\0';
                    do_we_need_to_complete = 0;
					i++;
					//putchar('\n');
					continue;
                }
				line[cnt_curr] = str[i];
                //write(fd_write, str+i, 1);
                cnt_curr++;
            }else{

					int flag_newline = 1;
                if (!do_we_need_to_complete){
				//	printf("cnt: %d\n", cnt_curr);
			
					for (int z = 0; z < cnt_curr && z < LINE_SIZE; z++){
						if (line[z] == '\n'){
							flag_newline = 0;
						}
						//printf("%d ", z);
					//	putchar(line[z]);
						//putchar('\n');
					}
					if (flag_newline){
				
						line[LINE_SIZE]='\n';
					}
					write(fd_write, line, cnt_curr);	
				//	line[cnt_curr-1]='\0';
				//	printf("%s", line);
                    cnt_curr = 0;
                }
                do_we_need_to_complete = 1;
                while (i < n_bytes && str[i] != '\n') {
                    i++;
                }
                if (i == n_bytes) break;
				
				//printf("cnt: %d\n", cnt_curr);
			//	line[cnt_curr]='\0';
				//printf("%s", line);
                cnt_curr = 0;
                do_we_need_to_complete = 0;
                //write(fd_write, str+i, 1);
				
            }
            i++;
        }
    }
    close(fd_write);
    close(fd_read);
    return 0;
}

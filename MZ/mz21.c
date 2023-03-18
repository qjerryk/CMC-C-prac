/* В бинарном файле лежат целые знаковые числа типа int.
 * Написать программу, которая запишет в другой файл эти числа в бинарном виде в порядке возрастания.
 * Запрещено хранить одновременно больше двух чисел из файла. Имена файлов передаются через параметры командной строки. Разрешается изменение входного файла. В случае, если данные во входящем (исходном) файле заданы некорректно,
 * (то есть количество байт в файле не кратно sizeof(int)) выходной файл (результат) оставить пустым.
*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
int size_of_file(int fd_read){
	int ind = 0, num, len;
	while(len = read(fd_read, &num, sizeof(int))){
		ind++;
		if (len != sizeof(int)){
			return 0;
		}
	}
	return ind;
}

int sorting(int fd_read, int fd_write){
	lseek(fd_read, 0, SEEK_SET);
	int *buffer = (int*)malloc(sizeof(int));
	*buffer = INT_MAX;
	int num, curr_min = INT_MAX, min_ind = 0, ind = 0;
	while (read(fd_read, &num, sizeof(int))){
		if (num <= curr_min){
			curr_min = num;
			min_ind = ind;
		}
		ind++;
	}
	lseek(fd_read, sizeof(int)*min_ind, SEEK_SET);
	write(fd_read, buffer, sizeof(int));
	*buffer = curr_min;
	write(fd_write, buffer, sizeof(int));
	free(buffer);
}


int main(int argc, char **argv){
	if (argc < 1 || !argv[1] || !argv[2]){
		fprintf(stderr, "Error");
		return 1;
	}
	int fd_read = open(argv[1], O_RDWR);
	int fd_write = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_read < 0 || fd_write < 0){
		fprintf(stderr, "Error");
		if (fd_read) close(fd_read);
		if (fd_write) close(fd_write);
		return 2;
	}
	int sz = size_of_file(fd_read);
	while(sz--){
		sorting(fd_read, fd_write);
	}
	close(fd_read);
	close(fd_write);
	return 0;
}

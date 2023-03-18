#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>


size_t word_len(const int fd, int *isdiv){
	char c;
	size_t len = 0;
	while (read(fd, &c, 1) == 1){
		fflush(stdout);
		if (c == ' ' || c == ',' || c == '.' || c == '\n'){
			*isdiv = 1;
			break;
		}
		++len;
	}
	return len;
}

size_t file_len(const int fd){
	lseek(fd, 0, SEEK_SET);
	size_t len = 0;
	char c;
	while(read(fd, &c, 1) == 1){
		++len;
	}
	return len;
}


void shift(const int fd, const int offset, const int flen, const int tail_pos){
	int ind;
	char tmp = '!';
	for (ind = 0; ind < offset; ind++){
		lseek(fd, 0, SEEK_END);
		write(fd, &tmp, 1);
	}
	for (ind = 0; offset + flen - ind - 1 != tail_pos; ind++){
		lseek(fd, -offset-ind-1, SEEK_END);
		char c;
		read(fd, &c, 1);
		lseek(fd, -ind-1, SEEK_END);
		write(fd, &c, 1);
	}
}

void rep_ends(const int fd, const int n, const int m){
	char c;
	size_t len = 0, tail_pos = 0, flen = file_len(fd), len_add=0;
	lseek(fd, 0, SEEK_SET);
	for (;;){
		int isdiv = 0;
		int is_last = 0;
	
		if (!(len=word_len(fd, &isdiv))){
			if (!len_add)
				break;
			else
				is_last = 1;
		}
		len+=len_add;
		//printf("%d\n", len);
		tail_pos+=len;
		if (len >= n+m){
			//char *buffer = (char*)calloc(len, sizeof(char));
		//	lseek(fd, -(int)len, SEEK_CUR);
	//		printf("%d %d %d %d\n", fd, n+m, flen, tail_pos);
			lseek(fd, tail_pos-1, SEEK_SET);
			read(fd, &c, 1);
			if (c == ' ' || c == '\n' || c == '.' || c == ',') break;
			shift(fd, n+m, flen, tail_pos);
			for (int i = 0; i < m; i++){
				char c;
				lseek(fd, tail_pos-i-1, SEEK_SET); // offsetfromstart - 1 - i to read c;
				read(fd, &c, 1);
	//			printf("%c\n", c);
				lseek(fd, tail_pos+n+m-i-1, SEEK_SET); // offset from start + n - i - 1
				write(fd, &c, 1);
			}
			for (int i = 0; i < len; i++){ // word len - n
				char c;
				lseek(fd, tail_pos - i - 1, SEEK_SET);//offset -  i - 1
				read(fd, &c, 1);
				//printf("%c\n", c);
				lseek(fd, tail_pos+n-1-i, SEEK_SET);//offset + n
				write(fd, &c, 1);
			}
			len_add = 0;
			//if (len == 3) break;
			flen+=n+m;
			tail_pos+=n+m;

		}
		lseek(fd, tail_pos, SEEK_SET);
		//head_pos+=len+spaces;
		if (flen == tail_pos){
			break;
		}
		//if (j == 1) break;
	//	printf("tail pos = %d flen = %d\n", tail_pos, flen);
		while (1){
			if (read(fd, &c, 1) != 1){
				break;
			}
			is_last = 0;
			if (c != ' ' && c != ',' && c !='.' && c != '\n'){
				break;
			}
			tail_pos++;
			//fflush(stdout);
			//printf("%c", c);
		//	printf("tail_pos = %d \n", tail_pos);
		}
		len_add = 1;
		if (is_last) break;
	}
}

void rep_tail(const int fd, const int n){
	rep_ends(fd, 0, n);
}

void rep_head(const int fd, const int n){
	rep_ends(fd, n, 0);
}


int main(int argc, char **argv){
	if (argc < 5 || argc > 6){
		fprintf(stderr, "Error");
		return 0;
	}
	int fd = open(argv[1], O_RDWR);
	if (argv[3][0] == 'e'){
		if (argc < 6){
			fprintf(stderr, "Error");
			return 0;
		}
		rep_ends(fd, atoi(argv[4]), atoi(argv[5]));
	}else if (argv[3][0] == 'h'){
		rep_head(fd, atoi(argv[4]));
	}else if(argv[3][0] == 't'){
		rep_tail(fd, atoi(argv[4]));
	}else{
		fprintf(stderr, "Error");
	}
	close(fd);
	return 0;
}

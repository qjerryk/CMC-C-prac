#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define BUFFER 1500


void print_str_arr(char** text, int sz){
	int ind;
	for (ind = 0; ind < sz; ++ind){
		printf("%s\n", text[ind]);
	}

}

int readline(char** str){
	int cnt = 0, currsize = BUFFER;
	char c;
	while ((c = getchar()) != '\n' && (c != EOF)){
		if (cnt >= currsize){
			currsize+=BUFFER;
			*str = (char*)realloc(*str, currsize*sizeof(char));
			if (str == NULL){
				fprintf(stderr, "Not enough memory");
				return 0;
			}
		}
		(*str)[cnt] = c;
		cnt++;
	}
	if (cnt == currsize){
		currsize+=1;
		*str = (char*)realloc(*str, currsize*sizeof(char));
		if (*str == NULL){
			fprintf(stderr, "Not enough memory");
			return 0;
		}
	}
	(*str)[cnt] = '\0';
	if (c == '\n') return -1;

	return cnt;
}


size_t my_strlen(char* str){
	if (str == NULL) return -1;
	int ind= 0;
	while(str[ind] != '\0'){
		ind++;
	}
	return ind;
}

void my_strcpy(char** dst, char* src){
	size_t ind = 0;
	for (ind = 0; ind < my_strlen(src); ind++){
		(*dst)[ind] = src[ind];
	}
	(*dst)[ind+1] = '\0';
}

int my_strcmp(char* str1, char* str2){
	if (str1 == NULL || str2 == NULL) return INT_MAX;
	int ind = 0;
	do{
		if (str1[ind] > str2[ind]) return 1;
		if (str1[ind] < str2[ind] ) return -1;
		ind++;
	}	while (str1[ind] != '\0' || str2[ind] != '\0');
	if (str1[ind] > str2[ind]) return 1;
	if (str1[ind] < str2[ind] ) return -1;
	return 0;
}

char* my_strstr(char* source, char* str){
	if (source == NULL || str == NULL) return NULL;
	int src_len = my_strlen(source);
	int str_len = my_strlen(str);
	int ind, j, is_appeared;
	for (ind = 0; ind < src_len - str_len+1 ; ++ind){
		is_appeared = 1;
		for (j = 0; j < str_len; j++){
			if (source[ind+j] != str[j]){
				is_appeared = 0;
				break;
			}
		}
		if (is_appeared) return source+ind;
	}
	return NULL;
}

size_t readtext(char ***text, int *currsize){
	*currsize = BUFFER;
	int ind = 0;
	int isreading = 0;
	do{
		(*text)[ind] = (char*)calloc(BUFFER, sizeof(char));
		isreading = readline(&(*text)[ind]);
//		printf("%s\n", (*text)[ind]);
		if (ind >= *currsize){
			*currsize+=BUFFER;
			*text = (char**)realloc(*text, *currsize*sizeof(char*));
			if (*text == NULL){
				fprintf(stderr, "Not enough memory");
				return 0;
			}
		}
		ind++;
	}while(isreading);
	if (ind)
		return ind-1;
	else
		return 0;
}

void findsubstr(char **text, size_t lenght, char* substr, int occur){
	size_t ind = 0;
	size_t cnt;
	char *tmp_str;
	for (; ind < lenght; ind++){
		cnt  = 0;
		tmp_str = (char*)calloc(my_strlen(text[ind])+1, sizeof(char));
		my_strcpy(&tmp_str, text[ind]);
		while ((tmp_str = my_strstr(tmp_str, substr)) != NULL){
			cnt++;
			tmp_str+=my_strlen(substr);
		}
		if (cnt == occur){
			printf("%zu:%s\n", ind+1, text[ind]);
		}
		free(tmp_str);
	}
}

void free_txt(char ***text, size_t lenght){
	size_t ind = 0;
	for (ind = 0; ind < lenght; ind++){
		free((*text)[ind]);
	}
	free(*text);
}

int main(int argc, char **argv){
	char **text = (char**)calloc(BUFFER, sizeof(char*));
	if (text == NULL){
		fprintf(stderr, "Not enough memory");
		return 0;
	}
	int memorysz = BUFFER;
	size_t lenght = readtext(&text, &memorysz);
//	printf("%c\n", text[0][0]);
//	print_str_arr(text, lenght);
	findsubstr(text, lenght, argv[1], atoi(argv[2]));
	free_txt(&text, lenght);
//	char* testline = (char*)calloc(BUFFER, sizeof(char));
//	readline(&testline);
//	printf("%s\n", testline);
	return 0;
}

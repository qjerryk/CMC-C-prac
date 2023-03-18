#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define BUFFER 1001

 

int readline(char** str){
	int cnt = 0, currsize = BUFFER;
	char c;
	while ((c = getchar()) != EOF){
		if (cnt >= currsize){
			currsize+=BUFFER;
			*str = (char*)realloc(*str, currsize);
		}
		(*str)[cnt] = c;
		cnt++;
	}
	if (cnt == currsize){
		currsize+=1;
		*str = (char*)realloc(*str, currsize);
	}
	(*str)[cnt] = '\0';
	return cnt;
	
}


void mystrcpy(char **src, char *str){
	int ind = 0;
	while ((str)[ind] != '\0'){
		(*src)[ind] = (str)[ind];
		ind++;
	}
	(*src)[ind] = '\0';
}


void deleting(char** str, char* badstr){
	char* tmp;
	int offset = 0, ind, cnt;
	while ((tmp = strstr(*str+offset, badstr))!= NULL){
		if (tmp != *str){
			if ((*(tmp-1) == ' ' || *(tmp-1) == '\n') && (tmp[strlen(badstr)] == ' ' || tmp[strlen(badstr)] == '\n')){
				cnt = 0;
				for (ind = strlen(badstr); tmp[ind] == ' '; ind++){
					cnt++;
				}
				mystrcpy(&tmp, (tmp+strlen(badstr)+cnt));
			}else if ((*(tmp-1) == ' ' || *(tmp-1) == '\n') && tmp[strlen(badstr)+1] == '\0'){
				mystrcpy(&tmp, (tmp+strlen(badstr)));
			}else{
				offset+=strlen(badstr);
			}
		}else{
			if (tmp[strlen(badstr)] == ' ' || tmp[strlen(badstr)] == '\n'){
				cnt = 0;
				for (ind = strlen(badstr); tmp[ind] == ' ' /*|| tmp[ind] == '\n'*/; ind++){
					cnt++;
				}
				mystrcpy(&tmp, (tmp+strlen(badstr)+cnt));
			}else if ( tmp[strlen(badstr)+1] == '\0'){
				mystrcpy(&tmp, (tmp+strlen(badstr)));
			}else{
				offset+=strlen(badstr);
			}

		}
	} 
}

int isLower(char c){
	return c>='a' && c <= 'z';
}

void cenzure(int argc, char** argv){
	int j, k;
	char *str = (char*)calloc(BUFFER, sizeof(char));
//	char tmp[1001] = "dqwedwqdwqd";
	while(1){
		readline(&str);
		if (str[0] == '\0'){
			break;
		}
		for (j = 1; j < argc; j++){
	//		strcpy(tmp, argv[j]);
			deleting(&str, argv[j]);
			argv[j][0]-=('a'-'A')*isLower(argv[j][0]);
			deleting(&str, argv[j]);
			for (k = 1; k < strlen(argv[j]); k++){
				argv[j][k]-=('a'-'A')*isLower(argv[j][k]);
			}
			deleting(&str, argv[j]);
		} 
		printf("%s\n", str);
	}
	free(str);
}


int main(int argc, char** argv){
	cenzure(argc, argv);
	return 0;
}


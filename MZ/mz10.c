#include <stdio.h>




int is_divisor(char c){
	return (c == ' ' || c == '\n');
}


int main(int argc, char** argv){
    if (argc != 3){
        return 1;
    }
	FILE* fr = fopen(argv[1], "r");
	FILE* fw = fopen(argv[2], "w");;
	if (fr == NULL){
		fprintf(stderr, "ERROR");
		return 1;
	}
	if (fw == NULL){
		fprintf(stderr, "ERROR");
		return 1;
	}
	char c = EOF, pred;
	size_t symb_cnt = 0, word_cnt = 0, str_cnt = 0;
	while (1){
		if (symb_cnt){
			pred = c;
		}
		c = fgetc(fr);
		if (c == EOF){
			break;
		}
		if (symb_cnt){
		//	if (str_cnt == 2)
		//		printf("%c %c\n", pred, c);
			if  (is_divisor(pred) && !is_divisor(c)){
				word_cnt++;
			}
		}else{
			if (!is_divisor(c)){
				word_cnt++;
			}
		}
		if (c == '\n'){
			str_cnt++;
		}
	//	printf("%c", c);
		symb_cnt++;
	}
	//fokwqidijowqdjiwqi
	printf("%zu %zu %zu", str_cnt, word_cnt, symb_cnt);
	fprintf(fw ,"%zu %zu %zu", str_cnt, word_cnt, symb_cnt);
	fclose(fr);
	fclose(fw);
	

}

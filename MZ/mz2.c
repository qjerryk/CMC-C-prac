#include <stdio.h>


int Read(char c, int* cnt){
	c = getchar();
        if (c == '\n'){
                *cnt = 0;
        }
        if ((c != '\n') &&( c != '=')){
                return 1;
        }

        while (c != '\n'){
                c = getchar();
                if (c == '='){
                       (*cnt)++;
                        continue;
                }
                if (c != '\n'){    
                    return 1;
                }
        }
	return 0;

}


int main() {
	char c = ' ';
	int cnt1 = 1, cnt2 = 1, error_checker;
	error_checker = Read(c, &cnt1);
	if (error_checker){
		printf("Incorrect");
		return 0;
	}
	error_checker = Read(c, &cnt2);
	if (error_checker){
		printf("Incorrect");
		return 0;
	}
	if (cnt1 > cnt2){
		printf("First");
	} else if(cnt2 > cnt1){
		printf("Second");
	}else{
		printf("Equal");
	}
	return 0;
}

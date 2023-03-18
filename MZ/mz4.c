#include <stdio.h>

int main(){
	int a, b, n, i, j;
	long long num;
	char c = ' ';
	scanf("%d%d%d", &a, &b, &n);
	printf ("%*c", n, c);
	for (i = a; i < b; ++i){
		printf(" %*d", n, i);
	}
	printf("\n");
		for (i = a; i < b; ++i){
			printf("%*d", n, i);
			for (j = a; j < b; ++j){
				num = (long long)i*j;
				printf(" %*lld", n, num);
			}
			printf("\n");
		}
		
	
	return 0;
}

#include <stdio.h>


void bit_reverse(unsigned int num){
    unsigned int res = 0;
    while (num){
        res = (res << 1) | (num & 1);
        num >>= 1;
    }
    printf("%d", res);
}


int main() {
	unsigned int num;
    scanf("%d", &num);
    bit_reverse(num);
	return 0;
}

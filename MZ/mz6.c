#include <stdio.h>

#define YEAR_MASK_SIZE 7
#define MONTH_MASK_SIZE 4
#define DAY_MASK_SIZE 5

void to_decimal(unsigned short bits){
	unsigned short day = 0,  month = 0,  year = 0;
	day = ((1 << DAY_MASK_SIZE) - 1 ) & bits;
	month = (((1 << (DAY_MASK_SIZE + MONTH_MASK_SIZE)) - 1 ) & bits) >> DAY_MASK_SIZE;
	year = (((1 << (DAY_MASK_SIZE + MONTH_MASK_SIZE + YEAR_MASK_SIZE)) -1 ) & bits) >> (DAY_MASK_SIZE+MONTH_MASK_SIZE);
	year += (year > 49)? 1900: 2000;
    printf("%d %d %d", year, month, day);
}



int main() {
	unsigned short bits = 1;
	scanf("%hu", &bits);
	to_decimal(bits);
	return 0;
}

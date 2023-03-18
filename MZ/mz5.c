#include <stdio.h>

#define YEAR_MASK 127
#define MONTH_MASK 15
#define DAY_MASK 31

void to_bits(int year, int month, int day, unsigned short* bits){
	*bits =(((year & YEAR_MASK) << 4) | (month & MONTH_MASK)) << 5 | ( day & DAY_MASK);
}

void output(unsigned short *bits) {
	unsigned short mask = 1 << 15;
	int tmp = 0, ind;
	for (ind = sizeof(unsigned short)*8-1; ind > -1; --ind, mask >>= 1 ){
		tmp = (*bits & mask) >> (ind);
		printf("%d", tmp);
	}
	printf("\n%hu", *bits);
	
}

int main() {
	unsigned short  year, month, day;
	unsigned short *bits = &year;
	scanf("%hu %hu %hu", &year, &month, &day);
	to_bits(year, month, day, bits);
	output(bits);
	return 0;
}


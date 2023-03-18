#include <stdio.h>


int main() {
	int Nx, Ny, i , j;
	double x, y;
	scanf("%d %d", &Nx, &Ny);
	for (i = 0; i < Ny; ++i){
		for (j = 0; j < Nx; ++j){
			x = 0 +(1.0/(Nx-1))*j;
			y = 1 -(1.0/(Ny-1))*i;
			printf("(%.3lf, %.3lf) ", x, y);
		}
		printf("\n");
	}
	return 0;
}	

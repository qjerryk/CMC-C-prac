#include <stdio.h>
int main()
{
    double  x, y, x1, y1, x2, y2, tmp;
    scanf("%lf%lf%lf%lf%lf%lf", &x, &y, &x1, &y1, &x2, &y2);
    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    if (x < x1 || x > x2) {
        printf("NO\n");
        return 0;
    }
    if (y < y1 || y > y2) {
        printf("NO\n");
        return 0;
    }
    printf("YES\n");
    return 0;
}

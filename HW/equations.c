#include <stdio.h>
#include <limits.h>

void output(int n){
    switch(n){
        case 0:
            printf("NO\n");
            break;
        case 1:
            printf("YES\n");
            break;
        case 2:
            printf("INPUT ERROR\n");
            break;
        case 3:
            printf("WRONG SIGN\n");
            break;
        case 4:
            printf("NO EQUAL SIGN\n");
            break;
        case 5:
            printf("NO ANSWER\n");
            break;
    }
}

int isDigit(char c){
    return (c >= '0') && (c <= '9');
}

int isOperator(char c){
    return (c == '=' || c == '*' || c == '+');
}

int Reading(){
    char c = ' ', operator = ' ';
    int ind = 0;
    long long nums[3] = {INT_MAX+1ll, INT_MAX+1ll, INT_MAX+1ll};
    while ((c = getchar()) != '\n'){
        if (c == '\t' || c == ' ') continue;

        if ((!isDigit(c) && !isOperator(c) && nums[ind] == INT_MAX+1ll && ind != 2) || (ind == 2 && nums[ind] != INT_MAX+1ll && !isDigit(c)) ){
            return 2;
        }

        if (isDigit(c)){
            if (nums[ind] == INT_MAX+1ll) nums[ind] = 0;
            nums[ind] = nums[ind]*10 + (c - '0');
            continue;
        }

        if (!isDigit(c) && !(c == '*' || c == '+') && ind == 0){ 
//            printf("%d\n", nums[0]);
            return 3;
        }
        if (!isDigit(c) && c != '='  && ind == 1) return 4;
        
        if (!isDigit(c) && ind == 2) return 5;

        if (isOperator(c)){
            if (ind == 0) operator = c;
            ++ind;
        }
    }
    if (nums[0] == INT_MAX+1ll) return 2;
    if (operator == ' ') return 3;
    if (nums[1] == INT_MAX+1ll) return 2;
    if (nums[2] == INT_MAX+1ll) return 5;
 //   printf("%d %c %d = %d\n", nums[0], operator, nums[1], nums[2]);
    if (operator == '+') return nums[0]+nums[1] == nums[2];
    else return nums[0]*nums[1] == nums[2];
}

int main(){
    int result;
    result = Reading();
    output(result);
    return 0;
}

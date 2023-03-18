#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER 15

int isUpper(char c){
    return (c >= 'A') && (c <= 'Z');
}

int isLower(char c){
    return (c >= 'a') && (c <= 'z');
}


int main() {
    char c = ' ';
    int uptr = 0, lptr = 0, currsizeofstr = BUFFER, i, j;
    char *str = (char *) calloc(BUFFER, sizeof(char));
    if (!str){
        fprintf(stderr, "Not enough memory");
        return 0;
    }
    char *tmp_str = "\0";
    while (((c = getchar()) != '\n') && (c != EOF)) {
        if (lptr + uptr >= currsizeofstr) {
            currsizeofstr += BUFFER;
            tmp_str = str;
            str = (char *) realloc(str, currsizeofstr);
            if (str == NULL) {
                fprintf(stderr, "Not enough memory");
                str = tmp_str;
                break;
            }
        }
        str[lptr + uptr] = c;
        if (c <= 'z' && c >= 'a') {
            lptr++;
        } else {
            uptr++;
        }
    }
    if (lptr + uptr + 1 >= currsizeofstr) {
        tmp_str = str;
        str = (char *) realloc(str, currsizeofstr + 1);
        if (str == NULL) {
            fprintf(stderr, "Not enough memory");
            str = tmp_str;
        }
    }
    for (i = 0; i < lptr + uptr; ++i) {
        for (j = 0; j < lptr + uptr - 1 - i; j++) {
            if (isUpper(str[j]) && isLower(str[j + 1])) {
                char tmp = str[j];
                str[j] = str[j + 1];
                str[j + 1] = tmp;
            }
        }
    }
    str[lptr + uptr] = '\0';
    printf("%s", str);
    if (str) free(str);
    return 0;
}

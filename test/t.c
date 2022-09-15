#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#define ArrLeng(x)  (sizeof(x) / sizeof((x)[0]))
int main(){
    long double d;
    d = 3.0190689000005070900009999;
    printf("%Lf\n", d);
    return 0;
}
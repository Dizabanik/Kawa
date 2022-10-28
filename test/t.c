#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#define ArrLeng(x)  (sizeof(x) / sizeof((x)[0]))
typedef struct _entity{
    int i;
} entity;

int *ints;

int main(){
    
    entity e;
    e.i = 10;
    void* p;
    p = &e;
    void *j;
    j = p;
    entity *k = j;
    printf("%d", k->i);
    
    
    return 0;
}

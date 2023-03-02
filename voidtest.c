#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    void* val = NULL;
    val = (unsigned char*)malloc(sizeof(unsigned char)*6);
    strcpy(val, "Hello");
    if(strcmp(val, "Hello")==0){
        printf("VAL1: %s\n", (unsigned char*)val);
        char str2[10] = " World";
        val = (unsigned char*)realloc(val, sizeof(unsigned char)*12);
        strcat(val, str2);
        printf("VAL2: %s\n", (unsigned char*)val);
        free(val);
    }
    else{
        printf("...\n");
    }
}
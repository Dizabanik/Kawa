#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define ArrLeng(x)  (sizeof(x) / sizeof((x)[0]))
#include <stdbool.h>
typedef struct _token{
    char type[51];
    char value[101];
    long double val;
} token;
typedef struct _var{
    char name[51];
    char value[101];
    char type[51];
    long double val;
} var;
typedef struct _func{
    char name[51];
    int pos;
    void* value;
    char type[51];
} func;
token* tokens;
var* vars;
func* funcs;
int tokLen;
int varLen;
char* codes = 0;
bool createVar(int pos, int *posi, bool withKeyword){
    if(withKeyword){
        bool isCustomKW = false;
        if(tokLen > pos+1 && strcmp(tokens[pos+1].type, "keyword_custom") == 0){
            isCustomKW = true;
        }
        if(!isCustomKW){
            if(tokLen <= pos+1){
                printf("Unexpected end of line, expected variable name\n");
                return false;
            }
            printf("Unexpected token %s, expected variable name\n", tokens[pos+1].type);
            return false;
        }
    }
    char varName[50] = "";
    strncpy(varName, tokens[pos+1].value, 101);
    bool isEq = false;
    bool isAdd = false;
    bool isSub = false;
    bool isEqAdd = false;
    bool isEqSub = false;
    bool isOper = false;
    if(tokLen > pos+2 && strcmp(tokens[pos+2].type, "operator") == 0){
        if(strcmp(tokens[pos+2].value, "eq") == 0){
            isEq = true;
            isOper = true;
        }
        else if(strcmp(tokens[pos+2].value, "add") == 0){
            isAdd = true;
            isOper = true;
        }
        else if(strcmp(tokens[pos+2].value, "sub") == 0){
            isSub = true;
            isOper = true;
        }
        else if(strcmp(tokens[pos+2].value, "eqadd") == 0){
            isEqAdd = true;
            isOper = true;
        }
        else if(strcmp(tokens[pos+2].value, "eqsub") == 0){
            isEqSub = true;
            isOper = true;
        }
        else{
            printf("Unexpected operator %s\n", tokens[pos+2].value);
            return false;
        }
    }
    if(withKeyword){
        if(!isOper && tokLen > pos+2){
            //----------------warning-------------------
            if(strcmp(tokens[pos+2].type, "keyword") != 0 || strcmp(tokens[pos+2].type, "keyword_custom") != 0){
                printf("Unexpected end of line, expected operator\n");
                return false;
            }
        }
    }
    char type[51] = "";
    void* vval;
    long double vvalF;
    bool exists = false; 
    bool existsOp = false; 
    if(isOper){
        if(strcmp(tokens[pos+3].type, "string") == 0){
            strncpy(type, "string", 10);
        }
        else if(strcmp(tokens[pos+3].type, "int") == 0){
            strncpy(type, "int", 10);
        }
        // else if(tokLen > pos+3 && strcmp(tokens[pos+3].type, "u_int") == 0){
        //     strcpy(type, "u_int");
        // }
        // else if(tokLen > pos+3 && strcmp(tokens[pos+3].type, "short") == 0){
        //     strcpy(type, "short");
        // }
        // else if(tokLen > pos+3 && strcmp(tokens[pos+3].type, "u_short") == 0){
        //     strcpy(type, "u_short");
        // }
        // else if(tokLen > pos+3 && strcmp(tokens[pos+3].type, "long") == 0){
        //     strcpy(type, "long");
        // }
        // else if(tokLen > pos+3 && strcmp(tokens[pos+3].type, "u_long") == 0){
        //     strcpy(type, "u_long");
        // }
        // else if(tokLen > pos+3 && strcmp(tokens[pos+3].type, "l_long") == 0){
        //     strcpy(type, "l_long");
        // }
        // else if(tokLen > pos+3 && strcmp(tokens[pos+3].type, "ul_long") == 0){
        //     strcpy(type, "ul_long");
        // }
        //else if(tokLen > pos+3 && strcmp(tokens[pos+3].type, "u_char") == 0){
        //    strcpy(type, "u_char");
        //}
        //else if(tokLen > pos+3 && strcmp(tokens[pos+3].type, "s_char") == 0){
        //    strcpy(type, "s_char");
        //}
        else if(strcmp(tokens[pos+3].type, "float") == 0){
            strncpy(type, "float", 10);
        }
        // else if(strcmp(tokens[pos+3].type, "double") == 0){
        //     strncpy(type, "double", 10);
        // }
        // else if(strcmp(tokens[pos+3].type, "l_double") == 0){
        //     strncpy(type, "l_double", 10);
        // }
        else if(strcmp(tokens[pos+3].type, "char") == 0){
            strncpy(type, "char", 10);
        }
        else if(strcmp(tokens[pos+3].type, "bool") == 0){
            strncpy(type, "bool", 10);
        }
        else if(strcmp(tokens[pos+3].type, "keyword_custom") == 0){
            
            for (int i = 0; i < varLen; i++) {
                if (strcmp(vars[i].name, tokens[pos+3].value) == 0) {
                    existsOp = true;
                    strncpy(type, vars[i].type, 51);
                    if(strcmp(type, "string") == 0){
                        vval = calloc(101, sizeof(char));
                        strncpy(vval, vars[i].value, 101);
                    }
                    else if(strcmp(type, "int") == 0){
                        //vval = calloc(1, sizeof(int));
                        vvalF = vars[i].val;
                    }
                    else if(strcmp(type, "float") == 0){
                        vvalF = vars[i].val;
                    }
                    else if(strcmp(type, "bool") == 0){
                        //vval = calloc(1, sizeof(bool));
                        vval = vars[i].value;
                    }
                    else if(strcmp(type, "char") == 0){
                        //vval = calloc(1, sizeof(char));
                        vval = vars[i].value;
                    }
                    // else if(strcmp(type, "double") == 0){
                    //     vvalF = vars[i].val;
                    // }
                    // else if(strcmp(type, "l_double") == 0){
                    //     vvalF = vars[i].val;
                    // }
                    else if(strcmp(type, "u_int") == 0){
                        //vval = calloc(1, sizeof(unsigned int));
                        vvalF = vars[i].val;
                    }
                    else if(strcmp(type, "short") == 0){
                        //vval = calloc(1, sizeof(short));
                        vvalF = vars[i].val;
                    }
                    else if(strcmp(type, "long") == 0){
                        //vval = calloc(1, sizeof(long));
                        vvalF = vars[i].val;
                    }
                    else if(strcmp(type, "u_long") == 0){
                        //vval = calloc(1, sizeof(unsigned long));
                        vvalF = vars[i].val;
                    }
                    else if(strcmp(type, "l_long") == 0){
                        //vval = calloc(1, sizeof(long long));
                        vvalF = vars[i].val;
                    }
                    else if(strcmp(type, "ul_long") == 0){
                        //vval = calloc(1, sizeof(unsigned long long));
                        vvalF = vars[i].val;
                    }
                    else if(strcmp(type, "u_char") == 0){
                        //vval = calloc(1, sizeof(unsigned char));
                        vvalF = vars[i].val;
                    }
                    else if(strcmp(type, "s_char") == 0){
                        //vval = calloc(1, sizeof(signed char));
                        vvalF = vars[i].val;
                    }
                    break;
                }
                existsOp = false;
            }
            if(!existsOp){
                printf("Can't find variable %s\n", varName);
                //free(vval);
                return false;
            }
        }
        else{
            strncpy(type, "undefined", 15);
        }
    }
    else{
        strncpy(type, "undefined", 15);
    }
    if(strcmp(type, "") == 0 || strcmp(type, "undefined") == 0){
        if(tokLen <= pos+3){
            if(isOper){
                printf("Unexpected end of line, expected value\n");
                //free(vval);
                return false;
            }
        } 
    }
    exists = false;
    for (int i = 0; i < varLen; i++) {
        if (strcmp(vars[i].name, varName) == 0) {
            exists = true;
            break;
        }
        exists = false;
    }
    if(exists){
        printf("Variable %s already exists\n", varName);
        //free(vval);
        return false;
    }
    if(varLen > 0){
        vars = realloc(vars, varLen + 1);
    }
    else{
        vars = malloc(sizeof(var));
    }
    varLen++;
    strncpy(vars[varLen-1].name, varName, 100);
    if(isOper){
        // if(strcmp(type, "string") == 0){
        //     vars[varLen-1].value = calloc(101, sizeof(char));
        // }
        // else if(strcmp(type, "int") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(int));
        // }
        //else if(strcmp(type, "float") == 0){

            //vars[varLen-1].value = calloc(51, sizeof(char));
        //}
        // else if(strcmp(type, "bool") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(bool));
        // }
        // else if(strcmp(type, "char") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(char));
        // }
        //else if(strcmp(type, "double") == 0){
            //vars[varLen-1].value = calloc(101, sizeof(char));
        //}
        //else if(strcmp(type, "l_double") == 0){
            //vars[varLen-1].value = calloc(101, sizeof(char));
        //}
        // else if(strcmp(type, "u_int") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(unsigned int));
        // }
        // else if(strcmp(type, "short") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(short));
        // }
        // else if(strcmp(type, "long") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(long));
        // }
        // else if(strcmp(type, "u_long") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(unsigned long));
        // }
        // else if(strcmp(type, "l_long") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(long long));
        // }
        // else if(strcmp(type, "ul_long") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(unsigned long long));
        // }
        // else if(strcmp(type, "u_char") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(unsigned char));
        // }
        // else if(strcmp(type, "s_char") == 0){
        //     vars[varLen-1].value = calloc(1, sizeof(signed char));
        // }
        if(strcmp(type, "") != 0 || strcmp(type, "undefined") != 0){
            if(existsOp){
                if(strncmp(type, "string", 51) == 0){
                    strncpy(vars[varLen-1].value, vval, 101);
                }
                else if(strncmp(type, "float", 10) == 0 || strncmp(type, "int", 10) == 0){
                    vars[varLen-1].val = vvalF;
                }
                else{
                    strcpy(vars[varLen-1].value, vval);
                }
            }
            else{
                if(strncmp(type, "string", 51) == 0){
                    strncpy(vars[varLen-1].value, tokens[pos+3].value, 101);
                }
                else if(strncmp(type, "float", 51) == 0){
                    vars[varLen-1].val = tokens[pos+3].val;
                }
                else{
                    strcpy(vars[varLen-1].value, tokens[pos+3].value);
                }
                // vars[varLen-1].value = 0;
                // vars[varLen-1].val = 0;
            }
            strncpy(vars[varLen-1].type, type, 51);
        }
        else{
            strcpy(vars[varLen-1].value, "");
            vars[varLen-1].val = 0;
            // printf("Undefined type of %s\n", vars[varLen-1].name);
            // return false;
        }
        pos+=4;
        *posi += 4;
    }
    else{
        strncpy(vars[varLen-1].type, "undefined", 51);
        strcpy(vars[varLen-1].value, "");
        vars[varLen-1].val = 0;
        pos+=2;
        *posi += 2;
    }
    //free(vval);
    return true;
}
void tokenize(){
    const int length = strlen(codes);
    int pos = 0;
    
    char BUILT_IN_KEYWORDS[2][20] = {{"var"}};
    char BUILT_IN_FUNCS[2][20] = {{"print"}};
    char varChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    int line = 1;
    int column = 0;
    while(pos<length){
        char currentChar = codes[pos];
        if(currentChar == ' '){
            pos++;
            column++;
            continue;
        }
        else if (currentChar == '\n' || currentChar == '\r')
        {
            line++;
            column = 0;
            pos++;
            continue;
        }
        else if(currentChar == '\"'){
            char res[101] = "";
            pos++;
            column++;
            while(codes[pos] != '\"' && pos < length){
                strncat(res, &codes[pos], 1);
                pos++;
                column++;
            }
            if(codes[pos] != '\"'){
                printf("Unterminated string at line %d column %d\n", line, column);
                pos++;
                continue;
            }
            pos++;
            column++;
            if(tokLen > 0){
                tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            }
            else{
                tokens = calloc(1, sizeof(token));
            }
            tokLen++;
            //strcpy(tokens[tokLen-1].type, "");
            //strcpy(tokens[tokLen-1].value, "");
            strncpy(tokens[tokLen-1].type, "string", 8);
            strncpy(tokens[tokLen-1].value, res, 101);
            
        }
        else if(currentChar == '\''){
            char res;
            pos++;
            column++;
            int leng = 0;
            while(codes[pos] != '\'' && pos < length){
                leng ++;
                res = codes[pos];
                pos++;
                column++;
            }
            
            if(codes[pos] != '\''){
                printf("Unterminated char at line %d column %d\n", line, column);
                pos++;
                continue;
            }
            // if(leng > 1){
            //     printf("Char field at line %d column %d contains more than one character, so it will be equal to the last character of field\n", line, column);
            // }
            pos++;
            column++;
            if(tokLen > 0){
                tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            }
            else{
                tokens = calloc(1, sizeof(token));
            }
            tokLen++;
            //strcpy(tokens[tokLen-1].type, "");
            //strcpy(tokens[tokLen-1].value, "");
            strncpy(tokens[tokLen-1].type, "char", 8);
            strncpy(tokens[tokLen-1].value, &res, 1);
            
        }
        else if(currentChar == '('){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            strncpy(tokens[tokLen-1].type, "bracket(", 15);
            pos++;
        }
        else if(currentChar == ')'){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            strncpy(tokens[tokLen-1].type, "bracket)", 15);
            pos++;
        }
        else if(currentChar == '['){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            strncpy(tokens[tokLen-1].type, "bracket[", 15);
            pos++;
        }
        else if(currentChar == ']'){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            strncpy(tokens[tokLen-1].type, "bracket]", 15);
            pos++;
        }
        else if(currentChar == '{'){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            strncpy(tokens[tokLen-1].type, "bracket{", 15);
            pos++;
        }
        else if(currentChar == '}'){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            strncpy(tokens[tokLen-1].type, "bracket}", 15);
            pos++;
        }
        else if(isdigit(currentChar)){
            char res[101] = "";
            bool isNum = true;
            bool sobaka = false;
            int digitsAfterDot = -1;
            int zerosCount = 0;
            while(isNum && pos < length){
                if(!isdigit(codes[pos]) || codes[pos] == ' ' || codes[pos] == 'n' || codes[pos] == '\r'){
                    if(codes[pos] != '.'){
                        //if(codes[pos] != 'd' || codes[pos] != 'l'){
                        if(codes[pos] != '@'){
                            isNum = false;
                            break;
                        }
                        // else if(codes[pos] == 'd'){
                        //    isDouble = 1;
                        // }
                        // else if(codes[pos] == 'l'){
                        //    isDouble = 2;
                        // }
                        else{
                            sobaka = true;
                        }
                        //}
                        
                    }
                    else if(digitsAfterDot == -1){
                        digitsAfterDot = 0;
                    }
                    else{
                        printf("Undefined second dot at line %d column %d\n", line, column);
                        pos++;
                        continue;

                    }
                }
                else if(codes[pos] == '0' && digitsAfterDot > 0){
                    zerosCount++;
                }
                else{
                    zerosCount = 0;
                }
                
                strncat(res, &codes[pos], 1);
                pos++;
                column++;
                if(digitsAfterDot > -1){
                    digitsAfterDot ++;
                }
            }
            if(tokLen > 0){
                tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            }
            else{
                tokens = calloc(1, sizeof(token));
            }
            tokLen++;
            //strcpy(tokens[tokLen-1].type, "");
            //strcpy(tokens[tokLen-1].value, "");
            if(digitsAfterDot > -1){
                if(zerosCount>0 && !sobaka){
                    res[strlen(res)-zerosCount] = '\0';
                }
                if(digitsAfterDot == 0){
                    strncat(res, '0', 1);
                    strncpy(tokens[tokLen-1].type, "float", 8);
                    tokens[tokLen-1].val = strtold(res, NULL);
                }
                else{
                    strncpy(tokens[tokLen-1].type, "float", 8);
                    tokens[tokLen-1].val = strtold(res, NULL);
                }
                
            }
            else{
                strncpy(tokens[tokLen-1].type, "int", 5);
                tokens[tokLen-1].val = strtold(res, NULL);
                if(pos < length){
                    //printf("Hello\n");
                }
            }
        }
        else{
            bool isInclude = false;
            for (int i = 0; i < ArrLeng(varChars); i++) {
                if (varChars[i] == currentChar) {
                    isInclude = true;
                    break;
                }
                isInclude = false;
            }
            if(isInclude){
                
                char res[101] = "";
                strncpy(res, &currentChar , 1);
                pos++;
                column++;
                bool isFunc = false;
                int brackets = 0;
                while(isInclude && pos<length){
                   for (int i = 0; i < ArrLeng(varChars); i++) {
                        if (varChars[i] == codes[pos]) {
                            isInclude = true;
                            strncat(res, &codes[pos], 1);
                            pos++;
                            column++;
                            break;
                        }
                        else if(codes[pos] == '('){
                            isFunc = true;
                            pos++;
                            column++;
                            break;
                        }
                        else if(codes[pos] == '['){
                            brackets = 1;
                            pos++;
                            column++;
                            break;
                        }
                        else if(codes[pos] == '{'){
                            brackets = 2;
                            pos++;
                            column++;
                            break;
                        }
                        else if(codes[pos] == ')'){
                            brackets = 3;
                            pos++;
                            column++;
                            break;
                        }
                        else if(codes[pos] == ']'){
                            brackets = 4;
                            pos++;
                            column++;
                            break;
                        }
                        else if(codes[pos] == '}'){
                            brackets = 5;
                            pos++;
                            column++;
                            break;
                        }
                        else{
                            isInclude = false;
                        }
                    }
                    
                }
                isInclude = false;
                for (int i = 0; i < ArrLeng(BUILT_IN_KEYWORDS); i++) {
                    if (strcmp(BUILT_IN_KEYWORDS[i], res) == 0) {
                        isInclude = true;
                        break;
                    }
                    isInclude = false;
                }
                if(tokLen > 0){
                    tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                }
                else{
                    tokens = calloc(1, sizeof(token));
                }
                tokLen++;
                //tokens[tokLen-1].type = calloc(1,1);
                //tokens[tokLen-1].value = calloc(1,1);
                if(isInclude){
                    strncpy(tokens[tokLen-1].type, "keyword", 10);
                }
                else{
                    
                    if(!isFunc){
                        strncpy(tokens[tokLen-1].type, "keyword_custom", 20);
                    }
                    else{
                        isInclude = false;
                        for (int i = 0; i < ArrLeng(BUILT_IN_FUNCS); i++) {
                            if (strcmp(BUILT_IN_FUNCS[i], res) == 0) {
                                isInclude = true;
                                break;
                            }
                            isInclude = false;
                        }
                        if(isInclude){
                            strncpy(tokens[tokLen-1].type, "function", 11);
                        }
                        else{
                            strncpy(tokens[tokLen-1].type, "function_custom", 20);
                        }
                        
                    }
                }
                strncpy(tokens[tokLen-1].value, res, 100);
                if(isFunc){
                    tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                    tokLen++;
                    strncpy(tokens[tokLen-1].type, "bracket(", 15);
                    isFunc = false;
                }
                if(brackets == 1){
                    tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                    tokLen++;
                    strncpy(tokens[tokLen-1].type, "bracket[", 15);
                }
                else if(brackets == 2){
                    tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                    tokLen++;
                    strncpy(tokens[tokLen-1].type, "bracket{", 15);
                }
                else if(brackets == 3){
                    tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                    tokLen++;
                    
                    strncpy(tokens[tokLen-1].type, "bracket)", 15);
                }
                else if(brackets == 4){
                    tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                    tokLen++;
                    strncpy(tokens[tokLen-1].type, "bracket]", 15);
                }
                else if(brackets == 5){
                    tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                    tokLen++;
                    strncpy(tokens[tokLen-1].type, "bracket}", 15);
                }
                isFunc = false;
                
            }
            else if(currentChar == '='){
                pos++;
                column++;
                if(tokLen > 0){
                    tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                }
                else{
                    tokens = calloc(1, sizeof(token));
                }
                //tokens[tokLen-1].type = calloc(1,1);
                //tokens[tokLen-1].value = calloc(1,1);
                tokLen++;
                strncpy(tokens[tokLen-1].type, "operator", 15);
                strncpy(tokens[tokLen-1].value, "eq", 15);
            }
            else{
                printf("Unexpected character %c at line %d column %d\n", codes[pos], line, column);

            }
        }
        
    }
    
}
bool parse(){
    const int len = tokLen;
    int pos = 0;
    while(pos<len){
        //-------------------print--------------------
        if(strcmp(tokens[pos].type, "function") == 0 && strcmp(tokens[pos].value, "print") == 0){
            if(tokLen <= pos+1){
                printf("Unexpected function invoke, expected (\n");
                return false;
            }
            else if(tokLen <= pos+2){
                printf("Unexpected end of line, expected value for print\n");
                return false;
            }
            int p = pos+3;
            while(strcmp(tokens[p].type, "bracket)") != 0){
                if(tokLen <= p){
                    printf("Unexpected end of line, expected )\n");
                    return false;
                }
                p++;
            }
            if(strcmp(tokens[p].type, "bracket)") != 0){
                printf("Unexpected end of function print, expected )\n");
                return false;
            }
            bool isVar = false;
            if(strcmp(tokens[pos+2].type, "keyword_custom") == 0){
                isVar = true;
            }
            bool isString = false;
            if(!isVar && strcmp(tokens[pos+2].type, "string") == 0){
                isString = true;
            }
            bool isChar = false;
            if(!isVar && strcmp(tokens[pos+2].type, "char") == 0){
                isChar = true;
            }
            bool isInt = false;
            if(!isVar && strcmp(tokens[pos+2].type, "int") == 0){
                isInt = true;
            }
            bool isFloat = false;
            if(!isVar && strcmp(tokens[pos+2].type, "float") == 0){
                isFloat = true;
            }
            if(!isString && !isVar && !isChar & !isInt && !isFloat){
                printf("Unexpected token %s\n", tokens[pos+2].type);
            }
            if(isVar){
                bool isNorm = false;
                for (int i = 0; i < varLen; i++) {
                    if (strcmp(tokens[pos+2].value, vars[i].name) == 0) {
                        isNorm = true;
                        if(vars[i].type){
                            if(strcmp(vars[i].type, "string") == 0){
                                printf("%s", vars[i].value);
                                break;
                            }
                            else if(strcmp(vars[i].type, "int") == 0){
                                printf("%d", (int)vars[i].val);
                                break;
                            }
                            else if(strcmp(vars[i].type, "u_int") == 0){
                                printf("%u", (unsigned int)vars[i].val);
                                break;
                            }
                            else if(strcmp(vars[i].type, "short") == 0){
                                printf("%hd", (short)vars[i].val);
                                break;
                            }
                            else if(strcmp(vars[i].type, "u_short") == 0){
                                printf("%hu", (unsigned short)vars[i].val);
                                break;
                            }
                            else if(strcmp(vars[i].type, "long") == 0){
                                printf("%ld", (long)vars[i].val);
                                break;
                            }
                            else if(strcmp(vars[i].type, "u_long") == 0){
                                printf("%lu", (unsigned long)vars[i].val);
                                break;
                            }
                            else if(strcmp(vars[i].type, "l_long") == 0){
                                printf("%lld", (long long)vars[i].val);
                                break;
                            }
                            else if(strcmp(vars[i].type, "ul_long") == 0){
                                printf("%llu", (unsigned long long)vars[i].val);
                                break;
                            }
                            else if(strcmp(vars[i].type, "float") == 0){
                                printf("%Lg", vars[i].val);
                                break;
                            }
                            else if(strcmp(vars[i].type, "char") == 0){
                                printf("%c", vars[i].value[0]);
                                break;
                            }
                            else if(strcmp(vars[i].type, "u_char") == 0){
                                printf("%u", vars[i].value[0]);
                                break;
                            }
                            else if(strcmp(vars[i].type, "s_char") == 0){
                                printf("%c", vars[i].value[0]);
                                break;
                            }
                            else if(strcmp(vars[i].type, "bool") == 0){
                                if(vars[i].value == true){
                                    printf("true");
                                }
                                else{
                                    printf("false");
                                }
                                break;
                            }
                        }
                        break;
                    }
                    isNorm = false;
                }
                if(!isNorm){
                    printf("Undefined variable %s", tokens[pos+2].value);
                    return false;
                }
                
            }
            else{
                if(isString){
                    printf("%s", tokens[pos+2].value);
                }
                else if(isFloat){
                    printf("%Lg", tokens[pos+2].val);
                }
                else if(isChar){
                    printf("%c", tokens[pos+2].value[0]);
                }
                else if(isInt){
                    printf("%d", (int)tokens[pos+2].val);
                }
            }
            pos += 4;
        }
        // if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "print") == 0){
        //     if(tokLen <= pos+1){
        //         printf("Unexpected end of line, expected value for print\n");
        //         return false;
        //     }
        //     bool isVar = false;
        //     if(strcmp(tokens[pos+1].type, "keyword_custom") == 0){
        //         isVar = true;
        //     }
        //     bool isString = false;
        //     if(!isVar && strcmp(tokens[pos+1].type, "string") == 0){
        //         isString = true;
        //     }
        //     bool isChar = false;
        //     if(!isVar && strcmp(tokens[pos+1].type, "char") == 0){
        //         isChar = true;
        //     }
        //     bool isInt = false;
        //     if(!isVar && strcmp(tokens[pos+1].type, "int") == 0){
        //         isInt = true;
        //     }
        //     bool isFloat = false;
        //     if(!isVar && strcmp(tokens[pos+1].type, "float") == 0){
        //         isFloat = true;
        //     }
        //     if(!isString && !isVar && !isChar & !isInt && !isFloat){
        //         printf("Unexpected token %s\n", tokens[pos+1].type);
        //     }
        //     if(isVar){
        //         bool isNorm = false;
        //         for (int i = 0; i < varLen; i++) {
        //             if (strcmp(tokens[pos+1].value, vars[i].name) == 0) {
        //                 isNorm = true;
        //                 if(vars[i].type){
        //                     if(strcmp(vars[i].type, "string") == 0){
        //                         printf("%s", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "int") == 0){
        //                         printf("%d", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "u_int") == 0){
        //                         printf("%u", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "short") == 0){
        //                         printf("%hd", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "u_short") == 0){
        //                         printf("%hu", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "long") == 0){
        //                         printf("%ld", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "u_long") == 0){
        //                         printf("%lu", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "l_long") == 0){
        //                         printf("%lld", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "ul_long") == 0){
        //                         printf("%llu", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "float") == 0){
        //                         printf("%.15g", vars[i].val);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "char") == 0){
        //                         printf("%c", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "u_char") == 0){
        //                         printf("%u", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "s_char") == 0){
        //                         printf("%c", vars[i].value);
        //                         break;
        //                     }
        //                     else if(strcmp(vars[i].type, "bool") == 0){
        //                         if(vars[i].value == true){
        //                             printf("true");
        //                         }
        //                         else{
        //                             printf("false");
        //                         }
        //                         break;
        //                     }
        //                 }
        //                 break;
        //             }
        //             isNorm = false;
        //         }
        //         if(!isNorm){
        //             printf("Undefined variable %s", tokens[pos+1].value);
        //             return false;
        //         }
                
        //     }
        //     else{
        //         if(isString){
        //             printf("%s", tokens[pos+1].value);
        //         }
        //         else if(isFloat){
        //             printf("%.15g", tokens[pos+1].val);
        //         }
        //         else if(isChar){
        //             printf("%c", tokens[pos+1].value);
        //         }
        //         else if(isInt){
        //             printf("%d", tokens[pos+1].value);
        //         }
        //     }
        //     pos+= 2;
        // }
        //-------------------var--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "var") == 0){
            bool b = createVar(pos, &pos, true);
            if(!b){
                return false;
            }
        }
        //--------var without var word---------------
        else if(strcmp(tokens[pos].type, "keyword_custom") == 0){
            bool exists = false;
            int index = 0;
            for (int i = 0; i < varLen; i++) {
                if (strcmp(vars[i].name, tokens[pos].value) == 0) {
                    exists = true;
                    index = i;
                    break;
                }
                exists = false;
            }
            bool isEq = false;
            if(exists){
                if(tokLen > pos+2){
                    if(strcmp(tokens[pos+1].type, "operator") == 0){
                        if(strcmp(tokens[pos+2].type, "keyword_custom") == 0){
                            exists = false;
                            int index2 = 0;
                            for (int i = 0; i < varLen; i++) {
                                if (strcmp(vars[i].name, tokens[pos+2].value) == 0) {
                                    exists = true;
                                    index2 = i;
                                    break;
                                }
                                exists = false;
                            }
                            if(exists){
                                // if(strcmp(vars[index2].type, "string") == 0){
                                // }
                                // else if(strcmp(vars[index2].type, "int") == 0){
                                //     vars[index].value = calloc(1, sizeof(int));
                                // }
                                // else if(strcmp(vars[index2].type, "float") == 0){
                                //     vars[index].value = calloc(51, sizeof(char));
                                // }
                                // else if(strcmp(vars[index2].type, "bool") == 0){
                                //     vars[index].value = calloc(1, sizeof(bool));
                                // }
                                // else if(strcmp(vars[index2].type, "char") == 0){
                                //     vars[index].value = calloc(1, sizeof(char));
                                // }
                                // else if(strcmp(vars[index2].type, "u_int") == 0){
                                //     vars[index].value = calloc(1, sizeof(unsigned int));
                                // }
                                // else if(strcmp(vars[index2].type, "short") == 0){
                                //     vars[index].value = calloc(1, sizeof(short));
                                // }
                                // else if(strcmp(vars[index2].type, "long") == 0){
                                //     vars[index].value = calloc(1, sizeof(long));
                                // }
                                // else if(strcmp(vars[index2].type, "u_long") == 0){
                                //     vars[index].value = calloc(1, sizeof(unsigned long));
                                // }
                                // else if(strcmp(vars[index2].type, "l_long") == 0){
                                //     vars[index].value = calloc(1, sizeof(long long));
                                // }
                                // else if(strcmp(vars[index2].type, "ul_long") == 0){
                                //     vars[index].value = calloc(1, sizeof(unsigned long long));
                                // }
                                // else if(strcmp(vars[index2].type, "u_char") == 0){
                                //     vars[index].value = calloc(1, sizeof(unsigned char));
                                // }
                                // else if(strcmp(vars[index2].type, "s_char") == 0){
                                //     vars[index].value = calloc(1, sizeof(signed char));
                                // }
                                if(strcmp(vars[index2].type, "string") == 0){
                                    strncpy(vars[index].value, vars[index2].value, 102);
                                }
                                else if(strcmp(vars[index2].type, "float") == 0 || strcmp(vars[index2].type, "int") == 0){
                                    vars[index].val = vars[index2].val;
                                }
                                else{
                                    strncpy(vars[index].value, "", 1);
                                }
                            }
                        }

                    }
                    else{
                        printf("Unexpected behaviour: expected operator\n");
                    }
                }
                else{
                    printf("Unexpected end of line when accesing variable %s\n", vars[index].name);
                }
            }
            else{
                pos--;
                bool b = createVar(pos, &pos, false);
                if(!b){
                    return false;
                }
            }
        }
        else{
            printf("Unexpected token %s\n", tokens[pos].type);
            return false;
        }
        
    }
    return true;
}
void run(){
    tokenize();
    parse();
}

int main(int argc, char* argv[]){
    if (argc >= 2){
        //printf("Welcome to the Puffin language v. 0.0.2!\nCreated by Dizabanik\n");
        char * buffer = 0;
        long length;
        FILE * f = fopen (argv[1], "rb");

        if (f)
        {
            fseek (f, 0, SEEK_END);
            length = ftell (f);
            fseek (f, 0, SEEK_SET);
            buffer = malloc (length+1);
            if (buffer)
            {
                fread (buffer, 1, length, f);
            }
            buffer[length] = '\0';
            fclose (f);
        }

        if (buffer)
        {
            codes = malloc(length+1);
            strncpy(codes, buffer, length+1);
            //printf("%c", codes[13]);
            //free(buffer);
            run();
        }
        
        
    }
    for (int i = 0; i < varLen; i++)
    {
        //free(vars[i].value);
    }
    
    return 0;
}
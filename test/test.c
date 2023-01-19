#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define ArrLeng(x)  (sizeof(x) / sizeof((x)[0]))
#include <stdbool.h>

#define C_LANG 1


short lang = 0;
bool build = false;
bool here = false;
bool debug = false;
FILE *fp;
typedef struct _argument{
    bool isTrue;
    bool isNeeded;
    int pairId;
} argument;
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
    char subtype[5];
    int bracketsToDelete;
} var;
typedef struct _valu{
    char value[101];
    char type[51];
    long double val;
    char operatorType[5];
} valu;
typedef struct _func{
    char name[51];
    int pos;
    void* value;
    char type[51];
} func;
token* tokens;
var* vars;
func* funcs;
int openedBracketsShaped = 0;
int openedBracketsRound = 0;
int openedBracketsSquare = 0;
int tokLen = 0;
int varLen = 0;
int argLen = 0;
char* codes = 0;
char *strremove(char *str, const char *sub) {
    char *p, *q, *r;
    if (*sub && (q = r = strstr(str, sub)) != NULL) {
        size_t len = strlen(sub);
        while ((r = strstr(p = r + len, sub)) != NULL) {
            while (p < r)
                *q++ = *p++;
        }
        while ((*q++ = *p++) != '\0')
            continue;
    }
    return str;
}
char* getVarValue(int pos, int* posP){
    

}
long double getVarVal(int pos, int* posP, bool toZeroBrackets){
    int bracketsRoC = 0;
    int bracketsShC = 0;
    int bracketsSqC = 0;
    if(toZeroBrackets == true){
        bracketsRoC = 1;
    }
    valu* vals;
    int count = 0;
    while(pos < tokLen){
        if(strcmp(tokens[pos].type, "bracket(") == 0){
            bracketsRoC++;
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            vals[count-1].val = getVarVal(pos+1, &pos, true);
            strcpy(vals[count-1].type, "float");
            pos++;
        }
        else if(strcmp(tokens[pos].type, "bracket)") == 0){
            if(bracketsRoC <= 0){
                break;
            }
            bracketsRoC--;
            pos++;
            if(toZeroBrackets == true){
                if(bracketsRoC == 0){
                    break;
                }
            }
        }
        else if(strcmp(tokens[pos].type, "keyword_custom") == 0){
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            bool isT = false;
            for(int i = 0; i < varLen; i ++){
                if(strcmp(vars[i].name, tokens[pos].value) == 0){
                    vals[count-1].val = vars[i].val;
                    strcpy(vals[count-1].value, vars[i].value);
                    strcpy(vals[count-1].type, vars[i].type);
                    
                    isT = true;
                    break;
                }
            }
            if(isT == false){
                printf("Variable %s not found\n", tokens[pos].value);
                break;
            }
            pos++;
        }
        else if(strcmp(tokens[pos].type, "int") == 0 || strcmp(tokens[pos].type, "float") == 0){
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            vals[count-1].val = tokens[pos].val;
            strcpy(vals[count-1].type, tokens[pos].type);
            pos++;
        }
        else if(strcmp(tokens[pos].type, "string") == 0 || strcmp(tokens[pos].type, "char") == 0){
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            // strcpy(vals[count-1].value, tokens[pos].value);
            vals[count-1].val = strtold(tokens[pos].value, NULL);
            strcpy(vals[count-1].type, tokens[pos].type);
            
            pos++;
        }
        else if(strcmp(tokens[pos].type, "operator") == 0){
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            strncpy(vals[count-1].operatorType, tokens[pos].value, 5);
            strcpy(vals[count-1].type, tokens[pos].type);
            
            pos++;
        }
        else{
            break;
        }
    }
    long double ret = 0;
    if(count > 0){
        if(count > 1){
            for(int i = 0; i < count; i++){
                if(strcmp(vals[i].type, "operator") == 0){
                    if(i > 0){
                        if(strcmp(vals[i].operatorType, "add") == 0){
                            // vals[i].val = vals[i-1].val + vals[i].val;
                            vals[i+1].val = vals[i-1].val + vals[i+1].val;
                            i++;
                        }
                        else if(strcmp(vals[i].operatorType, "sub") == 0){
                            vals[i+1].val = vals[i-1].val - vals[i+1].val;
                            i++;
                        }
                    }
                    else{
                        printf("Unexpected operator: %s\n", vals[i].operatorType);
                    }
                }
                else if(strcmp(vals[i].type, "bracket(") == 0){
                    
                }
                
            }
            ret = vals[count-1].val;
        }
        else{
            if(strcmp(vals[0].type, "int") == 0 || strcmp(vals[0].type, "float") == 0){
                ret = vals[0].val;
            }
        }
    }
    
    if(posP != NULL){
        *posP = pos;
    }
    return ret;
}
bool getValue(int pos, int* posP, valu* value, bool toZeroBrackets){
    valu val;
    int bracketsRoC = 0;
    int bracketsShC = 0;
    int bracketsSqC = 0;
    if(toZeroBrackets == true){
        bracketsRoC = 1;
    }
    valu* vals;
    int count = 0;
    while(pos < tokLen){
        if(strcmp(tokens[pos].type, "bracket(") == 0){
            bracketsRoC++;
            valu vv;
            getValue(pos+1, &pos, &vv, true);
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            vals[count-1].val = vv.val;
            strcpy(vals[count-1].type, vv.type);
            strcpy(vals[count-1].value, vv.value);
            pos++;
        }
        else if(strcmp(tokens[pos].type, "bracket)") == 0){
            if(bracketsRoC <= 0){
                break;
            }
            bracketsRoC--;
            pos++;
            if(toZeroBrackets == true){
                if(bracketsRoC == 0){
                    break;
                }
            }
        }
        else if(strcmp(tokens[pos].type, "keyword_custom") == 0){
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            bool isT = false;
            for(int i = 0; i < varLen; i ++){
                if(strcmp(vars[i].name, tokens[pos].value) == 0){
                    vals[count-1].val = vars[i].val;
                    strcpy(vals[count-1].value, vars[i].value);
                    strcpy(vals[count-1].type, vars[i].type);
                    
                    isT = true;
                    break;
                }
            }
            if(isT == false){
                printf("Variable %s not found\n", tokens[pos].value);
                break;
            }
            pos++;
        }
        else if(strcmp(tokens[pos].type, "int") == 0 || strcmp(tokens[pos].type, "float") == 0){
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            vals[count-1].val = tokens[pos].val;
            strcpy(vals[count-1].type, tokens[pos].type);
            pos++;
        }
        else if(strcmp(tokens[pos].type, "string") == 0 || strcmp(tokens[pos].type, "char") == 0){
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            strcpy(vals[count-1].value, tokens[pos].value);
            strcpy(vals[count-1].type, tokens[pos].type);
            
            pos++;
        }
        else if(strcmp(tokens[pos].type, "operator") == 0){
            if(strcmp(tokens[pos].value, "==") == 0 || strcmp(tokens[pos].value, ">") == 0 || strcmp(tokens[pos].value, "<") == 0 || strcmp(tokens[pos].value, "!=") == 0 || strcmp(tokens[pos].value, ">=") == 0 || strcmp(tokens[pos].value, "<=") == 0){
                break;
            }
            count ++;
            if(count == 1){
                vals = malloc(sizeof(valu));
            }
            else{
                vals = realloc(vals, count * sizeof(valu));
            }
            strncpy(vals[count-1].operatorType, tokens[pos].value, 5);
            strcpy(vals[count-1].type, tokens[pos].type);
            
            pos++;
        }
        else{
            break;
        }
    }
    if(count > 0){
        if(count > 1){
            for(int i = 0; i < count; i++){
                if(strcmp(vals[i].type, "operator") == 0){
                    if(i > 0){
                        if(strcmp(vals[i].operatorType, "add") == 0){
                            if(strcmp(vals[i-1].type, "string") == 0 || strcmp(vals[i-1].type, "char") == 0){
                                if(strcmp(vals[i+1].type, "int") == 0 || strcmp(vals[i+1].type, "float") == 0){
                                    sprintf(vals[i+1].value, "%Lg", vals[i+1].val);
                                }
                                strcpy(vals[i+1].type, vals[i-1].type);
                                char *tmp = strdup(vals[i+1].value);
                                
                                strcpy(tmp, vals[i-1].value);
                                
                                strcat(vals[i+1].value, tmp);
                                free(tmp);
                            }
                            else{
                                if(strcmp(vals[i+1].type, "string") == 0 || strcmp(vals[i+1].type, "char") == 0){
                                    vals[i+1].val = strtold(vals[i+1].value, NULL) + vals[i-1].val;
                                    strcpy(vals[i+1].type, vals[i-1].type);
                                }
                                else{
                                    vals[i+1].val = vals[i-1].val + vals[i+1].val;
                                    strcpy(vals[i+1].type, vals[i-1].type);
                                }
                            }
                            i++;
                        }
                        else if(strcmp(vals[i].operatorType, "sub") == 0){
                            if(strcmp(vals[i-1].type, "string") == 0 || strcmp(vals[i-1].type, "char") == 0){
                                strcpy(vals[i+1].type, vals[i-1].type);
                                strcpy(vals[i+1].value, strremove(vals[i-1].value, vals[i+1].value));
                            }
                            else{
                                
                                if(strcmp(vals[i+1].type, "string") == 0 || strcmp(vals[i+1].type, "char") == 0){
                                    vals[i+1].val = vals[i-1].val - strtold(vals[i+1].value, NULL) ;
                                    
                                }
                                else{
                                    vals[i+1].val = vals[i-1].val - vals[i+1].val;
                                }
                                strcpy(vals[i+1].type, vals[i-1].type);
                            }
                            i++;
                        }
                    }
                    else{
                        printf("Unexpected operator: %s\n", vals[i].operatorType);
                    }
                }
                else if(strcmp(vals[i].type, "bracket(") == 0){
                    
                }
                
            }
            strcpy(val.type, vals[count-1].type);
            strcpy(val.value, vals[count-1].value);
            val.val = vals[count-1].val;
        }
        else{
            strcpy(val.type, vals[0].type);
            strcpy(val.value, vals[0].value);
            val.val = vals[0].val;
        }
    }
    strcpy(value->type, val.type);
    strcpy(value->value, val.value);
    value->val = val.val;
    if(posP != NULL){
        *posP = pos;
    }
    return true;
}

bool getArgument(argument* arg, int posI, int* posP){
    valu val;
    int pos = posI;
    getValue(pos, &pos, &val, false);
    if(strcmp(val.type, "int") == 0 || strcmp(val.type, "float") == 0){
        if(strcmp(tokens[pos].type, "operator") == 0){
            if(strcmp(tokens[pos].value, "==") == 0){
                if(val.val == getVarVal(pos+1, &pos, false)){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
            else if(strcmp(tokens[pos].value, "!=") == 0){
                if(val.val == getVarVal(pos+1, &pos, false)){
                    arg->isTrue = false;
                }
                else{
                    arg->isTrue = false;
                }
            }
            else if(strcmp(tokens[pos].value, ">") == 0){
                if(val.val > getVarVal(pos+1, &pos, false)){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
            else if(strcmp(tokens[pos].value, "<") == 0){
                if(val.val < getVarVal(pos+1, &pos, false)){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
            else if(strcmp(tokens[pos].value, "<=") == 0){
                if(val.val <= getVarVal(pos+1, &pos, false)){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
            else if(strcmp(tokens[pos].value, ">=") == 0){
                if(val.val >= getVarVal(pos+1, &pos, false)){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
        }
    }
    else if(strcmp(val.type, "string") == 0 || strcmp(val.type, "char") == 0){
        if(strcmp(tokens[pos].type, "operator") == 0){
            if(strcmp(tokens[pos].value, "==") == 0){
                if(strcmp(val.value, getVarValue(pos+1, &pos)) == 0){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
            else if(strcmp(tokens[pos].value, "!=") == 0){
                if(strcmp(val.value, getVarValue(pos+1, &pos)) != 0){
                    arg->isTrue = false;
                }
                else{
                    arg->isTrue = true;
                }
            }
            else if(strcmp(tokens[pos].value, ">") == 0){
                if(strtold(val.value, NULL) > getVarVal(pos+1, &pos, false)){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
            else if(strcmp(tokens[pos].value, "<") == 0){
                if(strtold(val.value, NULL) < getVarVal(pos+1, &pos, false)){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
            else if(strcmp(tokens[pos].value, "<=") == 0){
                if(strtold(val.value, NULL) <= getVarVal(pos+1, &pos, false)){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
            else if(strcmp(tokens[pos].value, ">=") == 0){
                if(strtold(val.value, NULL) >= getVarVal(pos+1, &pos, false)){
                    arg->isTrue = true;
                }
                else{
                    arg->isTrue = false;
                }
            }
        }
    }
    return true;
}

bool createVar(int pos, int *posi, bool withKeyword, char *subtype, int bracketsDepthToDelete){
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
    strncpy(varName, tokens[pos+1].value, 50);
    bool isEq = false;
    bool isEqAdd = false;
    bool isEqSub = false;
    bool isOper = false;
    if(tokLen > pos+2 && strcmp(tokens[pos+2].type, "operator") == 0){
        if(strcmp(tokens[pos+2].value, "eq") == 0){
            isEq = true;
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
    bool exists= false;
    for (int i = 0; i < varLen; i++) {
        if (strcmp(vars[i].name, varName) == 0) {
            exists = true;
            break;
        }
    }
    if(exists == true){
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
    strncpy(vars[varLen-1].name, varName, 51);
    valu val;
    bool vv = getValue(pos+3, &pos, &val, false);
    strncpy(vars[varLen-1].value, val.value, 101);
    strncpy(vars[varLen-1].type, val.type, 51);
    vars[varLen-1].val = val.val;
    *posi = pos;
    return true;
}
void tokenize(){
    const int length = strlen(codes);
    int pos = 0;
    //s8 - signed int 8
    //s16 - signed int 16
    //s32 - signed int 32
    //i8 - int 8
    //i16 - int 16
    //i32 - int 32
    //u8 - unsigned int 8
    //u16 - unsigned int 16
    //u32 - unsigned int 32
    char BUILT_IN_KEYWORDS[10][10] = {{"var"}, {"s8"}, {"s16"}, {"s32"}, {"i8"}, {"i16"}, {"i32"}, {"u8"}, {"u16"}, {"u32"}};
    char BUILT_IN_FUNCS[2][20] = {{"print"}, {"println"}};
    char BUILT_IN_STATEMENTS[1][20] = {{"if"}};
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
        else if(currentChar == '+'){
            
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            strncpy(tokens[tokLen-1].type, "operator", 15);
            strncpy(tokens[tokLen-1].value, "add", 5);
            pos++;
        }
        else if(currentChar == '-'){
            
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            strncpy(tokens[tokLen-1].type, "operator", 15);
            strncpy(tokens[tokLen-1].value, "sub", 5);
            pos++;
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
                    char ze[2] = "0";
                    strncat(res, &ze[0], 1);
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
                // if(pos < length){
                //     //printf("Hello\n");
                // }
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
                            goto gt1;
                        }
                        else if(codes[pos] == '['){
                            brackets = 1;
                            pos++;
                            column++;
                            goto gt1;
                        }
                        else if(codes[pos] == '{'){
                            brackets = 2;
                            pos++;
                            column++;
                            goto gt1;
                        }
                        else if(codes[pos] == ')'){
                            brackets = 3;
                            pos++;
                            column++;
                            goto gt1;
                        }
                        else if(codes[pos] == ']'){
                            brackets = 4;
                            pos++;
                            column++;
                            goto gt1;
                        }
                        else if(codes[pos] == '}'){
                            brackets = 5;
                            pos++;
                            column++;
                            goto gt1;
                        }
                        else{
                            isInclude = false;
                        }
                    }
                    
                }
                gt1: isInclude = false;
                bool isStatement = false;
                for (int i = 0; i < ArrLeng(BUILT_IN_KEYWORDS); i++) {
                    if (strcmp(BUILT_IN_KEYWORDS[i], res) == 0) {
                        isInclude = true;
                        break;
                    }
                    isInclude = false;
                }
                if(!isInclude){
                    for (int i = 0; i < ArrLeng(BUILT_IN_STATEMENTS); i++) {
                        if (strcmp(BUILT_IN_STATEMENTS[i], res) == 0) {
                            isStatement = true;
                            break;
                        }
                        isStatement = false;
                    }
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
                    if(isStatement){
                        strncpy(tokens[tokLen-1].type, "statement", 15);
                    }
                    else if(!isFunc){
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
                strncpy(tokens[tokLen-1].value, res, 101);
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
                if(codes[pos] != '='){
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
                    strncpy(tokens[tokLen-1].value, "==", 6);
                }
            }
            else if(currentChar == '>'){
                pos++;
                column++;
                if(codes[pos] == '='){
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
                    strncpy(tokens[tokLen-1].value, ">=", 6);
                }
                else{
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
                    strncpy(tokens[tokLen-1].value, ">", 6);
                }
            }
            else if(currentChar == '<'){
                pos++;
                column++;
                if(codes[pos] == '='){
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
                    strncpy(tokens[tokLen-1].value, "<=", 6);
                }
                else{
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
                    strncpy(tokens[tokLen-1].value, "<", 6);
                }
            }
            else if(currentChar == '!'){
                pos++;
                column++;
                if(codes[pos] == '='){
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
                    strncpy(tokens[tokLen-1].value, "!=", 6);
                }
                else{
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
                    strncpy(tokens[tokLen-1].value, "!", 3);
                }
            }
            else if(currentChar == '&'){
                pos++;
                column++;
                if(codes[pos] == '&'){
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
                    strncpy(tokens[tokLen-1].value, "&&", 6);
                }
            }
            else if(currentChar == '|'){
                pos++;
                column++;
                if(codes[pos] == '|'){
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
                    strncpy(tokens[tokLen-1].value, "||", 6);
                }
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
        if(strcmp(tokens[pos].type, "function") == 0 && strcmp(tokens[pos].value, "print") == 0 || strcmp(tokens[pos].value, "println") == 0){
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
                            // else if(strcmp(vars[i].type, "bool") == 0){
                            //     if(vars[i].value == true){
                            //         printf("true");
                            //     }
                            //     else{
                            //         printf("false");
                            //     }
                            //     break;
                            // }
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
            if(strcmp(tokens[pos].value, "println") == 0){
                printf("\n");
            }
            pos += 4;
        }
        //----------------------if statement--------------------------
        else if(strcmp(tokens[pos].type, "statement") == 0 && strcmp(tokens[pos].value, "if") == 0){
            if(tokLen <= pos+1){
                printf("Unexpected statement start, expected (\n");
                return false;
            }
            if(strcmp(tokens[pos+1].type, "bracket(") != 0){
                printf("Unexpected statement start, expected (\n");
                return false;
            }
            pos+=2;
            if(tokLen <= pos){  
                printf("Statement arguments not found\n");
                return false;
            }
            int brackets = 1;
            int argsCount = 1;  
            argument* arg;
            arg = malloc(sizeof(argument));
            arg[argsCount-1].isNeeded = true;
            getArgument(&arg[argsCount-1], pos, &pos);

            while(brackets > 0 && tokLen > pos){
                if(strcmp(tokens[pos].type, "bracket(") == 0){
                    brackets++;
                }
                else if(strcmp(tokens[pos].type,"bracket)") == 0){
                    brackets--;
                }
                if(strcmp(tokens[pos].type, "operator") == 0){
                    if(strcmp(tokens[pos].value, "&&") == 0){
                        if(tokLen <= pos+1){
                            printf("Not enough arguments in statement after && operator\n");
                            return false;
                        }
                        argsCount++;
                        arg = realloc(arg, sizeof(argument) * argsCount);
                        arg[argsCount-1].isNeeded = true;
                        getArgument(&arg[argsCount-1], pos, &pos);
                    }
                    else if(strcmp(tokens[pos].value, "||") == 0){
                        if(tokLen <= pos+1){
                            printf("Not enough arguments in statement after || operator\n");
                            return false;
                        }
                        argsCount++;
                        arg = realloc(arg, sizeof(argument) * argsCount);
                        arg[argsCount-1].isNeeded = false;
                        getArgument(&arg[argsCount-1], pos, &pos);
                    }
                }
                pos++;
            }
            if(strcmp(tokens[pos].type, "bracket{") != 0){
                printf("Expected { bracket\n");
                return false;
            }
            pos++;
            int bracketsCount2 = 1;
            for(int i = 0; i < argsCount; i++){
                if(arg[i].isNeeded == true){
                    if(arg[i].isTrue == false){
                        while (bracketsCount2 > 0 && pos < tokLen)
                        {
                            if(strcmp(tokens[pos].type, "bracket}") == 0){
                                bracketsCount2--;
                            }
                            else if(strcmp(tokens[pos].type, "bracket{") == 0){
                                bracketsCount2++;
                            }
                            pos++;
                        }
                        break;
                    }
                    else{
                        openedBracketsShaped++;
                    }
                }
                else{
                    if(i < argsCount-1){
                        if(arg[i+1].isTrue == false){
                            while (bracketsCount2 > 0 && pos < tokLen)
                            {
                                if(strcmp(tokens[pos].type, "bracket}") == 0){
                                    bracketsCount2--;
                                }
                                else if(strcmp(tokens[pos].type, "bracket{") == 0){
                                    bracketsCount2++;
                                }
                                pos++;
                            }
                            break;
                        }
                        else{
                            openedBracketsShaped++;
                        }
                        i++;
                    }
                    else{
                        while (bracketsCount2 > 0 && pos < tokLen)
                        {
                            if(strcmp(tokens[pos].type, "bracket}") == 0){
                                bracketsCount2--;
                            }
                            else if(strcmp(tokens[pos].type, "bracket{") == 0){
                                bracketsCount2++;
                            }
                            pos++;
                        }
                        break;
                    }
                }
            }
        }
        else if(strcmp(tokens[pos].type, "bracket}") == 0){
            if(openedBracketsShaped > 0){
                openedBracketsShaped--;
            }
            else{
                printf("Unexpected bracket }\n");
            }
            pos++;
        }
        else if(strcmp(tokens[pos].type, "bracket)") == 0){
            if(openedBracketsRound > 0){
                openedBracketsRound--;
            }
            else{
                printf("Unexpected bracket )\n");
            }
            pos++;
        }
        else if(strcmp(tokens[pos].type, "bracket]") == 0){
            if(openedBracketsSquare > 0){
                openedBracketsSquare--;
            }
            else{
                printf("Unexpected bracket ]\n");
            }
            pos++;
        }
        else if(strcmp(tokens[pos].type, "bracket{") == 0){
            openedBracketsShaped++;
            pos++;
        }
        else if(strcmp(tokens[pos].type, "bracket(") == 0){
            openedBracketsRound++;
            pos++;
        }
        else if(strcmp(tokens[pos].type, "bracket[") == 0){
            openedBracketsSquare++;
            pos++;
        }
        //-------------------var--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "var") == 0){
            bool b = createVar(pos, &pos, true, NULL, openedBracketsShaped-1);
            if(!b){
                return false;
            }
        }
        //-------------------i8--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "i8") == 0){
            bool b = createVar(pos, &pos, true, "i8", openedBracketsShaped-1);
            if(!b){
                return false;
            }
        }
        //-------------------i16--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "i16") == 0){
            bool b = createVar(pos, &pos, true, "i16", openedBracketsShaped-1);
            if(!b){
                return false;
            }
        }
        //-------------------i32--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "i32") == 0){
            bool b = createVar(pos, &pos, true, "i32", openedBracketsShaped-1);
            if(!b){
                return false;
            }
        }
        //-------------------u8--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "u8") == 0){
            bool b = createVar(pos, &pos, true, "u8", openedBracketsShaped-1);
            if(!b){
                return false;
            }
        }
        //-------------------u16--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "u16") == 0){
            bool b = createVar(pos, &pos, true, "u16", openedBracketsShaped-1);
            if(!b){
                return false;
            }
        }
        //-------------------u32--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "u32") == 0){
            bool b = createVar(pos, &pos, true, "u32", openedBracketsShaped-1);
            if(!b){
                return false;
            }
        }
        //-------------------s8--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "s8") == 0){
            bool b = createVar(pos, &pos, true, "u8", openedBracketsShaped-1);
            if(!b){
                return false;
            }
        }
        //-------------------s16--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "s16") == 0){
            bool b = createVar(pos, &pos, true, "u16", openedBracketsShaped-1);
            if(!b){
                return false;
            }
        }
        //-------------------s32--------------------
        else if(strcmp(tokens[pos].type, "keyword") == 0 && strcmp(tokens[pos].value, "s32") == 0){
            bool b = createVar(pos, &pos, true, "u32", openedBracketsShaped-1);
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
                        if(strcmp(tokens[pos+1].value, "eq") == 0){
                            valu val;
                            if(getValue(pos+2, &pos, &val, false) != true){
                                printf("Error getting value\n");
                            }
                            vars[index].val = val.val;
                            strcpy(vars[index].value, val.value);
                            if(strcmp(vars[index].subtype, "") == 0 || vars[index].subtype == NULL){
                                strcpy(vars[index].type, val.type);
                            }
                        }
                    }
                }
            }
            else{
                pos--;
                bool b = createVar(pos, &pos, false, NULL, openedBracketsShaped-1);
                if(!b){
                    return false;
                }
            }
        }
        else{
            printf("Unexpected token of type %s and with value %s\n", tokens[pos].type, tokens[pos].value);
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
    char* version = "dev0.0.2.1";
    if (argc >= 2){
        for (int i = 1; i < argc; i++)
        {
            if(strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0){
                debug = true;
            }
            else if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0){
                printf("%s\n", version);
            }
            else if(strcmp(argv[i], "--build") == 0 || strcmp(argv[i], "-b") == 0){
                build = true;
            }
            else if(strcmp(argv[i], "--here") == 0 || strcmp(argv[i], "-h") == 0){
                here = true;
            }
            else{
                char * buffer = 0;
                long length;
                FILE * f = fopen (argv[i], "rb");
                if(debug){
                    printf("Opened file %s.\n", argv[i]);
                }
                if (f)
                {
                    fseek (f, 0, SEEK_END);
                    length = ftell (f);
                    if(debug){
                        printf("File %s length: %d.\n", argv[1], length);
                    }
                    fseek (f, 0, SEEK_SET);
                    buffer = malloc (length+1);
                    if (buffer)
                    {
                        fread (buffer, 1, length, f);
                    }
                    buffer[length] = '\0';
                    if(debug){
                        printf("File %s has been readed.\n", argv[1]);
                    }
                    fclose (f);
                }

                if (buffer)
                {
                    codes = malloc(length+1);
                    strncpy(codes, buffer, length+1);
                    if(debug){
                        printf("File %s has been added to codes.\n", argv[i]);
                    }
                    //printf("%c", codes[13]);
                    free(buffer);
                    if(debug){
                        printf("Freed buffer.\n");
                    }
                    
                }
            }
        
        }
        run();
        
        //printf("Welcome to the Puffin language v. 0.0.2!\nCreated by Dizabanik\n");
        
        
    }
    // for (int i = 0; i < varLen; i++)
    // {
    //     free(vars[i].value);
    //     if(debug){
    //         printf("Freed var value.\n");
    //     }
    // }
    if(debug){
        printf("\nDEBUG FREE\n");
    }
    free(vars);
    if(debug){
        printf("Freed all vars.\n");
    }
    free(funcs);
    if(debug){
        printf("Freed all funcs.\n");
    }
    free(tokens);
    if(debug){
        printf("Freed all tokens.\n");
    }
    free(codes);
    if(debug){
        printf("Freed all codes.\n");
    }
    

    return 0;
}
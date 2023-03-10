#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define ArrLeng(x)  (sizeof(x) / sizeof((x)[0]))

#define C_LANG 1

#define MAX_VALUES 256
#define MAX_COMMAND_LENGTH 512
#define MAX_ARGS 128
#define MAX_STR_LENGTH 101

#define ARG_UNDEFINED 0
#define ARG_FALSE 1
#define ARG_TRUE 2

#define SYNTAX_ERROR 0
#define RUNTIME_ERROR 1

#define USE_KW_TYPES
#define KW_INT 1
#define KW_FLOAT 2
#define KW_CHAR 3
#define KW_STRING 4
#define KW_BRACKET_ROUNDED_OPENED 5
#define KW_BRACKET_ROUNDED_CLOSED 6
#define KW_BRACKET_SHAPED_OPENED 7
#define KW_BRACKET_SHAPED_CLOSED 8
#define KW_BRACKET_SQUARED_OPENED 9
#define KW_BRACKET_SQUARED_CLOSED 10
#define KW_KEY 11
#define KW_KEY_CUSTOM 12
#define KW_OPERATOR 13
#define KW_FUN 14
#define KW_FUN_CUSTOM 15
#define KW_STATEMENT 16
#define KW_UNDEF 0


#pragma region Random


#pragma endregion
short lang = 0;
bool build = false;
bool here = false;
bool debug = false;
bool forceBuild = false;
FILE *fp;
uint_fast8_t lastArg = ARG_UNDEFINED;
bool supportColors = false;
typedef struct _argument{
    bool isTrue;
    bool isNeeded;
    int pairId;
} argument;
typedef struct _farg{
    char* name;
    void* value;
    char* type;
    char* subtype;
} farg;
typedef struct _token{
    uint_fast8_t type;
    char value[101];
    long double val;
    int line;
} token;

typedef struct _var{
    char name[51];
    char value[101];
    int type;
    long double val;
    void* vval;
    char subtype[7];
    int bracketsToDelete;
    char pretype[5];
    bool isFunc;
    int funcPos;
    int fargsCount;
    farg* fargs;
} var;
typedef struct _valu{
    char value[101];
    int type;
    long double val;
    void* vval;
    char operatorType[5];
    char subtype[7];
} valu;
token* tokens;
var* vars;
int openedBracketsShaped = 0;
int openedBracketsRound = 0;
int openedBracketsSquare = 0;
int tokLen = 0;
int varLen = 0;
int argLen = 0;
char* codes = 0;



void err_start(int error_type){
    if(supportColors == true){
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    printf("\nError");
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf(": ");
    if(error_type == SYNTAX_ERROR){
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        printf("SyntaxError");
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        printf(": ");
    }
    else if(error_type == RUNTIME_ERROR){
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        printf("RuntimeError");
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        printf(": ");
    }
#else
    if(error_type == SYNTAX_ERROR){
        printf("\n\x1b[31mError\x1b[0m: \x1b[31mSyntaxError\x1b[0m: ");
    }
    else if(error_type == RUNTIME_ERROR){
        printf("\n\x1b[31mError\x1b[0m: \x1b[31mRuntimeError\x1b[0m: ");
    }
    else{
        printf("\n\x1b[31mError\x1b[0m: ");
    }
   
#endif
    }
    else{
        if(error_type == SYNTAX_ERROR){
            printf("\nError: SyntaxError: ");
        }
        else if(error_type == RUNTIME_ERROR){
            printf("\nError: RuntimeError: ");
        }
        else{
            printf("\nError: ");
        }
    }
}

void err_end(int line){
    if(line > 0){
        if(supportColors == true){
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
        printf("(line %d)\n", line);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    #else
        printf("\x1b[36m(line %d)\x1b[0m\n", line);
    #endif
        }
        else{
            printf("(line %d)\n", line);
        }
    }
}


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
bool valSetValu(void* v, valu* val){
    
}
bool getValue(int pos, int* posP, valu* value, bool toZeroBrackets){
    valu val;
    int bracketsRoC = 0;
    int bracketsShC = 0;
    int bracketsSqC = 0;
    if(toZeroBrackets == true){
        bracketsRoC = 1;
    }
    valu vals[MAX_VALUES];
    int count = 0;
    while(pos < tokLen){
        if(tokens[pos].type == KW_BRACKET_ROUNDED_OPENED){
            bracketsRoC++;
            valu vv;
            getValue(pos+1, &pos, &vv, true);
            count ++;
            // if(count == 1){
            //     vals = malloc(sizeof(valu));
            // }
            // else{
            //     vals = realloc(vals, count * sizeof(valu));
            // }
            vals[count-1].val = vv.val;
            vars[count-1].type = vv.type;
            strcpy(vals[count-1].value, vv.value);
            pos++;
        }
        else if(tokens[pos].type == KW_BRACKET_ROUNDED_CLOSED){
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
        else if(tokens[pos].type == KW_KEY_CUSTOM){
            
            // if(count == 1){
            //     vals = malloc(sizeof(valu));
            // }
            // else{
            //     vals = realloc(vals, count * sizeof(valu));
            // }
            bool isT = false;
            for(int i = 0; i < varLen; i ++){
                if(strcmp(vars[i].name, tokens[pos].value) == 0){
                    count ++;
                    vals[count-1].val = vars[i].val;
                    strcpy(vals[count-1].value, vars[i].value);
                    vals[count-1].type = vars[i].type;
                    strcpy(vals[count-1].subtype, vars[i].subtype);
                    isT = true;
                    break;
                }
            }
            if(isT == false){
                // printf("Variable %s not found\n", tokens[pos].value);
                break;
            }
            pos++;
        }
        else if(tokens[pos].type == KW_INT || tokens[pos].type == KW_FLOAT){
            count ++;
            // if(count == 1){
            //     vals = malloc(sizeof(valu));
            // }
            // else{
            //     vals = realloc(vals, count * sizeof(valu));
            // }
            vals[count-1].val = tokens[pos].val;
            vals[count-1].type = tokens[pos].type;
            pos++;
        }
        else if(tokens[pos].type == KW_STRING){
            count ++;
            // if(count == 1){
            //     vals = malloc(sizeof(valu));
            // }
            // else{
            //     vals = realloc(vals, count * sizeof(valu));
            // }
            strcpy(vals[count-1].value, tokens[pos].value);
            vals[count-1].type = tokens[pos].type;
            pos++;
        }
        else if(tokens[pos].type == KW_CHAR){
            count ++;
            // if(count == 1){
            //     vals = malloc(sizeof(valu));
            // }
            // else{
            //     vals = realloc(vals, count * sizeof(valu));
            // }
            strcpy(vals[count-1].value, " ");
            vals[count-1].value[0] = tokens[pos].value[0];
            vals[count-1].type = tokens[pos].type;
            pos++;
        }
        else if(tokens[pos].type == KW_OPERATOR){
            if(strcmp(tokens[pos].value, "==") == 0 || strcmp(tokens[pos].value, ">") == 0 || strcmp(tokens[pos].value, "<") == 0 || strcmp(tokens[pos].value, "!=") == 0 || strcmp(tokens[pos].value, ">=") == 0 || strcmp(tokens[pos].value, "<=") == 0 || strcmp(tokens[pos].value, "&&") == 0 || strcmp(tokens[pos].value, "||") == 0){
                break;
            }
            count ++;
            // if(count == 1){
            //     vals = malloc(sizeof(valu));
            // }
            // else{
            //     vals = realloc(vals, count * sizeof(valu));
            // }
            strncpy(vals[count-1].operatorType, tokens[pos].value, 5);
            vals[count-1].type = tokens[pos].type;
            
            pos++;
        }
        else{
            break;
        }
    }
    if(count > 0){
        if(count > 1){
            for(int i = 0; i < count; i++){
                if(vals[i].type == KW_OPERATOR){
                    
                    if(i > 0){
                        if(strcmp(vals[i].operatorType, "add") == 0){
                            if(vals[i-1].type == KW_STRING || vals[i-1].type == KW_CHAR){
                                if(vals[i+1].type == KW_INT || vals[i+1].type == KW_FLOAT){
                                    sprintf(vals[i+1].value, "%Lg", vals[i+1].val);
                                }
                                vals[i+1].type = vals[i-1].type;
                                char str3[202];
                                strncpy(str3, vals[i-1].value, 101);
                                strncat(str3, vals[i+1].value, 101); 
                                
                                strncpy(vals[i+1].value, str3, 101);
                                
                            }
                            else{
                                if(vals[i+1].type == KW_STRING || vals[i+1].type == KW_CHAR){
                                    vals[i+1].val = strtold(vals[i+1].value, NULL) + vals[i-1].val;
                                    vals[i+1].type = vals[i-1].type;
                                }
                                else{
                                    vals[i+1].val = vals[i-1].val + vals[i+1].val;
                                    vals[i+1].type = vals[i-1].type;
                                }
                            }
                            i++;
                        }
                        else if(strcmp(vals[i].operatorType, "sub") == 0){
                            
                            if(vals[i-1].type == KW_STRING || vals[i-1].type == KW_CHAR){
                                vals[i+1].type = vals[i-1].type;
                                strcpy(vals[i+1].value, strremove(vals[i-1].value, vals[i+1].value));
                            }
                            else{
                                if(vals[i+1].type == KW_STRING || vals[i+1].type == KW_CHAR){
                                    vals[i+1].val = vals[i-1].val - strtold(vals[i+1].value, NULL);
                                    
                                }
                                else{
                                    
                                    vals[i+1].val = vals[i-1].val - vals[i+1].val;
                                }
                                vals[i+1].type = vals[i-1].type;
                            }
                            i++;
                        }
                    }
                    else{
                        err_start(SYNTAX_ERROR);
                        printf("Unexpected operator %s ", vals[i].operatorType);
                        err_end(-1);
                        //free(vals);
                        return false;
                    }
                }
                
            }
            val.type = vals[count-1].type;
            strcpy(val.value, vals[count-1].value);
            val.val = vals[count-1].val;
        }
        else{
            val.type = vals[0].type;
            strcpy(val.value, vals[0].value);
            val.val = vals[0].val;
        }
    }
    value->type = val.type;
    strcpy(value->value, val.value);
    value->val = val.val;
    
    if(posP != NULL){
        *posP = pos;
    }
    //free(vals);
    return true;
}


char* getVarValue(int pos, int* posP, bool toZeroBrackets){
    valu val;
    getValue(pos, posP, &val, toZeroBrackets);
    if(val.type == KW_FLOAT || val.type == KW_INT){
        char strc[MAX_STR_LENGTH] = "";
        sprintf(strc, "%Lg", val.val);
        char* res = malloc(strlen(strc) + 1);
        strcpy(res, strc);
        return res;
    }
    else if(val.type == KW_STRING || val.type == KW_CHAR){
        char* res = malloc(strlen(val.value) + 1);
        strcpy(res, val.value);
        return res;
    }
}
long double getVarVal(int pos, int* posP, bool toZeroBrackets){
    valu val;
    getValue(pos, posP, &val, toZeroBrackets);
    if(val.type == KW_FLOAT || val.type == KW_INT){
        return val.val;
    }
    else if(val.type == KW_STRING || val.type == KW_CHAR){
        return strtold(val.value, NULL);
    }
}

bool compareVals(valu* val1, valu* val2, char* operator){
    if(val1->type == KW_INT || val1->type == KW_FLOAT){
        if(val2->type == KW_STRING || val2->type == KW_CHAR){
            val1->val = strtold(val2->value, NULL);
        }
        if(strcmp(operator, "==") == 0){
            if(val1->val == val2->val){
                return true;
            }
            else{
                return false;
            }
        }
        else if(strcmp(operator, "!=") == 0){
            if(val1->val == val2->val){
                return false;
            }
            else{
                return true;
            }
        }
        else if(strcmp(operator, ">=") == 0){
            if(val1->val >= val2->val){
                return true;
            }
            else{
                return false;
            }
        }
        else if(strcmp(operator, "<=") == 0){
            if(val1->val <= val2->val){
                return true;
            }
            else{
                return false;
            }
        }
        else if(strcmp(operator, ">") == 0){
            if(val1->val > val2->val){
                return true;
            }
            else{
                return false;
            }
        }
        else if(strcmp(operator, "<") == 0){
            if(val1->val < val2->val){
                return true;
            }
            else{
                return false;
            }
        }
        else{
            err_start(SYNTAX_ERROR);
            printf("Unexpected operator %s\n", operator);
            err_end(-1);
            return false;
        }
    }
    else if(val1->type == KW_STRING && val1->type == KW_CHAR){
        if(val2->type == KW_INT || val2->type == KW_FLOAT){
            sprintf(val2->value, "%Lg", val2->val);
        }
        if(strcmp(operator, "==") == 0){
            if(strcmp(val1->value, val2->value) == 0){
                return true;
            }
            else{
                return false;
            }
        }
        else if(strcmp(operator, "!=") == 0){
            if(strcmp(val1->value, val2->value) != 0){
                return true;
            }
            else{
                return false;
            }
        }
        else if(strcmp(operator, ">=") == 0){
            if(strlen(val1->value) >= strlen(val2->value)){
                return true;
            }
            else{
                return false;
            }
        }
        else if(strcmp(operator, "<=") == 0){
            if(strlen(val1->value) <= strlen(val2->value)){
                return true;
            }
            else{
                return false;
            }
        }
        else if(strcmp(operator, ">") == 0){
            if(strlen(val1->value) > strlen(val2->value)){
                return true;
            }
            else{
                return false;
            }
        }
        else if(strcmp(operator, "<") == 0){
            if(strlen(val1->value) < strlen(val2->value)){
                return true;
            }
            else{
                return false;
            }
        }
        else{
            err_start(SYNTAX_ERROR);
            printf("Unexpected operator %s\n", operator);
            err_end(-1);
            return false;
        }
    }
}

bool getArgument(int posI, int* posP){
    valu val;
    valu val2;
    int pos = posI;
    getValue(pos, &pos, &val, false);
    int pos2 = pos;
    getValue(pos+1, &pos, &val2, false);
    bool result = compareVals(&val, &val2, tokens[pos2].value);
    *posP = pos;
    return result;
}

bool calculateSubtype(char *subtype, var* vari, int pos, int* posi, bool pasteSubtype, bool withOper){
    if(strcmp(subtype, "u32") == 0){
        if(withOper){
            vari->val = (uint_fast32_t)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "u16") == 0){
        if(withOper){
            vari->val = (uint_fast16_t)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "u8") == 0){
        if(withOper){
            vari->val = (uint_fast8_t)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "i32") == 0){
        if(withOper){
            vari->val = (int_fast32_t)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "i16") == 0){
        if(withOper){
            vari->val = (int_fast16_t)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "i8") == 0){
        if(withOper){
            vari->val = (int_fast8_t)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "double") == 0){
        if(withOper){
            vari->val = (double)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0.0;
        }
        vars[varLen-1].type = KW_FLOAT;
    }
    else if(strcmp(subtype, "float") == 0){
        if(withOper){
            vari->val = (float)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0.0;
        }
        vars[varLen-1].type = KW_FLOAT;
    }
    else if(strcmp(subtype, "ldob") == 0){
        if(withOper){
            vari->val = getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0.0;
        }
        vars[varLen-1].type = KW_FLOAT;
    }
    else if(strcmp(subtype, "long") == 0){
        if(withOper){
            vari->val = (long)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "llong") == 0){
        if(withOper){
            vari->val = (long long)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "int") == 0){
        if(withOper){
            vari->val = (int)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "si") == 0){
        if(withOper){
            vari->val = (signed int)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "ui") == 0){
        if(withOper){
            vari->val = (unsigned int)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "u64") == 0){
        if(withOper){
            vari->val = (uint_fast64_t)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "i64") == 0){
        if(withOper){
            vari->val = (int_fast64_t)getVarVal(pos, &pos, false);
        }
        else{
            vari->val = 0;
        }
        vars[varLen-1].type = KW_INT;
    }
    else if(strcmp(subtype, "mut") == 0){
        if(withOper){
            
            valu val;
            bool vv = getValue(pos+3, &pos, &val, false);
            strncpy(vars[varLen-1].value, val.value, 101);
            vars[varLen-1].type = val.type;
            vari->val = val.val;
        }
        else{
            strncpy(vars[varLen-1].value, "", 101);
            vars[varLen-1].type = KW_UNDEF;
            vari->val = 0;
        }
    }
    else if(strcmp(subtype, "sc") == 0){
        if(withOper){
            strncpy(vari->value, getVarValue(pos, &pos, false), 1);
        }
        else{
            strncpy(vari->value, "", 1);
        }
        vars[varLen-1].type = KW_CHAR;
    }
    else if(strcmp(subtype, "uc") == 0){
        if(withOper){
            strncpy(vari->value, getVarValue(pos, &pos, false), 1);
        }
        else{
            strncpy(vari->value, "", 1);
        }
        vars[varLen-1].type = KW_CHAR;
    }
    else if(strcmp(subtype, "char") == 0){
        if(withOper){
            strncpy(vari->value, getVarValue(pos, &pos, false), 1);
        }
        else{
            strncpy(vari->value, "", 1);
        }
        vars[varLen-1].type = KW_CHAR;
    }
    else if(strcmp(subtype, "str") == 0){
        if(withOper){
            strncpy(vari->value, getVarValue(pos, &pos, false), 101);
        }
        else{
            strncpy(vari->value, "", 1);
        }
        vars[varLen-1].type = KW_STRING;
    }
    else{

    }
    if(pasteSubtype == true){
        strncpy(vari->subtype, subtype, 7);
    }
    *posi = pos;
}

bool createVar(int pos, int *posi, bool withKeyword, char *subtype, int bracketsDepthToDelete, char *pretype, bool isF){
    if(withKeyword){
        bool isCustomKW = false;
        if(tokLen > pos+1 && (tokens[pos+1].type == KW_KEY_CUSTOM || tokens[pos+1].type == KW_FUN_CUSTOM)){
            isCustomKW = true;
        }
        if(!isCustomKW){
            if(tokLen <= pos+1){
                err_start(SYNTAX_ERROR);
                printf("Unexpected end of line, expected variable name ");
                err_end(tokens[pos].line);
                return false;
            }
            err_start(SYNTAX_ERROR);
            printf("Unexpected token %s, expected variable name ", tokens[pos+1].value);
            err_end(tokens[pos+1].line);
            return false;
        }
    }
    char varName[50] = "";
    strncpy(varName, tokens[pos+1].value, 50);
    bool isEq = false;
    bool isEqAdd = false;
    bool isEqSub = false;
    bool isOper = false;
    if(tokLen > pos+2 && tokens[pos+2].type == KW_OPERATOR){
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
            err_start(SYNTAX_ERROR);
            printf("Unexpected operator %s ", tokens[pos+2].value);
            err_end(tokens[pos+2].line);
            return false;
        }
    }
    // if(withKeyword){
    //     if(!isOper && tokLen > pos+2){
    //         //----------------warning-------------------
    //         if(strcmp(tokens[pos+2].type, "keyword") != 0 || strcmp(tokens[pos+2].type, "keyword_custom") != 0){
    //             printf("Unexpected end of line, expected operator\n");
    //             return false;
    //         }
    //     }
    // }
    bool exists= false;
    for (int i = 0; i < varLen; i++) {
        if (strcmp(vars[i].name, varName) == 0) {
            exists = true;
            break;
        }
    }
    if(exists == true){
        err_start(SYNTAX_ERROR);
        printf("Variable %s already exists ", varName);
        err_end(tokens[pos+1].line);
        //free(vval);
        return false;
    }
    
    if(varLen > 0){
        vars = realloc(vars, sizeof(var) * (varLen + 1));
        
    }
    else{
        vars = malloc(sizeof(var));
    }
    varLen++;
    
    strncpy(vars[varLen-1].name, varName, 51);
    valu val;
    if(subtype != NULL){
        calculateSubtype(subtype, &vars[varLen-1], pos, &pos, true, isOper);
    }
    else{
        if(isOper){
            bool vv = getValue(pos+3, &pos, &val, false);
            strncpy(vars[varLen-1].value, val.value, 101);
            vars[varLen-1].type = val.type;
            vars[varLen-1].val = val.val;
        }
        else{
            strncpy(vars[varLen-1].value, "", 2);
            vars[varLen-1].type = KW_UNDEF;
            vars[varLen-1].val = 0;
        }
    }
    if(pretype != NULL){
        strncpy(vars[varLen-1].pretype, pretype, 5);
    }
    // if(withKeyword == true){
    //     pos++;
    // }
    if(isF == true){
        vars[varLen-1].isFunc = true;
        pos++;
        int brAc = 1;
        while (pos < tokLen)
        {
            if(tokens[pos].type == KW_BRACKET_ROUNDED_CLOSED){
                brAc--;
                pos++;
            }
            if(tokens[pos].type == KW_KEY && strcmp(tokens[pos].value, "var") == 0){
                bool b = createVar(pos, &pos, true, NULL, openedBracketsShaped-2, NULL, false);
                if(!b){
                    return false;
                }
            }
        }
        
    }
    *posi = pos;
    return true;
}
void tokenize(char* code){
    const int length = strlen(code);
    int pos = 0;
    //i8 - int 8
    //i16 - int 16
    //i32 - int 32
    //i64 - int 64
    //u8 - unsigned int 8
    //u16 - unsigned int 16
    //u32 - unsigned int 32
    //u64 - unsigned int 64
    //sc - signed char
    //uc - unsigned char
    //mut - mutating type
    //si - signed int
    //ui - unsigned int
    //llong - long long
    //float - float
    //double - double
    //long - long
    //ldob - long double
    //int - int
    //var - var
    //str - string
    //char - char
    char BUILT_IN_KEYWORDS[20][10] = {{"var"}, {"int"}, {"si"}, {"ui"}, {"long"}, {"llong"}, {"i8"}, {"i16"}, {"i32"}, {"u8"}, {"u16"}, {"u32"}, {"mut"}, {"double"}, {"float"}, {"ldob"}, {"str"}, {"char"}, {"sc"}, {"uc"}};
    char BUILT_IN_FUNCS[2][20] = {{"print"}, {"println"}};
    char BUILT_IN_STATEMENTS[6][20] = {{"if"}, {"elif"}, {"else"}, {"while"}, {"break"}, {"continue"}};
    char varChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    int line = 1;
    int column = 0;
    while(pos<length){
        char currentChar = code[pos];
        if(currentChar == ' '){
            pos++;
            column++;
            continue;
        }
        else if(currentChar == ';'){
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
            while(code[pos] != '\"' && pos < length){
                if(code[pos] == '\\'){
                    pos++;
                    column++;
                    char ch = ' ';
                    if(code[pos] == 'n'){
                        ch = '\n';
                    }
                    else if(code[pos] == 't'){
                        ch = '\t';
                    }
                    else if(code[pos] == 'a'){
                        ch = '\a';
                    }
                    else if(code[pos] == 'b'){
                        ch = '\b';
                    }
                    else if(code[pos] == 'f'){
                        ch = '\f';
                    }
                    else if(code[pos] == 'r'){
                        ch = '\r';
                    }
                    else if(code[pos] == 'v'){
                        ch = '\v';
                    }
                    else if(code[pos] == '\\'){
                        ch = '\\';
                    }
                    else if(code[pos] == '\''){
                        ch = '\'';
                    }
                    else if(code[pos] == '\"'){
                        ch = '\"';
                    }
                    else if(code[pos] == '?'){
                        ch = '\?';
                    }
                    else{
                        err_start(SYNTAX_ERROR);
                        printf("Unexpected escape sequence in string ");
                        err_end(line);
                    }
                    strncat(res, &ch, 1);
                }
                else{
                    strncat(res, &code[pos], 1);
                }
                pos++;
                column++;
            }
            if(code[pos] != '\"'){
                err_start(SYNTAX_ERROR);
                printf("Unterminated string at line %d column %d ", line, column);
                pos++;
                err_end(line);
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
            tokens[tokLen-1].type = KW_STRING;
            strncpy(tokens[tokLen-1].value, res, 101);
            tokens[tokLen-1].line = line;
            
        }
        else if(currentChar == '\''){
            char res;
            pos++;
            column++;
            int leng = 0;
            while(code[pos] != '\'' && pos < length){
                leng ++;
                res = code[pos];
                pos++;
                column++;
            }
            
            if(code[pos] != '\''){
                err_start(SYNTAX_ERROR);
                printf("Unterminated char at line %d column %d ", line, column);
                pos++;
                err_end(line);
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
            tokens[tokLen-1].type = KW_CHAR;
            strncpy(tokens[tokLen-1].value, &res, 1);
            tokens[tokLen-1].line = line;
        }
        else if(currentChar == '+'){
            pos++;
            if(code[pos] == '+'){
                tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                tokLen++;
                tokens[tokLen-1].type = KW_OPERATOR;
                strncpy(tokens[tokLen-1].value, "++", 5);
                tokens[tokLen-1].line = line;
                pos++;
            }
            else if(code[pos] == '='){
                tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                tokLen++;
                tokens[tokLen-1].type = KW_OPERATOR;
                strncpy(tokens[tokLen-1].value, "+=", 5);
                tokens[tokLen-1].line = line;
                pos++;
            }
            else{
                tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                tokLen++;
                tokens[tokLen-1].type = KW_OPERATOR;
                strncpy(tokens[tokLen-1].value, "add", 5);
                tokens[tokLen-1].line = line;
            }
        }
        else if(currentChar == '-'){
            
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            tokens[tokLen-1].type = KW_OPERATOR;
            strncpy(tokens[tokLen-1].value, "sub", 5);
            tokens[tokLen-1].line = line;
            pos++;
        }
        else if(currentChar == '('){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            tokens[tokLen-1].type = KW_BRACKET_ROUNDED_OPENED;
            tokens[tokLen-1].line = line;
            pos++;
        }
        else if(currentChar == ')'){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            tokens[tokLen-1].type = KW_BRACKET_ROUNDED_CLOSED;
            tokens[tokLen-1].line = line;
            pos++;
        }
        else if(currentChar == '['){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            tokens[tokLen-1].type = KW_BRACKET_SQUARED_OPENED;
            tokens[tokLen-1].line = line;
            pos++;
        }
        else if(currentChar == ']'){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            tokens[tokLen-1].type = KW_BRACKET_SQUARED_CLOSED;
            tokens[tokLen-1].line = line;
            pos++;
        }
        else if(currentChar == '{'){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            tokens[tokLen-1].type = KW_BRACKET_SHAPED_OPENED;
            tokens[tokLen-1].line = line;
            pos++;
        }
        else if(currentChar == '}'){
            tokens = realloc(tokens, (tokLen+1)*sizeof(token));
            tokLen++;
            tokens[tokLen-1].type = KW_BRACKET_SHAPED_CLOSED;
            tokens[tokLen-1].line = line;
            pos++;
        }
        else if(isdigit(currentChar)){
            char res[101] = "";
            bool isNum = true;
            bool sobaka = false;
            int digitsAfterDot = -1;
            int zerosCount = 0;
            while(isNum && pos < length){
                if(!isdigit(code[pos]) || code[pos] == ' ' || code[pos] == 'n' || code[pos] == '\r'){
                    if(code[pos] != '.'){
                        //if(code[pos] != 'd' || code[pos] != 'l'){
                        if(code[pos] != '@'){
                            isNum = false;
                            break;
                        }
                        // else if(code[pos] == 'd'){
                        //    isDouble = 1;
                        // }
                        // else if(code[pos] == 'l'){
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
                        err_start(SYNTAX_ERROR);
                        printf("Undefined second dot at line %d column %d ", line, column);
                        pos++;
                        err_end(line);
                        continue;

                    }
                }
                else if(code[pos] == '0' && digitsAfterDot > 0){
                    zerosCount++;
                }
                else{
                    zerosCount = 0;
                }
                
                strncat(res, &code[pos], 1);
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
                    tokens[tokLen-1].type = KW_FLOAT;
                    tokens[tokLen-1].val = strtold(res, NULL);
                }
                else{
                    tokens[tokLen-1].type = KW_FLOAT;
                    tokens[tokLen-1].val = strtold(res, NULL);
                }
                
            }
            else{
                tokens[tokLen-1].type = KW_INT;
                tokens[tokLen-1].val = strtold(res, NULL);
                // if(pos < length){
                //     //printf("Hello\n");
                // }
            }
            tokens[tokLen-1].line = line;
        }
        else{
            bool isInclude = false;
            for (int i = 0; i < ArrLeng(varChars); i++) {
                if (varChars[i] == currentChar || isdigit(currentChar)) {
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
                        if (varChars[i] == code[pos] || isdigit(code[pos])) {
                            isInclude = true;
                            strncat(res, &code[pos], 1);
                            pos++;
                            column++;
                            break;
                        }
                        else if(code[pos] == '('){
                            isFunc = true;
                            pos++;
                            column++;
                            goto gt1;
                        }
                        else if(code[pos] == '['){
                            brackets = 1;
                            pos++;
                            column++;
                            goto gt1;
                        }
                        else if(code[pos] == '{'){
                            brackets = 2;
                            pos++;
                            column++;
                            goto gt1;
                        }
                        // else if(code[pos] == ')'){
                        //     brackets = 3;
                        //     pos++;
                        //     column++;
                        //     goto gt1;
                        // }
                        // else if(code[pos] == ']'){
                        //     brackets = 4;
                        //     pos++;
                        //     column++;
                        //     goto gt1;
                        // }
                        // else if(code[pos] == '}'){
                        //     brackets = 5;
                        //     pos++;
                        //     column++;
                        //     goto gt1;
                        // }
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
                }
                if(!isInclude){
                    for (int i = 0; i < ArrLeng(BUILT_IN_STATEMENTS); i++) {
                        if (strcmp(BUILT_IN_STATEMENTS[i], res) == 0) {
                            isStatement = true;
                            break;
                        }
                    }
                }
                tokLen++;
                if(tokLen > 1){
                    tokens = realloc(tokens, (tokLen)*sizeof(token));
                }
                else{
                    tokens = calloc(1, sizeof(token));
                }
                
                //tokens[tokLen-1].type = calloc(1,1);
                //tokens[tokLen-1].value = calloc(1,1);
                tokens[tokLen-1].line = line;
                if(isInclude){
                    tokens[tokLen-1].type = KW_KEY;
                }
                else{
                    if(isStatement){
                        tokens[tokLen-1].type = KW_STATEMENT;
                    }
                    else if(!isFunc){
                        tokens[tokLen-1].type = KW_KEY_CUSTOM;
                    }
                    else{
                        isInclude = false;
                        for (int i = 0; i < ArrLeng(BUILT_IN_FUNCS); i++) {
                            if (strcmp(BUILT_IN_FUNCS[i], res) == 0) {
                                isInclude = true;
                                break;
                            }
                        }
                        if(isInclude){
                            tokens[tokLen-1].type = KW_FUN;
                        }
                        else{
                            tokens[tokLen-1].type = KW_FUN_CUSTOM;
                        }
                        
                    }
                }
                strncpy(tokens[tokLen-1].value, res, 101);
                if(isFunc){
                    tokLen++;
                    if(tokLen > 1){
                        tokens = realloc(tokens, (tokLen)*sizeof(token));
                    }
                    else{
                        tokens = malloc(sizeof(token));
                    }
                    tokens[tokLen-1].type = KW_BRACKET_ROUNDED_OPENED;
                }
                if(brackets == 1){
                    tokLen++;
                    if(tokLen > 1){
                        tokens = realloc(tokens, (tokLen)*sizeof(token));
                    }
                    else{
                        tokens = malloc(sizeof(token));
                    }
                    tokens[tokLen-1].type = KW_BRACKET_SQUARED_OPENED;
                }
                else if(brackets == 2){
                    tokLen++;
                    if(tokLen > 1){
                        tokens = realloc(tokens, (tokLen)*sizeof(token));
                    }
                    else{
                        tokens = malloc(sizeof(token));
                    }
                    tokens[tokLen-1].type = KW_BRACKET_SHAPED_OPENED;
                }
                // else if(brackets == 3){
                //     tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                //     tokLen++;
                    
                //     strncpy(tokens[tokLen-1].type, "bracket)", 15);
                // }
                // else if(brackets == 4){
                //     tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                //     tokLen++;
                //     strncpy(tokens[tokLen-1].type, "bracket]", 15);
                // }
                // else if(brackets == 5){
                //     tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                //     tokLen++;
                //     strncpy(tokens[tokLen-1].type, "bracket}", 15);
                // }
                isFunc = false;
            }
            else if(currentChar == '/'){
                pos++;
                column++;
                if(code[pos] == '/'){
                    pos++;
                    column++;
                    while(pos < length && code[pos] != '\n'){
                        pos++;
                        column++;
                    }
                    line++;
                    column = 0;
                    pos++;
                    continue;
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, "/", 6);
                    tokens[tokLen-1].line = line;
                }
            }
            else if(currentChar == '='){
                pos++;
                column++;
                if(code[pos] != '='){
                    if(tokLen > 0){
                        tokens = realloc(tokens, (tokLen+1)*sizeof(token));
                    }
                    else{
                        tokens = calloc(1, sizeof(token));
                    }
                    //tokens[tokLen-1].type = calloc(1,1);
                    //tokens[tokLen-1].value = calloc(1,1);
                    tokLen++;
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, "eq", 5);
                    tokens[tokLen-1].line = line;
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, "==", 6);
                    tokens[tokLen-1].line = line;
                }
            }
            else if(currentChar == '>'){
                pos++;
                column++;
                if(code[pos] == '='){
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, ">=", 6);
                    tokens[tokLen-1].line = line;
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, ">", 6);
                    tokens[tokLen-1].line = line;
                }
            }
            else if(currentChar == '<'){
                pos++;
                column++;
                if(code[pos] == '='){
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, "<=", 6);
                    tokens[tokLen-1].line = line;
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, "<", 6);
                    tokens[tokLen-1].line = line;
                }
            }
            else if(currentChar == '!'){
                pos++;
                column++;
                if(code[pos] == '='){
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, "!=", 6);
                    tokens[tokLen-1].line = line;
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, "!", 3);
                    tokens[tokLen-1].line = line;
                }
            }
            else if(currentChar == '&'){
                pos++;
                column++;
                if(code[pos] == '&'){
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, "&&", 6);
                    tokens[tokLen-1].line = line;
                }
            }
            else if(currentChar == '|'){
                pos++;
                column++;
                if(code[pos] == '|'){
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
                    tokens[tokLen-1].type = KW_OPERATOR;
                    strncpy(tokens[tokLen-1].value, "||", 6);
                    tokens[tokLen-1].line = line;
                }
            }
            else{
                err_start(SYNTAX_ERROR);
                printf("Unexpected character %c at line %d column %d ", code[pos], line, column);
                pos++;
                err_end(line);
                continue;
            }
        }
        
    }
    
}
typedef struct {
    int pos;
    int bracketsShaped;
} loopStruct;
bool parse(){
    const int len = tokLen;
    int pos = 0;
    char varT[12] = "";
    uint_fast8_t isInIf = 5;
    int bracketsToEndIf = 0;
    loopStruct loops[200];
    int loopsTo = 0;
    argument arg[MAX_ARGS];
    while(pos<len){
        if(tokens[pos].type == KW_STATEMENT && strcmp(tokens[pos].value, "else") == 0){
            pos++;
            if(lastArg == ARG_FALSE){
                if(tokens[pos+1].type == KW_BRACKET_SHAPED_OPENED){
                    lastArg = ARG_UNDEFINED;
                    openedBracketsShaped++;
                    pos++;
                    isInIf = 1;
                    continue;
                }
            }
            else if(lastArg == ARG_TRUE){
                if(tokens[pos+1].type == KW_BRACKET_SHAPED_OPENED){
                    int bracketsS = 1;
                    lastArg = ARG_UNDEFINED;
                    pos++;
                    while(pos < tokLen && bracketsS > 0){
                        if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                            bracketsS++;
                        }
                        else if(tokens[pos].type == KW_BRACKET_SHAPED_CLOSED){
                            bracketsS--;
                        }
                        pos++;
                    }
                    isInIf = 0;
                    continue;
                }
            }
            else{
                err_start(SYNTAX_ERROR);
                printf("Unexpected else statement without preceding if or elif ");
                err_end(tokens[pos-1].line);
                return false;
            }
        }
        else if(tokens[pos].type == KW_STATEMENT && strcmp(tokens[pos].value, "elif") == 0){
            if(lastArg == ARG_FALSE){
                if(tokLen <= pos+1){
                    err_start(SYNTAX_ERROR);
                    printf("Unexpected statement start, expected ( ");
                    err_end(tokens[pos].line);
                    return false;
                }
                if(tokens[pos+1].type != KW_BRACKET_ROUNDED_OPENED){
                    err_start(SYNTAX_ERROR);
                    printf("Unexpected statement start, expected ( ");
                    err_end(tokens[pos].line);
                    return false;
                }
                pos+=2;
                if(tokLen <= pos){  
                    err_start(SYNTAX_ERROR);
                    printf("Statement arguments not found ");
                    err_end(tokens[pos].line);
                    return false;
                }
                int brackets = 1;
                int argsCount = 1;
                //arg[argsCount-1].isNeeded = true;
                arg[argsCount-1].isTrue = getArgument(pos, &pos);
                while(brackets > 0 && tokLen > pos){
                    if(tokens[pos].type == KW_BRACKET_ROUNDED_OPENED){
                        brackets++;
                        pos++;
                    }
                    else if(tokens[pos].type == KW_BRACKET_ROUNDED_CLOSED){
                        brackets--;
                        pos++;
                    }
                    if(tokens[pos].type == KW_OPERATOR){
                        if(strcmp(tokens[pos].value, "&&") == 0){
                            arg[argsCount].isTrue = getArgument(pos+1, &pos);
                            arg[argsCount].isNeeded = true;
                            argsCount++;
                        }
                        else if(strcmp(tokens[pos].value, "||") == 0){
                            arg[argsCount].isTrue = getArgument(pos+1, &pos);
                            arg[argsCount].isNeeded = false;
                            argsCount++;
                        }
                    }
                }
                bool result = false;
                if(argsCount > 1){
                    for(int i = 1; i < argsCount; i++){
                        if(arg[i].isNeeded == true){
                            if(arg[i].isTrue == true){
                                if(arg[i-1].isTrue == true){
                                    result = true;
                                    i++;
                                }
                                else{
                                    result = false;
                                    break;
                                }
                            }
                            else{
                                result = false;
                                break;
                            }
                        }
                        else{
                            if(arg[i].isTrue == true){
                                result = true;
                                i++;
                            }
                            else{
                                if(arg[i-1].isTrue == true){
                                    result = true;
                                    i++;
                                }
                                else{
                                    result = false;
                                    break;
                                }
                            }
                        }
                    }
                }
                else if(argsCount == 1){
                    if(arg[0].isTrue == true){
                        result = true;
                    }
                }
                else{
                    result = true;
                }
                if(result == true){
                    if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                        lastArg = ARG_TRUE;
                        isInIf=1;
                        bracketsToEndIf = openedBracketsShaped;
                        openedBracketsShaped++;
                        pos++;
                        continue;
                    }
                }
                else{
                    if(tokens[pos].type == KW_BRACKET_SHAPED_CLOSED){
                        int bracketsS = 1;
                        lastArg = ARG_FALSE;
                        pos++;
                        while(pos < tokLen && bracketsS > 0){
                            if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                                bracketsS++;
                            }
                            else if(tokens[pos].type == KW_BRACKET_SHAPED_CLOSED){
                                bracketsS--;
                            }
                            pos++;
                        }
                    }
                    isInIf = 0;
                }
                continue;
            }
            else if(lastArg == ARG_TRUE){
                if(tokLen <= pos+1){
                    err_start(SYNTAX_ERROR);
                    printf("Unexpected statement start, expected ( ");
                    err_end(tokens[pos].line);
                    return false;
                }
                if(tokens[pos+1].type != KW_BRACKET_ROUNDED_OPENED){
                    err_start(SYNTAX_ERROR);
                    printf("Unexpected statement start, expected ( ");
                    err_end(tokens[pos].line);
                    return false;
                }
                pos+=2;
                if(tokLen <= pos){  
                    err_start(SYNTAX_ERROR);
                    printf("Statement arguments not found ");
                    err_end(tokens[pos].line);
                    return false;
                }
                int brackets = 1;
                int argsCount = 1;
                while(brackets > 0 && tokLen > pos){
                    if(tokens[pos].type == KW_BRACKET_ROUNDED_OPENED){
                        brackets++;
                    }
                    else if(tokens[pos].type == KW_BRACKET_ROUNDED_CLOSED){
                        brackets--;
                    }
                    pos++;
                }
                if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                    int bracketsS = 1;
                    lastArg = ARG_TRUE;
                    pos++;
                    while(pos < tokLen && bracketsS > 0){
                        if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                            bracketsS++;
                        }
                        else if(tokens[pos].type == KW_BRACKET_SHAPED_CLOSED){
                            bracketsS--;
                        }
                        pos++;
                    }
                    isInIf = 0;
                    continue;
                }
            }
            else{
                err_start(SYNTAX_ERROR);
                printf("Unexpected elif statement without preceding if ");
                err_end(tokens[pos].line);
                return false;
            }
        }
        //---------------CLEAR PREVIOUS IF------------
        if(isInIf == 0){
            lastArg = ARG_UNDEFINED;
            isInIf = 5;
        }
        //-------------------print--------------------
        if(tokens[pos].type == KW_FUN && strncmp(tokens[pos].value, "print", 6) == 0 || strncmp(tokens[pos].value, "println", 6) == 0){
            if(build == false){
                int p = pos;
                if(tokLen <= pos+1){
                    err_start(SYNTAX_ERROR);
                    printf("Unexpected function invoke, expected ( ");
                    err_end(tokens[p].line);
                    return false;
                }
                else if(tokLen <= pos+2){
                    err_start(SYNTAX_ERROR);
                    printf("Unexpected end of line, expected value for %s ", tokens[p].value);
                    err_end(tokens[p].line);
                    return false;
                }
                
                valu val;

                if(getValue(pos+2, &pos, &val, false) == false){
                    err_start(RUNTIME_ERROR);
                    printf("Error getting value for print ");
                    err_end(tokens[p+2].line);
                    return false;
                }
                if(tokens[pos].type != KW_BRACKET_ROUNDED_CLOSED){
                    err_start(SYNTAX_ERROR);
                    printf("Unexpected end of function print, expected ) ");
                    err_end(tokens[pos].line);
                    return false;
                }
                if(val.type == KW_STRING || val.type == KW_CHAR){
                    printf("%s", val.value);
                }
                else if(val.type == KW_INT || val.type == KW_FLOAT){
                    printf("%Lg", val.val);
                }
                else if(val.type == KW_UNDEF){
                    printf("");
                }
                if(strcmp(tokens[p].value, "println") == 0){
                    printf("\n");
                }
                pos++;
            }
            else{
                if(forceBuild == false){
                    int p = pos;
                    if(tokLen <= pos+1){
                        err_start(SYNTAX_ERROR);
                        printf("Unexpected function invoke, expected ( ");
                        err_end(tokens[p].line);
                        return false;
                    }
                    else if(tokLen <= pos+2){
                        err_start(SYNTAX_ERROR);
                        printf("Unexpected end of line, expected value for %s ", tokens[p].value);
                        err_end(tokens[p].line);
                        return false;
                    }
                    
                    valu val;
                    if(getValue(pos+2, &pos, &val, false) == false){
                        err_start(RUNTIME_ERROR);
                        printf("Error getting value for print ");
                        err_end(tokens[pos].line);
                        return false;
                    }
                    pos++;
                }
                else{
                    int p = pos;
                    
                    valu val;
                    getValue(pos+2, &pos, &val, false);
                    pos++;
                }
            }   
        }
        else if(tokens[pos].type == KW_FUN_CUSTOM){
            int p = pos;
            if(tokLen <= pos+1){
                err_start(SYNTAX_ERROR);
                printf("Unexpected function invoke, expected ( ");
                err_end(tokens[pos].line);
                return false;
            }
            int vvp = -1;
            for (int i = 0; i < varLen; i++)
            {
                if(strcmp(vars[i].name, tokens[pos].value) == 0){
                    if(vars[i].isFunc == false){
                        err_start(SYNTAX_ERROR);
                        printf("%s is not a function ", vars[i].name);
                        err_end(tokens[pos].line);
                        return false;
                    }
                    vvp = i;
                }
            }
            if(vvp == -1){
                pos--;
                if(strcmp(varT, "") == 0){
                    bool b = createVar(pos, &pos, false, NULL, openedBracketsShaped-1, NULL, true);
                    if(!b){
                        return false;
                    }
                }
                else{
                    bool b = createVar(pos, &pos, false, NULL, openedBracketsShaped-1, varT, true);
                    strcpy(varT, "");
                    if(!b){
                        return false;
                    }
                }
                // printf("Can't find function or variable with name %s\n", tokens[pos].value);
                // return false;
            }
            
            valu val;

            if(getValue(pos+2, &pos, &val, false) == false){
                err_start(RUNTIME_ERROR);
                printf("Error getting value for %s ", tokens[p].value);
                err_end(tokens[pos].line);
                return false;
            }
        }
        //----------------------if statement--------------------------
        else if(tokens[pos].type == KW_STATEMENT && strcmp(tokens[pos].value, "if") == 0){
            lastArg = ARG_UNDEFINED;
            if(tokLen <= pos+1){
                err_start(SYNTAX_ERROR);
                printf("Unexpected statement start, expected ( ");
                err_end(tokens[pos].line);
                return false;
            }
            if(tokens[pos+1].type != KW_BRACKET_ROUNDED_OPENED){
                err_start(SYNTAX_ERROR);
                printf("Unexpected statement start, expected ( ");
                err_end(tokens[pos].line);
                return false;
            }
            pos+=2;
            if(tokLen <= pos){  
                err_start(SYNTAX_ERROR);
                printf("Statement arguments not found ");
                err_end(tokens[pos].line);
                return false;
            }
            int brackets = 1;
            int argsCount = 1;
            //arg[argsCount-1].isNeeded = true;
            arg[argsCount-1].isTrue = getArgument(pos, &pos);
            while(brackets > 0 && tokLen > pos){
                if(tokens[pos].type == KW_BRACKET_ROUNDED_OPENED){
                    brackets++;
                    pos++;
                }
                else if(tokens[pos].type == KW_BRACKET_ROUNDED_CLOSED){
                    brackets--;
                    pos++;
                }
                if(tokens[pos].type == KW_OPERATOR){
                    if(strcmp(tokens[pos].value, "&&") == 0){
                        arg[argsCount].isTrue = getArgument(pos+1, &pos);
                        arg[argsCount].isNeeded = true;
                        argsCount++;
                    }
                    else if(strcmp(tokens[pos].value, "||") == 0){
                        arg[argsCount].isTrue = getArgument(pos+1, &pos);
                        arg[argsCount].isNeeded = false;
                        argsCount++;
                    }
                }
            }
            bool result = false;
            if(argsCount > 1){
                for(int i = 1; i < argsCount; i++){
                    if(arg[i].isNeeded == true){
                        if(arg[i].isTrue == true){
                            if(arg[i-1].isTrue == true){
                                result = true;
                                i++;
                            }
                            else{
                                result = false;
                                break;
                            }
                        }
                        else{
                            result = false;
                            break;
                        }
                    }
                    else{
                        if(arg[i].isTrue == true){
                            result = true;
                            i++;
                        }
                        else{
                            if(arg[i-1].isTrue == true){
                                result = true;
                                i++;
                            }
                            else{
                                result = false;
                                break;
                            }
                        }
                    }
                }
            }
            else if(argsCount == 1){
                if(arg[0].isTrue == true){
                    result = true;
                }
            }
            else{
                result = true;
            }
            if(result == true){
                if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                    lastArg = ARG_TRUE;
                    isInIf=1;
                    bracketsToEndIf = openedBracketsShaped;
                    openedBracketsShaped++;
                    pos++;
                    continue;
                }
                
            }
            else{
                if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                    int bracketsS = 1;
                    lastArg = ARG_FALSE;
                    pos++;
                    while(pos < tokLen && bracketsS > 0){
                        if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                            bracketsS++;
                        }
                        else if(tokens[pos].type == KW_BRACKET_SHAPED_CLOSED){
                            bracketsS--;
                        }
                        pos++;
                    }
                    continue;
                }
            }
            
        }
        else if(tokens[pos].type == KW_BRACKET_SHAPED_CLOSED){
            if(openedBracketsShaped > 0){
                openedBracketsShaped--;
                if(isInIf == 1 && bracketsToEndIf == openedBracketsShaped){
                    isInIf = 0;
                }
                if(loops[loopsTo-1].bracketsShaped == openedBracketsShaped){
                    loopsTo--;
                    pos = loops[loopsTo].pos;
                    continue;
                }
            }
            else{
                err_start(SYNTAX_ERROR);
                printf("Unexpected bracket } ");
                err_end(tokens[pos].line);
            }
            pos++;
        }
        else if(tokens[pos].type == KW_BRACKET_ROUNDED_CLOSED){
            if(openedBracketsRound > 0){
                openedBracketsRound--;
            }
            else{
                err_start(SYNTAX_ERROR);
                printf("Unexpected bracket ) ");
                err_end(tokens[pos].line);
            }
            pos++;
        }
        else if(tokens[pos].type == KW_BRACKET_SQUARED_CLOSED){
            if(openedBracketsSquare > 0){
                openedBracketsSquare--;
            }
            else{
                err_start(SYNTAX_ERROR);
                printf("Unexpected bracket ] ");
                err_end(tokens[pos].line);
            }
            pos++;
        }
        else if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
            openedBracketsShaped++;
            pos++;
        }
        else if(tokens[pos].type == KW_BRACKET_ROUNDED_OPENED){
            openedBracketsRound++;
            pos++;
        }
        else if(tokens[pos].type == KW_BRACKET_SQUARED_OPENED){
            openedBracketsSquare++;
            pos++;
        }
        //-------------------var--------------------
        else if(tokens[pos].type == KW_KEY && strcmp(tokens[pos].value, "var") == 0){
            bool b = createVar(pos, &pos, true, NULL, openedBracketsShaped-1, NULL, false);
            if(!b){
                return false;
            }
        }
        //---------------while loop--------------------------
        else if(tokens[pos].type == KW_STATEMENT && strcmp(tokens[pos].value, "while") == 0){
            int p = pos;
            if(tokLen <= pos+1){
                err_start(SYNTAX_ERROR);
                printf("Unexpected loop start, expected ( ");
                err_end(tokens[pos].line);
                return false;
            }
            if(tokens[pos+1].type != KW_BRACKET_ROUNDED_OPENED){
                err_start(SYNTAX_ERROR);
                printf("Unexpected loop start, expected ( ");
                err_end(tokens[pos].line);
                return false;
            }
            pos+=2;
            if(tokLen <= pos){  
                err_start(SYNTAX_ERROR);
                printf("Loop arguments not found ");
                err_end(tokens[pos].line);
                return false;
            }
            int brackets = 1;
            int argsCount = 1;
            //arg[argsCount-1].isNeeded = true;
            arg[argsCount-1].isTrue = getArgument(pos, &pos);
            while(brackets > 0 && tokLen > pos){
                if(tokens[pos].type == KW_BRACKET_ROUNDED_OPENED){
                    brackets++;
                    pos++;
                }
                else if(tokens[pos].type == KW_BRACKET_ROUNDED_CLOSED){
                    brackets--;
                    pos++;
                }
                if(tokens[pos].type == KW_OPERATOR){
                    if(strcmp(tokens[pos].value, "&&") == 0){
                        arg[argsCount].isTrue = getArgument(pos+1, &pos);
                        arg[argsCount].isNeeded = true;
                        argsCount++;
                    }
                    else if(strcmp(tokens[pos].value, "||") == 0){
                        arg[argsCount].isTrue = getArgument(pos+1, &pos);
                        arg[argsCount].isNeeded = false;
                        argsCount++;
                    }
                }
            }
            bool result = false;
            if(argsCount > 1){
                for(int i = 1; i < argsCount; i++){
                    if(arg[i].isNeeded == true){
                        if(arg[i].isTrue == true){
                            if(arg[i-1].isTrue == true){
                                result = true;
                                i++;
                            }
                            else{
                                result = false;
                                break;
                            }
                        }
                        else{
                            result = false;
                            break;
                        }
                    }
                    else{
                        if(arg[i].isTrue == true){
                            result = true;
                            i++;
                        }
                        else{
                            if(arg[i-1].isTrue == true){
                                result = true;
                                i++;
                            }
                            else{
                                result = false;
                                break;
                            }
                        }
                    }
                }
            }
            else if(argsCount == 1){
                if(arg[0].isTrue == true){
                    result = true;
                }
            }
            else{
                result = true;
            }
            if(result == true){
                if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                    loops[loopsTo].pos = p;
                    loops[loopsTo].bracketsShaped = openedBracketsShaped;
                    loopsTo++;
                    openedBracketsShaped++;
                    pos++;
                    continue;
                }
                
            }
            else{
                if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                    int bracketsS = 1;
                    pos++;
                    while(pos < tokLen && bracketsS > 0){
                        if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                            bracketsS++;
                            pos++;
                        }
                        else if(tokens[pos].type == KW_BRACKET_SHAPED_CLOSED){
                            bracketsS--;
                            pos++;
                        }
                        else{
                            pos++;
                        }
                    }
                    continue;
                }
            }
        }
        //-----------break---------
        else if(tokens[pos].type == KW_STATEMENT && strcmp(tokens[pos].value, "break") == 0){
            int tB = loops[loopsTo-1].bracketsShaped;
            pos++;
            while (pos < tokLen && openedBracketsShaped > tB)
            {
                if(tokens[pos].type == KW_BRACKET_SHAPED_OPENED){
                    openedBracketsShaped++;
                }
                else if(tokens[pos].type == KW_BRACKET_SHAPED_CLOSED){
                    openedBracketsShaped--;
                }
                pos++;
            }
            loopsTo--;
        }
        //--------------continue-------------
        else if(tokens[pos].type == KW_STATEMENT && strcmp(tokens[pos].value, "continue") == 0){
            loopsTo--;
            pos = loops[loopsTo].pos;
            continue;
        }
        //-------------------all keywords-------------------
        else if(tokens[pos].type == KW_KEY){
            if(tokens[pos+1].type == KW_KEY_CUSTOM){
                if(strcmp(varT, "") == 0){
                    bool b = createVar(pos, &pos, true, tokens[pos].value, openedBracketsShaped-1, NULL, false);
                    if(!b){
                        return false;
                    }
                }
                else{
                    bool b = createVar(pos, &pos, true, tokens[pos].value, openedBracketsShaped-1, varT, false);
                    strcpy(varT, "");
                    if(!b){
                        return false;
                    }
                }
            }
            else if(tokens[pos+1].type == KW_FUN_CUSTOM){
                if(strcmp(varT, "") == 0){
                    bool b = createVar(pos, &pos, true, tokens[pos].value, openedBracketsShaped-1, NULL, true);
                    if(!b){
                        return false;
                    }
                }
                else{
                    bool b = createVar(pos, &pos, true, tokens[pos].value, openedBracketsShaped-1, varT, true);
                    strcpy(varT, "");
                    if(!b){
                        return false;
                    }
                }
            }
        }
        //--------var without var word---------------
        else if(tokens[pos].type == KW_KEY_CUSTOM){
            printf(" \b"); // ERROR WITHOUT THIS(IDK WHY)
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
                    if(tokens[pos+1].type == KW_OPERATOR){
                        if(strcmp(tokens[pos+1].value, "eq") == 0){
                            if(strcmp(vars[index].subtype, "") == 0 || vars[index].subtype == NULL || strcmp(vars[index].subtype, "mut") == 0 || strcmp(vars[index].subtype, "ft\\eq") == 0){
                                if(strcmp(vars[index].subtype, "mut") == 0){
                                    valu val;
                                    if(getValue(pos+2, &pos, &val, false) != true){
                                        err_start(RUNTIME_ERROR);
                                        printf("Error getting value\n");
                                        err_end(tokens[pos].line);
                                    }
                                    vars[index].type = val.type;
                                    vars[index].val = val.val;
                                    strcpy(vars[index].value, val.value);
                                }
                                else{
                                    valu val;
                                    if(getValue(pos+2, &pos, &val, false) != true){
                                        err_start(RUNTIME_ERROR);
                                        printf("Error getting value\n");
                                        err_end(tokens[pos].line);
                                    }
                                    if(vars[index].type == KW_INT || vars[index].type == KW_FLOAT){
                                        if(val.type == KW_STRING || val.type == KW_CHAR){
                                            vars[index].val = strtold(val.value, NULL);
                                        }
                                        else{
                                            vars[index].val = val.val;
                                        }
                                    }
                                    else if(vars[index].type == KW_STRING){
                                        if(val.type == KW_INT || val.type == KW_FLOAT){
                                            sprintf(vars[index].value, "%Lg", val.val);
                                        }
                                        else{
                                            strcpy(vars[index].value, val.value);
                                        }
                                    }
                                    else if(vars[index].type == KW_CHAR){
                                        if(val.type == KW_INT || val.type == KW_FLOAT){
                                            sprintf(vars[index].value, "%Lg", val.val);
                                            strncpy(vars[index].value, vars[index].value, 1);
                                            vars[index].value[1] = '\0';
                                        }
                                        else{
                                            strncpy(vars[index].value, val.value, 1);
                                            vars[index].value[1] = '\0';
                                        }
                                        
                                    }
                                }
                            }
                            else{
                                calculateSubtype(vars[index].subtype, &vars[index], pos+2, &pos, false, true);
                            }
                        }
                        else if(strcmp(tokens[pos+1].value, "++") == 0){
                            if(vars[index].type == KW_INT || vars[index].type == KW_FLOAT){
                                vars[index].val++;
                            }
                            else if(vars[index].type == KW_STRING){
                                err_start(SYNTAX_ERROR);
                                printf("Unexpected operator ++ for string ");
                                err_end(tokens[pos+1].line);
                                return false;
                            }
                            else if(vars[index].type == KW_CHAR){
                                vars[index].value[0]++;
                            }
                            pos+=2;
                        }
                        else if(strcmp(tokens[pos+1].value, "+=") == 0){
                            valu val;
                            if(getValue(pos+2, &pos, &val, false) != true){
                                err_start(RUNTIME_ERROR);
                                printf("Error getting value\n");
                                err_end(tokens[pos].line);
                            }
                            if(vars[index].type == KW_INT || vars[index].type == KW_FLOAT){
                                if(val.type == KW_INT || val.type == KW_FLOAT){
                                    vars[index].val+= val.val;
                                }
                                else if(val.type == KW_STRING || val.type == KW_FLOAT){
                                    vars[index].val+= strtold(val.value, NULL);
                                }
                            }
                            else if(vars[index].type == KW_STRING){
                                if(val.type == KW_STRING || val.type == KW_CHAR){
                                    strcat(vars[index].value, val.value);
                                }
                                else{
                                    char temp[MAX_STR_LENGTH];
                                    sprintf(temp, "%Lg", val.val);
                                    strcat(vars[index].value, temp);
                                }
                            }
                            else if(vars[index].type == KW_CHAR){
                                if(val.type == KW_INT || val.type == KW_FLOAT){
                                    vars[index].value[0]+=(int)val.val;
                                }
                                else{
                                    if(strcmp(vars[index].subtype, "mut") == 0){
                                        vars[index].type = KW_STRING;
                                        strcat(vars[index].value, val.value);
                                    }
                                    else{
                                        err_start(SYNTAX_ERROR);
                                        printf("Cannot append string to character ");
                                        err_end(tokens[pos+1].line);
                                        return false;
                                    }
                                }
                            }
                        }
                        else{
                            err_start(SYNTAX_ERROR);
                            printf("Unexpected operator %s ", tokens[pos+1].value);
                            err_end(tokens[pos+1].line);
                            return false;
                        }
                    }
                }
            }
            else{
                pos--;
                if(strcmp(varT, "") == 0){
                    bool b = createVar(pos, &pos, false, NULL, openedBracketsShaped-1, NULL, false);
                    if(!b){
                        return false;
                    }
                }
                else{
                    bool b = createVar(pos, &pos, false, NULL, openedBracketsShaped-1, varT, false);
                    strcpy(varT, "");
                    if(!b){
                        return false;
                    }
                }
            }
        }
        else{
            if(pos < tokLen){
                err_start(SYNTAX_ERROR);
                printf("Unexpected token %s ", tokens[pos].value);
                err_end(tokens[pos].line);
                return false;
            }
            else{
                return true;
            }
        }
        
    }
    return true;
}
void run(){
    
}

int main(int argc, char* argv[]){
    char* version = "dev0.0.5.0";
    
#ifdef _WIN32
        // Windows console
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            DWORD consoleMode;
            if (GetConsoleMode(hConsole, &consoleMode)) {
                consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                if (SetConsoleMode(hConsole, consoleMode)) {
                    supportColors = true;
                } else {
                    supportColors = false;
                }
            } else {
                supportColors = false;
            }
        } else {
            supportColors = false;
        }
#else
        if (isatty(fileno(stdout))) {
            // Unix-like console
            if (system("tput colors > /dev/null") == 0) {
                supportColors = true;
            } else {
                supportColors = false;
            }
        }
        else{
            supportColors = false;
        }
#endif
    if (argc >= 2){
        for (int i = 1; i < argc; i++)
        {
            if(strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0){
                debug = true;
            }
            else if(strcmp(argv[i], "--info") == 0 || strcmp(argv[i], "-i") == 0){
                printf("\n");
                printf("                                   ./@@/  @&\n");
                printf("                                @@@@@@@&   @@@@@&                   ");printf(" :::    :::     :::     :::       :::     :::     \n");
                printf("                             &@@@@@@@@@&   .@@@@@@@/                ");printf(":+:   :+:    :+: :+:   :+:       :+:   :+: :+:   \n");
                printf("                           #@@@@@@@@@@@@    &@@@@@@@@               ");printf("+:+  +:+    +:+   +:+  +:+       +:+  +:+   +:+  \n");
                printf("                          @@@@@@@@@@@@@@    #@@@@@@@@@/             ");printf("+#++:++    +#++:++#++: +#+  +:+  +#+ +#++:++#++: \n");
                printf("                         &@@@@@@@@@@@@@#    #@@@@@@@@@@/            ");printf("+#+  +#+   +#+     +#+ +#+ +#+#+ +#+ +#+     +#+ \n");
                printf("                        /@@@@@@@@@@@@@@     &@@@@@@@@@@@            ");printf("#+#   #+#  #+#     #+#  #+#+# #+#+#  #+#     #+# \n");
                printf("                        &@@@@@@@@@@@@@/     @@@@@@@@@@@@*           ");printf("###    ### ###     ###   ###   ###   ###     ### \n");
                printf("                        @@@@@@@@@@@@@&     @@@@@@@@@@@@@(           \n");
                printf("                        &@@@@@@@@@@@@     (@@@@@@@@@@@@@*           \n");
                printf("                         @@@@@@@@@@@@     @@@@@@@@@@@@@@            \n");
                printf("                         %%@@@@@@@@@@@    /@@@@@@@@@@@@@*           ");printf("       ___  __   __     __           \n");
                printf("                          &@@@@@@@@@@    #@@@@@@@@@@@@*             ");printf("\\  / |__  |__) /__` | /  \\ |\\ | .   \n");
                printf("                           *@@@@@@@@@    #@@@@@@@@@@@               ");printf(" \\/  |___ |  \\ .__/ | \\__/ | \\| .   %s\n", version);
                printf("                             (@@@@@@@@   (@@@@@@@@@                 \n");
                printf("                                &@@@@@(   @@@@@@(                   \n");
                printf("                                    ./&#  #\n");
                printf("                                                                    \n");
                printf("                   )_) _   _   _         ( _   _   _ ( _ o  _   _   \n");
                printf("                  ( ( (_( )_) )_) (_(     ) ) (_( (_  )\\ ( ) ) (_(  \n");
                printf("                         (   (      _)                           _) \n");

            }
            else if(strcmp(argv[i], "--build") == 0 || strcmp(argv[i], "-b") == 0){
                build = true;
                lang = C_LANG;
            }
            else if(strcmp(argv[i], "--here") == 0 || strcmp(argv[i], "-h") == 0){
                here = true;
            }
            // else if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "-version") == 0){
            //     here = true;
            // }
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
                        printf("File %s length: %d.\n", argv[i], length);
                    }
                    fseek (f, 0, SEEK_SET);
                    buffer = malloc (length+1);
                    if (buffer)
                    {
                        fread (buffer, 1, length, f);
                    }
                    buffer[length] = '\0';
                    if(debug){
                        printf("File %s has been readed.\n", argv[i]);
                    }
                    fclose (f);
                }

                if (buffer)
                {
                    tokenize(buffer);
                    parse();
                    // codes = malloc(length+1);
                    // strncpy(codes, buffer, length+1);
                    // if(debug){
                    //     printf("File %s has been added to codes.\n", argv[i]);
                    // }
                    // //printf("%c", codes[13]);
                    // free(buffer);
                    // if(debug){
                    //     printf("Freed buffer.\n");
                    // }
                }
            }
        
        }
        run();
        
        //printf("Welcome to the Puffin language v. 0.0.2!\nCreated by Dizabanik\n");
        
        
    }
    else{
        printf("To exit type exit() or press CTRL + C\n");
        char comm[MAX_COMMAND_LENGTH] = "";
        printf(">>> ");
        scanf("%s",comm);
        while (strcmp(comm, "exit()") != 0)
        {
            tokenize(comm);
            parse();
            printf("\n>>> ");
            scanf("%s",comm);
        }
        
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
    if(vars != NULL){
        free(vars);
        if(debug){
            printf("Freed all vars.\n");
        }
    }
    if(tokens != NULL){
        free(tokens);
        if(debug){
            printf("Freed all tokens.\n");
        }
    }
    if(codes != NULL){
        free(codes);
        if(debug){
            printf("Freed all codes.\n");
        }
    }

    return 0;
}
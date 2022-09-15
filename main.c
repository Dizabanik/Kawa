
#include <stdbool.h>

#define C_LANG 0
#define JAVA_SCRIPT 1
#define DEFAULT 0
#define FUNC_START 1
#define FUNC_CYCLE 2
#define VAR_START 3
#define VAR_START_IN_FUNC 3
int count1, count2;

#define ArrLeng(x)  (sizeof(x) / sizeof((x)[0]))
#define foreach(item, array) \
    for(int keep = 1, \
            count = 0,\
            size = sizeof (array) / sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
      for(item = (array) + count; keep; keep = !keep)

#pragma region str

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


typedef  char * tXt ;
void logError(tXt log, int line, tXt filename, tXt lineS){
    printf("\033[0;31m");
    printf("%s:%d: error: ", filename, line);
    printf("\033[0m");
    printf("%s\n", log);
    if(lineS != NULL){
        printf("%d | %s\n", line, lineS);
    }
    
}
//choose the following numbers wisely depending on available memory
#ifndef txtPOOLSIZE
#define txtPOOLSIZE 32768 // available memory for strings
#define txtMAXLEN 256   // max string length
#endif
#define txtNOTFOUND 9999


//show 3 ints with name and value
//#define print3int(x,y,z) printf(#x "=%d "#y "=%d "#z "=%d\n",x,y,z) ;


#define ZeroMemory(x,y)  memset (x, 0, y)
char nullchar = 0 ;
#define txtEmpty &nullchar

// limit an integer
int limii(int x,int mn,int mx) {
  if (x > mx)
    x = mx ;
  if (x < mn)
    x = mn ;
  return x ; }

// smallest of two integers 
int miniii(int x,  int y){
  if (x < y)
    return x;
  else
    return y; }

// all string are allocated in this memory pool
int txtpoolidx = 0 ;

#ifdef txtHEAPPOOL
// all strings are allocated on the heap. strings are not overwritten.


char txtfirstpool[txtMAXLEN] ;
char * txtpool = txtfirstpool ;
int txtPoolLim = 0 ;
// allocated  moor pool on the heap
void txtNewPool(void) {
  char * prevpool ;
  prevpool = txtpool ;
  txtpool = (char *) malloc(txtPOOLSIZE) ;
  memcpy(txtpool,&prevpool,sizeof(prevpool)) ;
  int oldidx = txtpoolidx ;
  txtpoolidx = sizeof(prevpool) ;
  txtPoolLim = txtPOOLSIZE-txtMAXLEN-1 ;
 }


// update poolidx and check for new pool needed  
void txtFixpool(void) {
  txtpool[txtpoolidx++] = 0 ;
  if (txtpoolidx > txtPoolLim) 
    txtNewPool() ; 
}


typedef struct txtPoolPoint {
  int idx ;
  char * buf ; } txtPoolPoint ;

//mark a position in the textpools
void txtMarkPoolPoint(txtPoolPoint * p) {
  p-> idx = txtpoolidx ;
  p->buf = txtpool ; }
  
  
//rewind to position in textpool (partial garbage collection)
void txtRewind(txtPoolPoint * p) {
  while (txtpool != p->buf) {
    char * nextpool ;
    memcpy(&nextpool,txtpool,sizeof(nextpool)) ;
    free(txtpool) ;
    txtpool = nextpool ;} 
  txtpoolidx = p->idx ;
  if (txtpool == txtfirstpool)
    txtPoolLim = 0 ;
  else
    txtPoolLim = txtPOOLSIZE-txtMAXLEN-1 ;
}
  
//flush all allocated pools (full garbage collection) 
void txtFlushPool(void) {
  txtPoolPoint pp = {0,txtfirstpool} ;
  txtRewind(&pp) ;
 }




#else
// strings are stored in global data space

char txtpool[txtPOOLSIZE] ;

//fix the poolidx
void txtFixpool(void) {
  txtpool[txtpoolidx++] = 0 ;
  if (txtpoolidx > txtPOOLSIZE-txtMAXLEN-1) {
    txtpoolidx = 0 ; }
}

void txtFlushPool(void) {
  txtpoolidx = 0 ; }

#endif
//substring
tXt txtSub(tXt tx,int bpos,int len) {
  tXt rslt = &txtpool[txtpoolidx] ;
  int ln = strlen(tx) ;
  int n ;
  if (bpos < 0)
    bpos = limii(ln+bpos-1,0,ln-1) ;
  n = limii(strlen(tx) -bpos,0,len) ;
  if (n > 0)
    memcpy(&txtpool[txtpoolidx],&tx[bpos],n) ;
  txtpoolidx += n ;
  txtFixpool() ;
  return rslt ; }

tXt txtDelete(tXt tx,int bpos,int len) {
  tXt rslt = &txtpool[txtpoolidx] ;
  int ln = strlen(tx) ;
  int n ;
  if (bpos < 0)
    bpos = limii(ln+bpos-1,0,ln-1) ;
  if (bpos > 0) {
    memcpy(rslt,tx,bpos) ;
    txtpoolidx+= bpos ; }
  if (bpos+len < ln) {
    memcpy(&rslt[bpos],&tx[bpos+len],ln-len-bpos) ;
    txtpoolidx += ln-bpos-len ; }
  txtFixpool() ;
  return rslt ; }
  


// search position in string
int txtPos(tXt src, tXt zk) {
  char * p = strstr(src,zk) ;
  if (p == NULL)
    return txtNOTFOUND ;
  else
    return p-src ; }

char txtErrorBuf[txtMAXLEN] ;


// stitching unlimited number of strings. Last parameter should be NULL
tXt txtConcat(tXt tx1,...) {
  tXt rslt = &txtpool[txtpoolidx] ;
  int txtlim = txtpoolidx+txtMAXLEN ;
  tXt x = tx1 ;
  va_list ap;
  va_start(ap,tx1);
  while (x != NULL) {
    int len = strlen(x) ;
    if (txtpoolidx+len > txtlim)
      len = txtlim-txtpoolidx ;
    memcpy(&txtpool[txtpoolidx],x,len) ;
    txtpoolidx += len ;
    if (txtpoolidx >= txtlim) {
      snprintf(txtErrorBuf,txtMAXLEN,"txtConcat length overflow") ;
      x = NULL ; }
    else
      x = va_arg(ap,tXt) ; }
  va_end(ap);
  txtFixpool() ;
  return rslt ;
}

// check for error
int txtAnyError(void) {
  return txtErrorBuf[0] != 0 ; }

//retrieve and reset error message
tXt txtLastError(void) {
  tXt rslt = txtConcat(txtErrorBuf,NULL) ;
  txtErrorBuf[0] = 0 ; 
  return rslt ; }


//convert to uppercase
tXt txtUpcase(tXt s) {
  tXt rslt = txtConcat(s,NULL) ;
  char * p = rslt ;
  while (*p) {
    if (*p >= 'a' && *p <= 'z')
      *p = *p -'a'+'A' ;
    p++ ; }
  return rslt ; }

// make one char string
tXt txtC(char c) {
  tXt rslt = &txtpool[txtpoolidx] ;
  txtpool[txtpoolidx++] = c ;
  txtFixpool() ;
  return rslt ; }



// create string with standard printf format spec
tXt txtPrintf(const char* format, ...)
{
    va_list argList;
    va_start(argList, format);
    char buf[txtMAXLEN] ;
    int n = vsnprintf(buf,txtMAXLEN-1,format, argList);
    if (n >= txtMAXLEN-1)
      snprintf(txtErrorBuf,txtMAXLEN,"txtPrintf overflow (%d >= %d)",n,txtMAXLEN) ;
    va_end(argList);
    return txtConcat(buf,NULL) ;
}

// allocate memomory in the string pool
tXt txtMalloc(int len) {
  tXt rslt = &txtpool[txtpoolidx] ;
  txtpoolidx += len ;
  txtFixpool() ;
  ZeroMemory(rslt,len) ;
  return rslt ; }

//grab the first part of a string
tXt txtEat(tXt * src,char delim) {
  int p = txtPos(*src,txtC(delim)) ;
  tXt rslt = txtSub(*src,0,p) ;
  *src = &((*src)[miniii(p+1,strlen(*src))]) ;
  return rslt ; }


//grab the first part of a string
tXt txtEats(tXt * src,tXt delims) {
  int p = txtNOTFOUND ;
  while (*delims)
    p = miniii(p,txtPos(*src,txtC(*delims++))) ;
  tXt rslt = txtSub(*src,0,p) ;
  *src = &((*src)[miniii(p+1,strlen(*src))]) ;
  return rslt ; }



// reverse a string
tXt txtFlip(tXt s) {
  tXt rslt = txtConcat(s,NULL) ;
  char * p = &rslt[strlen(s)-1] ;
  while (*s)
    *p-- = *s++ ; 
  return rslt ; }

// preserve string
tXt fridge(tXt s) {
  int len = strlen(s) ;
  tXt rslt = (char *) malloc(len+2) ;
  strcpy(rslt,s) ;
  rslt[len+1] = 'F' ;
  return rslt ; }


// release fridged string
void clearfridge(tXt s) {
  if (s != NULL && s != &nullchar) {
    int fpod = strlen(s)+ 1 ;
    if (s[fpod] != 'F') {
      char part[10] ;
      memcpy(part,s,9) ;
      part[9] = 0 ;
      snprintf(txtErrorBuf,txtMAXLEN,"false unfridge (%s....)\n",part) ;}
    else {
      s[fpod] = ' ' ;
      free(s) ; } }
}

// un preserve string
tXt unfridge(tXt s) {
  tXt rslt = txtConcat(s,NULL) ;
  clearfridge(s) ;
  return rslt ; }

// put new value in fridged string
void refridge(tXt * p ,tXt nval) {
  clearfridge(*p) ;
  *p = fridge(nval) ; }

// remove spaces around string
tXt txtTrim(tXt tx) {
  int b = 0 ;
  int e = strlen(tx)-1 ;
  int ee = e ;
  while (b <= e && tx[b] == ' ')
    b++ ;
  while (b <= e && tx[e] == ' ')
    e -- ;
  if (b != 0 || e != ee)  
    tx = txtSub(tx,b,e-b+1) ;
  return tx ; }

// remove all from set of chars around string
tXt txtTrims(tXt tx,tXt whites) {
  int b = 0 ;
  int e = strlen(tx)-1 ;
  int ee = e ;
  while (b <= e && strchr(whites,tx[b]) )
    b++ ;
  while (b <= e && strchr(whites,tx[e]))
    e -- ;
  if (b != 0 || e != ee)  
    tx = txtSub(tx,b,e-b+1) ;
  return tx ; }

// replace words in string
tXt txtReplace(tXt src,tXt old,tXt nw) {
  tXt rslt = txtEmpty ;
  int p = txtPos(src,old) ;
  int nrtry = 0 ;
  while (p < txtNOTFOUND && nrtry++ < txtMAXLEN) {
    rslt = txtConcat(rslt,txtSub(src,0,p),nw,NULL) ;
    src = &src[p+strlen(old)] ;
    p = txtPos(src,old) ; }
  rslt = txtConcat(rslt,src,NULL) ;
  return rslt ; }

char txtENDBUMP = 0 ;
tXt txtEndFile = & txtENDBUMP ;

//read one line from a file
tXt txtFromFile(FILE * fi) {
  char inbuf[txtMAXLEN] ;
  inbuf[0] = 0 ;
  if (fgets(inbuf,txtMAXLEN,fi) == NULL) {
    return txtEndFile ;}
  int li = strlen(inbuf) ; 
  if (li >= txtMAXLEN-1)
    snprintf(txtErrorBuf,txtMAXLEN,"line of %d chars in txtFromFile()",li) ;
  if (li > 0 && inbuf[li-1]=='\n') 
    inbuf[li-1]=0 ; 
  return  txtConcat(inbuf,NULL) ; 
}


#ifndef txtSKIPEXAMP
// some example code
  


typedef struct tLemma {
  tXt tx ;
  int count ;
   } tLemma;
typedef struct tLemma *pLemma;


int wordcompare (const void * a, const void * b)
{
  int rslt = ((pLemma)a)->count -((pLemma)b)->count ;
  if (rslt == 0)
    rslt = strcmp(txtUpcase(((pLemma)a)->tx) ,txtUpcase(((pLemma)b)->tx)) ;
  return rslt ;
}



void wordfrequency(void) {
  FILE * fi = fopen("sample.txt","r") ;
  tXt delims = " \t[]().-,?\"" ;
  if (fi == NULL)
    return ;
  int nrlemma = 32 ;
  int nextlemma = 0 ;
  int i ;
  tXt lastlin ;
  pLemma wlist = (pLemma) malloc(nrlemma*sizeof(tLemma)) ;
  tXt rlin = txtFromFile(fi) ;
  while (rlin != txtEndFile) {
    tXt lin = txtTrims(rlin,delims) ;
    while (lin[0]) {
      tXt wrd = txtTrims(txtEats(&lin,delims) ,delims ) ;
      if (wrd[0]) {
        for(i=(0);i<=(nextlemma-1);i++) {
          if (strcmp(wlist[i].tx,wrd) == 0) {
            wlist[i].count++ ;
            i = nextlemma+100 ; } }
        if (i < nextlemma+10) {
          if (nextlemma == nrlemma) {
            nrlemma += 32 ;
            wlist = realloc(wlist,nrlemma*sizeof(tLemma)) ; }
          wlist[nextlemma].tx = fridge(wrd) ;
          wlist[nextlemma++].count = 1 ; } } }
    rlin = txtFromFile(fi) ; } 
  fclose(fi) ;
  qsort(wlist,nextlemma,sizeof(tLemma),wordcompare) ;
  for (i = 0 ; i <= nextlemma-1;i++)
    printf("%5d %s\n",wlist[i].count,unfridge(wlist[i].tx)) ;
  free(wlist) ;
  printf("%d words\n",nextlemma) ;
}


// int main(int argc, char * argv[]) {
//   /* oldcomment() ;*/
//   wordfrequency() ;
//   if (txtAnyError()) { //check for errors
//     printf("%s\n",txtLastError()) ;
//     //return 1 ; 
//     }
//   tXt s = "123,456,789" ;
//   s = txtReplace(s,"123","321") ; // replace 123 by 321
//   int num = atoi(txtEat(&s,',')) ; // pick the first number
//   printf("num = %d s = %s \n",num,s) ;
//   s = txtPrintf("%s,%d",s,num) ; // printf in new string
//   printf("num = %d s = %s \n",num,s) ;
//   s = txtConcat(s,"<-->",txtFlip(s),NULL) ; // concatenate some strings
//   num = txtPos(s,"987") ; // find position of substring
//   printf("num = %d s = %s \n",num,s) ;
//   s = fridge(s) ;           // preserve string for long time use
//   refridge(&s,txtConcat(txtSub(s,4,7),",123",NULL)) ; // update string in fridge ;
//   printf("num = %d s = %s \n",num,s) ;
//   clearfridge(s) ;          // cleanup heap
//   if (txtAnyError()) { //check for errors
//     printf("%s\n",txtLastError()) ;
//     return 1 ; }
//   return 0 ;
//   }
      
/* output should look like:
num = 321 s = 456,789
num = 321 s = 456,789,321
num = 19 s = 456,789,321<-->123,987,654
num = 19 s = 789,321,123
*/  

#endif
tXt *split(tXt text, tXt c){
    //char buf[] ="abc/qwe/ccd";
    tXt* arr;
    arr = calloc(1, sizeof(tXt*));
    int i = 0;
    char *p = strtok (text, c);
    
    while (p != NULL)
    {
        
        i++;
        
        arr = realloc(arr, i*sizeof(tXt*));
        arr[i-1] = p;
       // printf("%s\n", arr[i]);
        p = strtok (NULL, c);

    }
    return arr;
}
tXt *splitlen(tXt text, tXt c, int* count){
    //char buf[] ="abc/qwe/ccd";
    tXt* arr;
    arr = calloc(1, sizeof(tXt*));
    int i = 0;
    char *p = strtok (text, c);
    
    while (p != NULL)
    {
        
        i++;
        *count = i;
        arr = realloc(arr, i*sizeof(tXt*));
        arr[i-1] = p;
       // printf("%s\n", arr[i]);
        p = strtok (NULL, c);

    }
    return arr;
}
#pragma endregion
#pragma region var

typedef struct _kawaVar{
    char* name;
    char* value;
} kawaVar;
typedef struct _kawaInt{
    char* name;
    int value;
} kawaInt;
typedef struct _kawaFloat
{
    char* name;
    float value;
} kawaFloat;
typedef struct _kawaString
{
    char* name;
    char* value;
} kawaString;
typedef struct _kawaChar
{
    char* name;
    char value;
} kawaChar;
typedef struct _kawaBool
{
    char* name;
    bool value;
} kawaBool;


#pragma endregion
kawaChar* kwchars;
kawaInt* kwints;
kawaString* kwstrings;
kawaFloat* kwfloats;
kawaVar* kwvars;
kawaBool* kwbools;
#define StartAllVars kwchars = calloc(1, sizeof(kawaChar)); \
					 kwints = calloc(1, sizeof(kawaInt)); \
					 kwfloats = calloc(1,sizeof(kawaFloat)); \
					 kwstrings = calloc(1, sizeof(kawaString)); \
					 kwbools = calloc(1, sizeof(kawaBool)); \
					 kwvars = calloc(1, sizeof(kawaVar));

void AddString(tXt name, tXt value){
	kwstrings = realloc(kwstrings, ArrLeng(kwstrings)+1 * sizeof(kawaString));
	strcpy(kwstrings[ArrLeng(kwstrings)].name, name);
	strcpy(kwstrings[ArrLeng(kwstrings)].value, value);
}
void AddChar(tXt name, char value){
	kwchars = realloc(kwchars, ArrLeng(kwchars)+1 * sizeof(kawaChar));
	strcpy(kwchars[ArrLeng(kwchars)].name, name);
	kwchars[ArrLeng(kwchars)].value = value;
}
void AddInt(tXt name, int value){
	kwints = realloc(kwints, ArrLeng(kwints)+1 * sizeof(kawaInt));
	strcpy(kwints[ArrLeng(kwints)].name, name);
	kwints[ArrLeng(kwints)].value = value;
}
void AddFloat(tXt name, float value){
	kwfloats = realloc(kwfloats, ArrLeng(kwfloats)+1 * sizeof(kawaFloat));
	strcpy(kwfloats[ArrLeng(kwfloats)].name, name);
	kwfloats[ArrLeng(kwfloats)].value = value;
}
void AddBool(tXt name, bool value){
	kwbools = realloc(kwbools, ArrLeng(kwbools)+1 * sizeof(kawaBool));
	strcpy(kwbools[ArrLeng(kwbools)].name, name);
	kwbools[ArrLeng(kwbools)].value = value;
}

char *k_strcpy(char *destination, char *source)
{
    char *start = destination;

    while(*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }

    *destination = '\0'; // add '\0' at the end
    return start;
}
int system(const char *command);
int stage = 0;
int lang = C_LANG;
bool addDefault = true;
tXt glFilename;
tXt *code;
tXt* t;
tXt replace(tXt before, tXt oldsub, tXt newsub)
{
  // get the lengths of the before string, old substring and new substring
  int old_length = strlen(oldsub);
  int new_length = strlen(newsub);
  int before_length = strlen(before);

  // stores pointer to the dynamically allocated after string
  tXt after;

  // if the length of the old substring is the same as the new substring,
  // we can allocate space for the after string without doing any analysis
  // of the before string beforehand
  if (old_length == new_length)
  {
    // allocate the same amount of space for the after string as the before
    // string (the +1 is for the null terminator)
    after = txtMalloc((before_length + 1));
  }
  // otherwise we need to figure out how many times does the old substring
  // occur in the before string, and use that information to allocate enough
  // space for the after string
  else
  {
    // keeps track of occurrences of the old substring in the before string
    int occurrences = 0;

    // use i as the current position in the before string and loop through
    // the string until we reach the end of the string
    int i = 0;
    while (i < before_length)
    {
      // if the first occurrence of the old substring in the before string
      // from index i onwards occurs at index i, then we have found an
      // occurrence of the substring at index i
      if (strstr(&before[i], oldsub) == &before[i])
      {
        // we increment the number of occurrences, and increment i by the
        // length of the old substring to skip over the old substring characters
        occurrences++;
        i += old_length;
      }
      // otherwise if we did not find an occurrence, increment i by 1 to check
      // for an occurrence at the next index
      else i++;
    }

    // calculate the difference between the new and old substring lengths
    int sub_diff = new_length - old_length;

    // use the before string's length as a starting point for the after
    // string's length
    int after_length = before_length;

    // and now adjust the after string's length by the number of occurrences
    // in the before string multipled by the difference in length between
    // the new substring and the old substring (and keep in mind if the new
    // substring is less length than the old substring, it will be negative
    // and as appropriate after_length will be less than before_length)
    after_length += occurrences * sub_diff;

    // allocate space for the after string, accounting for the null terminator
    after = malloc((after_length + 1) * sizeof(char));
  }

  // i will keep track of our current index in the before string
  int i = 0;

  // j will keep track of our current index in the after string that we will
  // now build
  int j = 0;

  // again we loop through the before string, just as above, but now we
  // will build the after string as we do so
  while (i < strlen(before))
  {
    // if the first occurrence of the old substring in the before string
    // from index i onwards occurs at index i, then we have found an
    // occurrence of the substring at index i
    if (strstr(&before[i], oldsub) == &before[i])
    {
      // we now copy into the after string AT INDEX J the new substring
      strcpy(&after[j], newsub);

      // we skip over old substring's amount of characters in the before string
      i += old_length;

      // but we add the new substring's amount of characters to j, because the
      // length of the old and new substring's may be different... j may be less
      // or more than i, but we'll use it to keep track of where we are writing
      // next into the after string
      j += new_length;
    }
    // if we did not find an occurrence of the old substring at index i in the
    // before string, we copy the next character from the before string into the
    // after string (using the respective variables), and we increment the
    // the indexes by 1 to move onto the next element in each array
    else
    {
      after[j] = before[i];
      i++;
      j++;
    }
  }

  // put a null terminator at the end of the after string
  after[j] = '\0';

  // return a pointer to the after string
  return after;
}
void removeChar(char * str, char charToRemmove){
    int i, j;
    int len = strlen(str);
    for(i=0; i<len; i++)
    {
        if(str[i] == charToRemmove)
        {
            for(j=i; j<len; j++)
            {
                str[j] = str[j+1];
            }
            len--;
            i--;
        }
    }

}
int wordsCount(char* string){
    int count = 0, i;

    for (i = 0;string[i] != '\0';i++)
    {
        if (string[i] == ' ' && string[i+1] != ' ')
            count++;
    }
    return count;
}
tXt incl;
tXt start;
tXt middle;
tXt end;
tXt* strings;
void AddIncludeAndDefine(tXt str, bool withN){
    if(withN){
        //strcat(&middle, strcat(&str, "\n"));
        incl = txtConcat(incl, txtConcat(str, "\n", NULL), NULL);
    }
    else{
        incl = txtConcat(incl, str, NULL);
    }
}
void AddToStart(tXt str, bool withN){
    if(withN){
        //strcat(&middle, strcat(&str, "\n"));
        start = txtConcat(start, txtConcat(str, "\n", NULL), NULL);
    }
    else{
        start = txtConcat(start, str, NULL);
    }
}
void AddToMiddle(tXt str, bool withN){
    if(withN){
        //strcat(&middle, strcat(&str, "\n"));
        middle = txtConcat(middle, txtConcat(str, "\n", NULL), NULL);
    }
    else{
        middle = txtConcat(middle, str, NULL);
    }
}
void AddToEnd(tXt str, bool withN){
    if(withN){
        //strcat(&middle, strcat(&str, "\n"));
        end = txtConcat(end, txtConcat(str, "\n", NULL), NULL);
    }
    else{
        end = txtConcat(end, str, NULL);
    }
}
char** removeDupWord(char str[], char* tok)
{
    // Returns first token
    char *token = strtok(str, tok);
    char** arr = malloc(wordsCount(str) * sizeof(char*));;

    int count=0;
    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL)
    {
        //printf("%s\n", token);

        arr[count] = malloc((strlen(token)+1) * sizeof(char));
        strcpy(&arr[count], &token);
        count++;
        token = strtok(NULL, tok);
    }
    return arr;
}
// char** split(char* str, char* ch){


//     if(strlen(str) > 0){
//         size_t length = strlen(str);
//         size_t i = 0;
//         int count = 1;

//         for (i = 0; i < length; i++) {
//             if(str[i] == ch){
//                 count++;

//             }  /* Print each character of the string. */
//         }
//         char ** arr = malloc(sizeof(char) * count);
//         count = 0;
//         char* strBefore;
//         for (i = 0; i < length; i++) {
//             if(str[i] == ch){
//                 if(strBefore != NULL || strBefore != ""){
//                     strcpy(&arr[count], &strBefore);
//                     //*arr[count] = *strBefore;
//                     //strcpy(&h1, &h2);
//                    // printf("%s\n", h1);
//                     //arr[count] = strBefore;

//                    // printf("%d count: %s\n", count, &arr[count]);
//                     strcpy(&strBefore, "");
//                 }
//             }
//             else{


//                 strcat(&strBefore, &str[i]);
//                 printf("%c\n", str[i]);
//             }
//             return arr;
//         }
//     }

// }
// void split1(char* text, char* c){
//     //char buf[] ="abc/qwe/ccd";
//     code = calloc(1, sizeof(char**));
//     int i = 0;
//     char *p = strtok (text, c);

//     while (p != NULL)
//     {


//         i++;
//         count1++;
//         code = realloc(code, i*sizeof(char*));
//         strcpy(&code[i], p);
//        // printf("%s\n", arr[i]);
//         p = strtok (NULL, c);

//     }

// }
// void split2(char* text, char* c){
//     //char buf[] ="abc/qwe/ccd";
//     t = calloc(1, sizeof(char**));
//     int i = 0;
//     char *p = strtok (text, c);

//     while (p != NULL)
//     {


//         i++;
//         count2++;
//         t = realloc(t, i*sizeof(char*));
//         sdscpy(t[i], p);
//        // printf("%s\n", arr[i]);
//         p = strtok (NULL, c);

//     }

// }
int main(int argc, char* argv[])
{
    incl = txtEmpty;
    start = txtEmpty;
    middle = txtEmpty;
    end = txtEmpty;
    //split("Hello world!", "j");

        // printf("%s\n", arr[0]);




    //system("gcc -o test ./test/test.c");
    if (argc >= 2){
        printf("Welcome to the Puffin language v. 0.0.2!\nCreated by Dizabanik\n");

        for(int i = 1; i < argc; i++){
            int len = strlen(argv[i]);
            char* last_four = &argv[i][len-3];
            //printf("Args");
            printf("Arguments: %s\n", last_four);
            if(strcmp(last_four, ".kw")){
                printf("File extention is not valid :(\n");
            }
            else if(strcmp(last_four, ".kh")){

            }




        }
        FILE **inputFiles = malloc(sizeof(FILE*) * (argc-1));
        int i;
        char* file = 0;
        int ccc = 0;
        long length;
        FILE * f = fopen (argv[1], "rb");
        glFilename = argv[1];
        if (f)
        {
            fseek (f, 0, SEEK_END);
            length = ftell (f);
            fseek (f, 0, SEEK_SET);
            file = malloc(length);
            if (file)
            {
                fread (file, 1, length, f);
            }
            fclose (f);
        }
        
        if (file)
        {
           
            //printf("%s\n", file);
            // start to process your data / extract strings here...
            //code = txt(file,strlen(file), "\n", 1, &count1);
            code = splitlen(file, "\n", &count1);
            for (int j = 0; j < count1; j++)
                printf("%s\n", code[j]);
        }
        // for (i = 1; i < argc; i++){
        //     inputFiles[i] = fopen(argv[i], "r");

        //    // fscanf(inputFiles[i],"%s", &file);

        // }



       
       
      //  printf("Code::::::::::::::::::\n%s\n", code[0]);
      
        for (size_t j = 0; j < count1; j++)
        {
            t = splitlen(code[j], " ", &count2);
            
            for (size_t i = 0; i < count2; i++)
            {
                if(strcmp(t[i], "print") == 0){
                    printf("PRI");
                }
            }
           
            
        }
        if(addDefault == true){
            if(lang == C_LANG){
                AddIncludeAndDefine("//default defines and includes\n#include<stdio.h>\n#include<stdbool.h>\n#include<stdlib.h>\n#include<string.h>\n#define string char*\n", false);
                //AddIncludeAndDefine("#define TYPE_LIST(a,b)  \\nX(a,b,int)            \\nX(a,b,unsigned int)   \\nX(a,b,short)          \\nX(a,b,unsigned short) \\nX(a,b,char)           \\nX(a,b,signed char)    \\nX(a,b,unsigned char)  \\nX(a,b,float)          \\nX(a,b,double)\n#define X(a,b,type) type: _Generic((b), type: (a) == (b), default: 0),\n#define is_truly_equal(a, b) _Generic((a), TYPE_LIST(a,b) default: 0)\n", false);
            }
        }


       
    

        // char **fff = malloc(ArrLeng(split(file, "n")) * sizeof(char*));

        // for (size_t i = 0; i < ArrLeng(split(file, "n")); i++)
        // {
        //     fff[i] = malloc((5+1) * sizeof(char));
        // }

        // for (size_t i = 0; i < ArrLeng(fff); i++)
        // {
        //     printf("%d\n", ArrLeng(split(file, "n")));
        // }

        




        
    }
    else {
        printf("Welcome to the Puffin language v. 0.0.2!\nCreated by Dizabanik\n");


    }
    while (1)
    {
      /* code */
    }
    

    return 0;

}


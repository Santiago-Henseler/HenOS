#include "string.h"

int32 strLen(const char *str){
    if(str == NULL)
        return -1;

    int len = 0;    
    while (*str){
        len++; str++;
    }
    return len;
}

int strReverse(char *str){
    if(str == NULL)
        return -1;

    int len = strLen(str);

    for (int i = 0; i < len/2 ; i++) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
}

int strCopy(char * src, char * dst){   
    if(src == NULL || dst == NULL)
        return -1;

    memCopy(src, dst, strLen(src)+1);

    return 0;
}

bool strCompare(const char *first, const char *second){
    if(first == NULL || second == NULL)
        return false;

    while (*first && *second) {
        if (*first != *second)
            return false;
        first++;
        second++;
    }
    return *first == *second;
}

int strConcat(char * src, char ** dst){
    if(src == NULL || dst == NULL || *dst == NULL)
        return -1;

    uint32 lenDst = strLen(*dst);
    uint32 lenSrc = strLen(src);

    char * tmp = realloc(*dst, lenDst + lenSrc + 1);
    if(tmp == NULL)
        return -1;

    *dst = tmp;

    memCopy(src, &(*dst)[lenDst], lenSrc + 1);

    return 0;
}

bool strContains(char * str, char c){
    if(str == NULL)
        return false;

    int index = strIndexOf(str, c);

    return  index == -1 ? false : true;
}

int strIndexOf(char * str, char c){
    int index = -1;

    int i = 0;
    while(*str && index != -1){
        if(*str == c)
            index = i;
        str++; i++;
    }

    return index;
}

char * strCutAt(char ** str, char c){

    if(str == NULL)
        return "";

    char * string = *str;
    bool cut = false;

    char * firstHalf = (char *)malloc(strLen(string) + 1);
    char *result = firstHalf;

    while (*string && !cut){
        if(*string == c){
          cut = true;
          *str = string;
          *firstHalf = '\0';
        }else{
          *firstHalf = *string;
          firstHalf++;
        }
        string++;
    }

    return result;
}

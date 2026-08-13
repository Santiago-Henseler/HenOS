#include "stdio.h"

void appendStr(char * src, char ** dst, uint32 memLen, int i){
    if(strLen(*dst)+strLen(src)+1 > memLen){
        *dst = (char *) realloc(*dst, strLen(*dst)+strLen(src)+1);
    }
    memCopy(src, &(*dst)[i], strLen(src)+1);
}

int printf(char* str, ...){
    // TODO: reemplazar printVga por syscall write (cuando exista)
    va_list args;
    va_start(args, str);

    if(!strContains(str, '%')){
        printVga(str, WHITE); 
        return 0;
    }

    char * txt = malloc(1);
    txt[0] = '\0';
    
    while (*str){
        if(*str == '%'){
            str++;
            switch (*str){
                case 's':
                    char * s = va_arg(args, char *);
                    strConcat(s, &txt);
                    break;
                case 'c':
                    char c = va_arg(args, int);
                    char * formatC[2] = {c, '\0'};
                    strConcat(formatC, &txt);
                    break;
                case 'i':
                    int num = va_arg(args, int);
                    char * i = intToStr(num);
                    strConcat(i, &txt);
                    free(i);
                    break;
                case 'u':
                    int uNum = va_arg(args, int);
                    char * u = uIntToStr(uNum);
                    strConcat(u, &txt);
                    free(u);
                    break;
                case 'x':
                    int hex =  va_arg(args, int);
                    char * h = hexToStr(hex);
                    strConcat(h, &txt);
                    free(h);
                    break;
                case 'f':
                    int boolean =  va_arg(args, int);
                    strConcat(boolean ? "True" : "False", &txt);
                    break;
                case 'b':
                    // TODO
                    break;
                case 'v':
                    // TODO
                    break;
                default:
                    char formatD[3] = {'%',*str, '\0'};
                    strConcat(formatD, &txt);
                    break;
            }
        }else{
            char format[2] = {*str, '\0'};
            strConcat(format, &txt);
        }
        str++;
    }

    printVga(txt, WHITE);
    free(txt);
    
    va_end(args);
}

int putChar(char c){
    char * format[2] = {c, '\0'};
    return printf(format);
}

int putInt(int i){
    char * num = intToStr(i);
    return printf(num);
}
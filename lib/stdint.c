#include "stdint.h"

char * numToStr(int num, bool sig){
    char * str = (char *) malloc(sizeof(char) * 12); // Como maximo puedo representar 2**32-1 numeros (entran en 12 caracteres)

    if(str == NULL)
        return NULL;

    int j = 0;
    if(num == 0){
        str[j++] = KEY_0;
        str[j] = '\0';
        return str;
    }

    bool neg = false;
    if(num < 0 && sig){
       num = ~(num)+1;
       neg = true;
    }

    while (num > 0){
        str[j] = (num % DEC_BASE) + KEY_0;
        num /= DEC_BASE;
        j++;
    }
    
    if(neg)
        str[j++] = '-';

    str[j] = '\0';
    strReverse(str);

    return str;
}

char* intToStr(int num){
    return numToStr(num, true);
}

char * uIntToStr(int num){
    return numToStr(num, false);
}

char * hexToStr(int num){
    char * str = (char *) malloc(sizeof(char) * 12);

    if(str == NULL)
        return NULL;

    int j = 0;
    char letra[6] = {'A', 'B', 'C', 'D', 'E', 'F'};

    if(num == 0){
        str[j++] = KEY_0;
        str[j] = '\0';
        return str;
    }

    while (num > 0){
        if(num % HEX_BASE >= 10){
            str[j] = letra[(num % HEX_BASE) % 10];
        }else{
            str[j] = (num % HEX_BASE) + '0';
        }
        num /= HEX_BASE;
        j++;
    }
    str[j++] = 'X';
    str[j++] = '0';
    str[j++] = '\0';
    strReverse(str);

    return str;
}

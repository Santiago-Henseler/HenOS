#ifndef STDINT_H
#define STDINT_H

typedef char  int8;
typedef short int16;
typedef int   int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long long uint64;

#include "stdmem.h"
#include "stdbool.h"    
#include "string.h"

// Bases para int y hexa 
#define HEX_BASE 16
#define DEC_BASE 10

// Alinea a la base
#define ALIGN(addr, align) (((int)(addr) + ((align) - 1)) & ~((align) - 1))

// Dado un int devuelve un string terminado (considerando el signo)
char * intToStr(int num);

// Dado un int devuelve un string terminado
char * uIntToStr(int num);

// Dado un int(en hexa) devuelve un string terminado e iniciado en 0x
char * hexToStr(int num);

// Dado un int lo devuelve en binario en un string
char * uIntToBin(int i);

#endif
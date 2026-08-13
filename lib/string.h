#ifndef STRING_H
#define STRING_H

// Headers extra
#include <ascii.h>
#include <stdmem.h>
#include <stdint.h>
#include <stdbool.h>

// Dado un str terminado en \0 devuelve su largo (sin contar el \0)
int32 strLen(const char *str);

// Dado un str lo invierte
int strReverse(char *str);

// Copia el char* de src a dst
int strCopy(char * src, char * dst);

// Devuelve true si ambos char* son iguales
bool strCompare(const char *first, const char *second);

// Concatena src al final de dst
int strConcat(char * src, char ** dst);

// Devuelve true si el char* contiene el caracter
bool strContains(char * str, char c);

// Devuelve el indice de la primera aparicion del caracter 
int strIndexOf(char * str, char c);

// Devuelve un char * con los char hasta la primera aparicion del caracter y en str devuelve el resto
char * strCutAt(char ** str, char c);

#endif
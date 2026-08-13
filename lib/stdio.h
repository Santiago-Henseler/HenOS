#ifndef STDIO_H
#define STDIO_H

// Headers extra //
#include <vga/vga.h>
#include <string.h>
#include <stdarg.h>

// Muestra en pantalla agregando el formato indicado 
// Soportado:  %s => char*  ;  %c => char ; %f => bool ;
//             %u => uInt   ;  %i => int  ; %x => hexa ; 
//             %v => uBit   ;  %b => bit  ; 
int printf(char* str, ...);

// Wraper a printf para escribir un solo caracter
int putChar(char c);

// Wraper a printf para escribir un entero
int putInt(int i);

#endif
#ifndef STDMEM_H
#define STDMEM_H

// Headers extra
#include <stdio.h>
#include <stdbool.h>
#include <page.h>
#include <syscall.h>

#define NULL 0

// Devuelve un puntero al primer byte de un bloque de size bytes
// Si no se pudo reservar memoria devuelve NULL 
void * malloc(uint32 size);

// Devuelve un puntero al primer byte de un bloque de size bytes inicializado en 0
void * calloc(uint32 size);

// Devuelve un puntero al primer byte de un bloque de size bytes y copia el contenido del bloque aputando por ptr
// Libera la memoria del bloque apuntado por ptr
void * realloc(void * ptr, uint32 size);

// Libera el bloque de memoria indicado
// Si el puntero es invalido su comportamiento no esta definido
void free(void * ptr);

// Copia size bytes del contedio de src en dst 
void memCopy(void * src, void * dst, uint32 size);

// Sete size bytes con value
void memSet(void * dst, uint8 value, uint32 size);

#endif
#ifndef SYSCALL_H
#define SYSCALL_H

// Headers extra
#include <stdint.h>

// TODO: despues tratar de que sea generico donde leo/escribo
extern void write(char * fileName, uint8 * bytes, uint32 size);

extern void read(char * fileName, uint32 size);

#endif
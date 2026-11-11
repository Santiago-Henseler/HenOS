#ifndef TSS_H
#define TSS_H

// Headers extra
#include <stdint.h>
#include <gdt.h>

// Estructura que define el Task State Segment  
// Task State Segment: registro en hardware que almacena el esp del kernel para ser llamado cuando ocurra una interrupcion desde Ring 3 
typedef struct tss {
    uint32 prev_tss;
    uint32 esp;
    uint32 ss;
} __attribute__((packed)) tss;

void initTss();

#endif
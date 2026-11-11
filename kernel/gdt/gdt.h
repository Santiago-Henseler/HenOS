#ifndef GDT_H
#define GDT_H

// Headers extra
#include <stdint.h>

#define GDT_ENTRIES 6

extern uint64 gdt[GDT_ENTRIES];

struct GDTPointer {
    uint16 limit;
    uint32 base;
} __attribute__((packed));

// Inicia la Global Descriptor Table indicando bien los segmentos
void initGdt();

#endif
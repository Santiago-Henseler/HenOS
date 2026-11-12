#ifndef IDT_H
#define IDT_H

// Headres extra
#include <stdint.h>

// Cantidad de entradas
#define IDT_ENTRIES 256

// Flags
#define IDT_PRESENT   0x80
#define IDT_RING0     0x00
#define IDT_RING3     0x60
#define IDT_INT_GATE  0x0E
#define IDT_FLAGS (IDT_PRESENT | IDT_RING0 | IDT_INT_GATE )

// Interrupt Descriptor Table Entry
typedef struct {
    uint16 offset_low;
    uint16 selector;
    uint8  zero;
    uint8  type_attr;
    uint16 offset_high;
} __attribute__((packed)) IDTEntry;

// Interrupt Descriptor Entry Pointer
typedef struct {
    uint16 limit;
    uint32 base;
} __attribute__((packed)) IDTPointer;

// Inicializa las iterrupciones cargando la IDT
void initInterrupts();

#endif
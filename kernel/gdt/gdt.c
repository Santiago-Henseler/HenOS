#include "gdt.h"

extern void load_gdt(struct GDTPointer * gdt);

uint64 gdt[GDT_ENTRIES];

void initGdt(){

    gdt[0] = 0x0000000000000000;
    gdt[1] = 0x00CF9A000000FFFF; // Kernel code segment
    gdt[2] = 0x00CF92000000FFFF; // Kernel data segment 
    gdt[3] = 0x00CFFA000000FFFF; // User code segment
    gdt[4] = 0x00CFF2000000FFFF; // User data segment
    gdt[5] = 0x0000000000000000; // Task state segment

    struct GDTPointer gdtPointer = {
        .limit = sizeof(gdt) - 1 ,
        .base = (uint32)gdt
    };

    load_gdt(&gdtPointer);
}
#include "tss.h"

extern uint32 stack_top;

tss t;

void initTss(){
    t.esp = (uint32)&stack_top;
    t.ss = 0x10;

    uint32 base = (uint32)&t;
    uint32 limit = sizeof(tss) - 1;

    uint64 descriptor = 0;

    descriptor |= (limit & 0xFFFF);
    descriptor |= ((uint64)(base & 0xFFFF)) << 16;
    descriptor |= ((uint64)((base >> 16) & 0xFF)) << 32;
    descriptor |= ((uint64)0x89) << 40;
    descriptor |= ((uint64)((limit >> 16) & 0x0F)) << 48;
    descriptor |= ((uint64)((base >> 24) & 0xFF)) << 56;

    gdt[5] = descriptor;
    
    asm volatile ("ltr %%ax" : : "a"((uint16)0x28));
}

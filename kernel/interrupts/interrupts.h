#ifndef INTERRUPTS_H
#define INTERRUPTS_H

// Headers extra
#include <keyboard/keyboard.h>
#include <stdint.h>
#include <stdio.h>
#include <idt.h>
#include <pic.h>
#include "../process/process.h"    
#include "../drivers/floppyDisk/floppy.h"

typedef struct {
    uint32 gs , fs, es, ds;                               // Data segment
    uint32 edi, esi, ebp, useless, ebx, edx, ecx, eax;    // Registros comunes
    uint32 interrupt;                                     // Numero de interrupcion 
    uint32 code;                                          // Codigo de interrupcion (si lo pushea la cpu)
    uint32 eip, cs, eflags, esp, ss;                      // Pusheado por la cpu en la interrupcion
} __attribute__((packed)) InterruptRegisters;

// Encargado de manejar todas las interrupciones
void interrupthandler(InterruptRegisters * interrupt);

#endif
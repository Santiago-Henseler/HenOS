#ifndef PROCESS_H
#define PROCESS_H

// Headers extra
#include <page.h>
#include <stdint.h>
#include <stdmem.h>

#define MAX_PROCESS 10
#define CODE_SEGMENT 0x280000
#define STACK_SEGMENT 0x290000

typedef struct InterruptRegisters{
    uint32 gs, fs, es, ds;                                // Data segment
    uint32 edi, esi, ebp, useless, ebx, edx, ecx, eax;    // Registros comunes
    uint32 interrupt;                                     // Numero de interrupcion 
    uint32 code;                                          // Codigo de interrupcion (si lo pushea la cpu)
    uint32 eip, cs, eflags, esp, ss;                      // Pusheado por la cpu en la interrupcion
} __attribute__((packed)) InterruptRegisters;

typedef enum procState {
    READY,
    RUNNING,
    FINISHED
} procState;

typedef struct process{
    uint8 pid;      // El pid va a ser la posicion en la processList
    procState state;
    InterruptRegisters regs;
    pageDirectoryEntry * pageDir;
} process;

typedef struct processList{
    process * proc[MAX_PROCESS];
    uint8 size;
} processList;

extern processList * procs;
extern process * actualProcess;

// Inicia los procesos del so seteando todo
void initProcess();

// Crea un nuevo proceso
void newProcess(uint32 ip);

// Ejecuta un proceso en particular
void execProcess(uint8 pid);

// Guarda el estado del proceso en una interrupcion
void interruptProcess(InterruptRegisters regs);

// Borra el proceso en ejecucion
void killProcess();

#endif
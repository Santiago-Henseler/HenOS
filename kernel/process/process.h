#ifndef PROCESS_H
#define PROCESS_H

// Headers extra
#include <stdint.h>
#include <interrupts.h>
#include <page.h>
#include <stdmem.h>

#define MAX_PROCESS 10
#define CODE_SEGMENT 0x280000
#define STACK_SEGMENT 0x290000

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


#endif
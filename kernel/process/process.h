#ifndef PROCESS_H
#define PROCESS_H

// Headers extra
#include <stdint.h>
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
    uint32 ip;
    void * sp;
} process;

typedef struct processList{
    process proc[MAX_PROCESS];
    uint8 size;
} processList;

extern processList * procs;
extern process * actualProcess;

extern void runProcess(void * ip);

void execProcess(uint8 pid);

void initProcess();

void newProcess(uint32 ip);

#endif
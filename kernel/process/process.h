#ifndef PROCESS_H
#define PROCESS_H

// Headers extra
#include <stdint.h>
#include <stdmem.h>

#define MAX_PROCESS 10

typedef struct process{
    uint8 pid;
    void * input;
    void * output;
    void * codeSegment;
    void * stackSegment;
} process;

typedef struct processList{
    process proc[MAX_PROCESS];
    uint8 size;
} processList;

extern processList procs;

extern void runProcess(void * codeSegment);

void initProcess();

void newProcess(void * codeSegment);

#endif
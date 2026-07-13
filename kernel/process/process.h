#ifndef PROCESS_H
#define PROCESS_H

// Headers extra
#include <stdint.h>
#include <stdmem.h>

typedef struct process{
    uint8 pid;
    void * input;
    void * output;
    void * codeSegment;
    void * stackSegment;
} process;

process newProcess();

#endif
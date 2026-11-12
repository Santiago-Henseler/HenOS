#include "process.h"

extern void runProcess(InterruptRegisters * interruptRegs);

process * actualProcess;
processList * procs;

void initProcess(){
    actualProcess = NULL;
    procs = (processList *)calloc(sizeof(processList));
    procs->size = 0;
}

void newProcess(uint32 ip){
    
    if(procs->size >= MAX_PROCESS)
        return;

    process * new = (process *)malloc(sizeof(process));

    new->pid = procs->size;
    new->state = READY;
    
    // Seteo todos los segmentos
    memSet(&new->regs, 0, sizeof(new->regs));
    new->regs.ds = 0x23;
    new->regs.es = 0x23;
    new->regs.ss = 0x23;
    new->regs.fs = 0x23;
    new->regs.gs = 0x23;
    new->regs.cs = 0x1B;
    new->regs.eflags = 0x202;

    int e = mapPage(ALIGN(getRamBlock(), PAGE_SIZE), ((char*) STACK_SEGMENT), PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE | PAGE_FLAG_USER);
    if(e == -1)
        return;

    new->regs.esp = STACK_SEGMENT + PAGE_SIZE; 
    new->regs.eip = ip; // Leer ELF y cargar el codigo a algun lado de memoria

    new->pageDir = PAGE_FLAG_NO_PRESENT; // Setear pageDirectory aca
    
    procs->proc[procs->size] = new;

    procs->size++;
}

void execProcess(uint8 pid){
    process * p = procs->proc[pid];
    runProcess(&p->regs);
}

void killProcess(int pid){
    if(pid >= MAX_PROCESS || pid >= procs->size)
        return;

    // TODO: Liberar las paginas que usa el proceso y cambiar la Page Table
    process * p = procs->proc[pid];
    freePage(p->regs.esp);

    procs->size--;  
    procs->proc[pid] = procs->proc[procs->size];
}
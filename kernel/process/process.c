#include "process.h"

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

    // Seteo una pagina entera de stack
    mapPage(ALIGN(getRamBlock(), PAGE_SIZE), ((char*) STACK_SEGMENT), PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);

    process new = {procs->size, READY, ip, ((char*) STACK_SEGMENT)};
    memCopy((void *)&new, (void *)&procs->proc[procs->size], sizeof(process)); 

    procs->size++;
}

void execProcess(uint8 pid){
    if(pid >= MAX_PROCESS || pid >= procs->size)
        return;

    if(actualProcess != NULL)
        actualProcess->state = READY;

    process p = procs->proc[pid];
    p.state = RUNNING;   
    memCopy((void *)&p, (void *)&procs->proc[pid], sizeof(process));
    memCopy((void *)&p, (void *)actualProcess, sizeof(process));
  
    printf("ejecuto el proceso que corre en %x \n", (char*) p.ip);

    // TODO: Cambiar el Stack Pointer y la Page Table

    runProcess((char*) p.ip);
    printf("finalizo");
}

void killProcess(int pid){
    if(pid >= MAX_PROCESS || pid >= procs->size)
        return;

    // TODO: Liberar las paginas que usa el proceso y cambiar la Page Table
    procs->size--;  
    procs->proc[pid] = procs->proc[procs->size];
}
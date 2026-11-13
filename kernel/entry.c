#include <gdt.h>
#include <tss.h>
#include <idt.h>
#include <page.h>
#include <block.h>
#include <stdio.h>
#include <vga/vga.h>
#include <fileSystem.h>
#include <floppyDisk/floppy.h>

#include <syscallHandler.h>
#include "process/process.h"

extern uint8 kernelEnd;
extern uint8 kernelStart;

// Para debug outB(0xE9, '0');
void main(uint32 memRamSize){
    clearVga();

    initGdt();
    initTss();
    initInterrupts();
    
    uint32 kernelSize = (uint32)&kernelEnd - (uint32)&kernelStart;
    initMemBlock(memRamSize - kernelSize);
    initPageTable(memRamSize - kernelSize);
    initFloppyDisk();
    initFileSystem();
    
    initProcess();

    printf("[Info] Se entro en modo protegido y se activaron las interrupciones \n");
    printf("[Info] Se inicio la memoria en bloques, espacio disponible: %i\n", memRamSize - kernelSize);
    printf("[info] Se inicio la memoria paginada\n");  
    printf("[info] Se inicio el driver del floppy disk\n");  
    printf("[info] Se inicio el sistema de procesos\n");  

    extern uint8 libEnd;
    extern uint8 libStart;

    printf("%x -- %x\n", (uint32)&libStart, (uint32)&libEnd);


    uint8 buffer[FLOPPY_BLOCK_SIZE];
    int err = readFloppyDisk(52, buffer);
     
    mapPage(ALIGN(getRamBlock(), PAGE_SIZE), ((char*) 0x280000), PAGE_FLAG_PRESENT | PAGE_FLAG_USER);
    memCopy(buffer, ((char*) 0x280000), 80);
    newProcess(((char*) 0x280000));
    
    execProcess(0);

    for (;;);
}
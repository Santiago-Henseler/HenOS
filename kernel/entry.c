// TODO: controlar los inputs de todas las lib (NULLs, valores validos etc)
#include <vga/vga.h>
#include <stdio.h>
#include <idt.h>
#include <block.h>
#include <page.h>
#include <syscall.h>
#include <floppyDisk/floppy.h>
#include <fileSystem.h>

#include "process/process.h"

extern uint8 kernelEnd;
extern uint8 kernelStart;

void main(uint32 memRamSize){
  clearVga();
  initInterrupts();

  uint32 kernelSize = (uint32)&kernelEnd - (uint32)&kernelStart;
  initMemBlock(memRamSize - kernelSize);
  initPageTable(memRamSize - kernelSize);
  initFloppyDisk();
  
  initFileSystem();

 // printf("[Info] Se entro en modo protegido y se activaron las interrupciones \n");
  printf("[Info] Se inicio la memoria en bloques, espacio disponible: %i a\n", memRamSize - kernelSize);
  //printf("[info] Se inicio la memoria paginada\n");  
  //printf("[info] Se inicio el driver del floppy disk\n");  

  // TODO: terminar de implementar los procesos
  //
  // initProcess(); 
  //
  // LEVANTAR PROCESO DE DISCO
  //uint8 buffer[FLOPPY_BLOCK_SIZE];
  //int err = readFloppyDisk(52, buffer);
  // 
  //mapPage(ALIGN(getRamBlock(), PAGE_SIZE), ((char*) 0x280000), PAGE_FLAG_PRESENT );
  //memCopy(buffer, ((char*) 0x280000), 80);
  //newProcess(((char*) 0x280000));
  //
  //execProcess(0);

  for (;;);
}
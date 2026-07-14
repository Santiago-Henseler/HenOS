#ifndef PAGE_H
#define PAGE_H

// Headers extra
#include <stdint.h>
#include <block.h>
#include <vga/vga.h>

// Cantidad de entradas en la page directory
#define PAGE_DIR_SIZE 1024

// Cantidad de entradas en la page table
#define PAGE_TABLE_SIZE 1024

// Tamaño de la pagina
#define PAGE_SIZE 4096

// Mascara para obtener la direccion del 
// Page Directory a partir de la Virtual Addres
#define VA_TO_PD_MASK 0xFFC00000

// Mascara para obtener el offset dentro 
// de la Page Table de la Virtual Addres
#define VA_TO_PT_MASK 0x3FF000

// Mascara para obtener la direccion fisica (sin offset)
#define PA_TO_ADDR_MASK 0xFFFFF000

// Mascara para obtener la direccion de la
// Page Table a partir del Page Directory Entry
#define PDE_TO_PT_MASK 0xFFFFF000

// Mascara para obtener el offset de una direccion
#define VA_TO_OFFSET 0x00000FFF

// bits 31..12 de direccion de la page table entry 
// bit 7 para tamaño de pagina (0 = 4Kb)  
// bit 4 para no cachear la pagina
// bit 3 para habilitar el write-through
// bit 2 para control (user = 1)/(kernel = 0) 
// bit 1 para habilitar lectura y escritura
// bit 0 presente
typedef uint32 pageDirectoryEntry;

// La pagina es de acceso global
#define PAGE_FLAG_GLOBAL 0x100
// La pagina no es cacheable
#define PAGE_FLAG_NO_CACHE 0x10
// Habilita write-through
#define PAGE_FLAG_WT 0xF
// User page
#define PAGE_FLAG_USER 0x4
// Kernel page
#define PAGE_FLAG_KERNEL 0x0
// Hablita escribir en la pagina
#define PAGE_FLAG_WRITE 0x2
// Pagina presente
#define PAGE_FLAG_PRESENT 0x1
// Pagina no iniciada
#define PAGE_FLAG_NO_PRESENT 0x0

// bits 31..12 de direccion de una pagina
// bit 8 para indicar pagina global
// bit 6 para indicar si la pagina fue usada  
// bit 4 para no cachear la pagina
// bit 3 para habilitar el write-through
// bit 2 para control (user = 1)/(kernel = 0) 
// bit 1 para habilitar lectura y escritura
// bit 0 presente
typedef uint32 pageTableEntry;

// Inicia las tablas de pagina, mapea el kernel
// la memoria de video y los structs de memoria en bloques
void initPageTable();

// Devuelve un puntero a una direccion de memoria virtual paginada
void * getPage();

void mapPage(void *pa, void *va, uint32 flags);

#endif
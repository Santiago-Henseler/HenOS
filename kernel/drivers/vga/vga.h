#ifndef VGA_H
#define VGA_H

// Headers extra 
#include "../io.h"
#include <ascii.h>

// Tamaño de la pantalla 
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA memory map
#define MEM_VGA ((char*) 0xB8000)

// VGA ports para mover el cursor 
#define COMMAND_PORT_VGA 0x3D4
#define DATA_PORT_VGA 0x3D5

// Colors definition 
#define BLACK 0x0
#define BLUE  0x1
#define GREEN 0x2
#define RED   0x4
#define WHITE 0x7
#define PINK 0xD

// Background color
#define BLUE_BG 0x10
#define BRIGHTNESS_BG 0x80

// Escribo el str* sobre la memoria mapeada de la vga respetando el cursor
void printVga(const char* str, int color);

// Limpio toda la memoria mapeada de la vga 
void clearVga(void);

#endif
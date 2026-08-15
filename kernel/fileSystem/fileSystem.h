#ifndef FILESYSTEM_H
#define FILESYSTEM_H

// Headers extra
#include <floppyDisk/floppy.h>
#include <string.h>
#include <stdbool.h>
#include <stdmem.h>
#include <stdint.h>

// Bloque donde se encuentra el superBloque
#define SUPER_BLOCK_POS 50

// Tamaño del bitmap para poder saber que bloques estan libres
#define FS_BITMAP_SIZE 352 // (FLOPPY_MAX_BLOCK - SUPER_BLOCK_POS - 6 / 8) - 1 (resto 6 y 1 para que sea multiplo de 8)

// Cantidad de dentrys que entran en un bloque
#define FS_DENTRY_BLOCK FLOPPY_BLOCK_SIZE/sizeof(dentry) 

// Codigo para indicar que el fileSystem esta creado
#define FS_CODE 270425

// Tamaño maximo del nombre de un archivo
#define FILE_NAME_SIZE 30
#define FILE_MAX_BLOCKS 50
#define FILE_MAX_SIZE FILE_MAX_BLOCKS * FLOPPY_BLOCK_SIZE

#define MAX_DENTRY_PER_BLOCK 16

typedef enum fileType{
    DIR,
    EXEC,
    DATA
} fileType;

// Inicializa el fileSystem en memoria
void initFileSystem();

// Crea un archivo en el directorio actual
int createFile(fileType type, char * fileName);

// Escribe size bytes en un archivo existente
int writeFile(char * fileName, uint8 * data, uint32 size);

void * readFile(char * fileName);



#endif
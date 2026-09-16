#ifndef FLOPPY_H
#define FLOPPY_H

// Headers extra
#include <stdint.h>
#include <stdbool.h>
#include <stdmem.h>
#include <kernelPanic.h>
#include "../io.h"
#include "../dma.h"

//TODO: Siempre que utilizo el floppy chequear que los bloques no pisen bloques del sisop 

#define FLOPPY_DMA_ADDR 0x1000

// Tamaños de floppy
#define FLOPPY_BLOCK_SIZE 512
#define FLOPPY_HEADS 2
// Floppy de 360KB
#define FLOPPY_360KB 0x01
#define FLOPPY_CYLINDERS_360KB 40
#define FLOPPY_SECTORS_360KB 9
// Floppy de 720KB
#define FLOPPY_720KB 0x03
#define FLOPPY_CYLINDERS_720KB 80
#define FLOPPY_SECTORS_720KB 9
// Floppy de 1,44MB
#define FLOPPY_144MB 0x04
#define FLOPPY_CYLINDERS_144MB 80
#define FLOPPY_SECTORS_144MB 18
// Floppy de 2,88 MB
#define FLOPPY_288MB 0x05
#define FLOPPY_CYLINDERS_288MB 80
#define FLOPPY_SECTORS_288MB 36

#define FLOPPY_DATA_PORT 0x3F5
#define FLOPPY_STATUS_PORT 0x3F4
#define FLOPPY_DIGITAL_OUTPUT_PORT 0x3F2 

#define FLOPPY_VERSION_COMMAND 16
#define FLOPPY_CONFIGURE_COMMAND 19
#define FLOPPY_LOCK_COMMAND 0x94
#define FLOPPY_SEEK_COMMAND 0x0F
#define FLOPPY_SENSE_COMMAND 0x08
#define FLOPPY_RECALIBRATE_COMMAND 0x07

#define FLOPPY_MT 0x80
#define FLOPPY_MFM 0x40

// Parametros para conseguir info sobre los floppys que hay disponibles
#define CMOS_FLOPPY_INFO_OUT 0x70
#define CMOS_FLOPPY_INFO_IN 0x71
#define CMOS_FLOPPY_INFO (1 << 7) | 0x10

// Inicia el floppy disk 
void initFloppyDisk();

// Indica que se recibio una interrupcion por parte del floppy
void setFloppyInt();

// Permite escribir un bloque de memoria en el floppy disk
int writeFloppyDisk(int blockNum, uint8 buffer[FLOPPY_BLOCK_SIZE]);

// Permite leer un bloque de memoria del floppy disk
int readFloppyDisk(int blockNum, uint8 buffer[FLOPPY_BLOCK_SIZE]);

#endif
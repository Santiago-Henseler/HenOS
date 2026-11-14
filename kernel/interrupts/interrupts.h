#ifndef INTERRUPTS_H
#define INTERRUPTS_H

// Headers extra
#include <idt.h>
#include <pic.h>
#include <stdio.h>
#include <sched.h>
#include <stdint.h>
#include <process.h>
#include <syscallHandler.h>
#include <floppyDisk/floppy.h>
#include <keyboard/keyboard.h>
   

// Total de interrupciones por hardware
#define HARDWARE_INT 16

// Cantidad de interrupciones por software
#define SOFTWARE_INT 32 

// Para las syscalls
#define EXTRA_INT 2

// Encargado de manejar todas las interrupciones
void interrupthandler(InterruptRegisters * interrupt);

#endif
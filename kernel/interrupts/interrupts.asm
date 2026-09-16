extern mapPic

global idtLoad
idtLoad:
    mov eax, [esp+4]
    lidt [eax]
    call mapPic                     ; Mapeo las interrupciones por hardware
    sti                             ; Activo todas las interrupciones
    ret

extern interrupthandler

commonInterruptHandler:    
    pusha                           ; Pusheo registros de proposito general al stack
                  
    push ds                         ; Guardo el data segment actual
    push es
    push fs
    push gs

    mov ax, 0x10                    ; kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp

    call interrupthandler

    add  esp, 4                      ; Paso de largo el stack pointer que pushie antes del call
    
    pop gs
    pop fs
    pop es
    pop ds                           ; Reestablezco el segmento de datos

    popa

    add esp, 8

    iret

;; Declaración de todos los interrupt handlers usando macro
%macro interruptHandlerNoCode 1
    global interruptHandlerNoCode%1
    interruptHandlerNoCode%1:
        push dword 0                ; Codigo de error 0
        push dword %1               ; Codigo de la interrupcion
        jmp commonInterruptHandler
%endmacro

%macro interruptHandlerCode 1
    global interruptHandlerCode%1
    interruptHandlerCode%1:
        ; El codigo de error lo da la cpu
        push dword %1               ; Codigo de la interrupcion
        jmp commonInterruptHandler
%endmacro

interruptHandlerNoCode 0
interruptHandlerNoCode 1
interruptHandlerNoCode 2
interruptHandlerNoCode 3
interruptHandlerNoCode 4
interruptHandlerNoCode 5
interruptHandlerNoCode 6
interruptHandlerNoCode 7
interruptHandlerCode 8
interruptHandlerNoCode 9
interruptHandlerCode 10
interruptHandlerCode 11
interruptHandlerCode 12
interruptHandlerCode 13
interruptHandlerCode 14
interruptHandlerNoCode 15
interruptHandlerNoCode 16
interruptHandlerCode 17
interruptHandlerNoCode 18
interruptHandlerNoCode 19
interruptHandlerNoCode 20
interruptHandlerNoCode 21
interruptHandlerNoCode 22
interruptHandlerNoCode 23
interruptHandlerNoCode 24
interruptHandlerNoCode 25
interruptHandlerNoCode 26
interruptHandlerNoCode 27
interruptHandlerNoCode 28
interruptHandlerNoCode 29
interruptHandlerNoCode 30
interruptHandlerNoCode 31
interruptHandlerNoCode 32
interruptHandlerNoCode 33
interruptHandlerNoCode 34
interruptHandlerNoCode 35
interruptHandlerNoCode 36
interruptHandlerNoCode 37
interruptHandlerNoCode 38
interruptHandlerNoCode 39
interruptHandlerNoCode 40
interruptHandlerNoCode 41
interruptHandlerNoCode 42
interruptHandlerNoCode 43
interruptHandlerNoCode 44
interruptHandlerNoCode 45
interruptHandlerNoCode 46
interruptHandlerNoCode 47
interruptHandlerNoCode 48
interruptHandlerNoCode 49
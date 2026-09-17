; Defini la int 0x30 como syscall

global write
write:
    mov eax, 0x0        ; Cargo el numero de syscall 
    mov ebx, [esp+4]    ; Donde escribir
    mov ecx, [esp+8]    ; Puntero a los bytes
    mov edx, [esp+12]   ; Size a escribir
    int 0x30    
    ret

global read
read:
    mov eax, 0x1        ; Cargo el numero de syscall 
    mov ebx, [esp+4]    ; De donde leer
    mov ecx, [esp+8]    ; Size a leer
    int 0x30    
    ret
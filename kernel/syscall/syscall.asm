global syscall
syscall:
    mov eax, [esp+4]        ; Cargo el numero de syscall
    int 0x30                ; Defini la int 0x30 como la syscall
    ret
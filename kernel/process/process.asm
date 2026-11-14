global runProcess
runProcess:
    mov eax, [esp+4] 
    mov esp, eax

    pop gs
    pop fs
    pop es
    pop ds  ;; Reestablezco el segmento de datos

    popa    ;; Seteo todos los registros comunes

    add esp, 8

    iret    ;; Cargo el resto de registros
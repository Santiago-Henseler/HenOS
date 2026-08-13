global runProcess
runProcess:
    mov eax, [esp+4] 
    mov esp, eax

    popa    ;; Seteo todos los registros comunes
    iret    ;; Cargo el resto de registros
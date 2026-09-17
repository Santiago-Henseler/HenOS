#include "interrupts.h"

char * interruptName[SOFTWARE_INT] = {
    "Se intento dividir por 0", "Debug int", "NMI int",
    "Breackpoint int", "Ocurrio un overflow", "Operacion se fue de rango",
    "Codigo de operación invalido", "PC sin coprocesador", "Doble fallo",
    "", "TSS invalido", "Segmento no presente",
    "Fallo de segmento del stack", "Referencia a memoria protegida", "Fallo de pagina",
    "", "Error en la unidad de punto flotante", "",
    "", "", "", "", "","","","","","","","","",""
};

void interruptSoftware(InterruptRegisters * interruptRegs){

    printf("Interrupt: %i Code: %x \n", interruptRegs->interrupt, interruptRegs->code);
    printf("%s \n", interruptName[interruptRegs->interrupt]);
    printf("eip => %x  esp => %x  flags => %x \n", interruptRegs->eip, interruptRegs->esp, interruptRegs->eflags);

    for(;;); // TODO: Notificar y cambiar de proceso
}

void interruptHardware(InterruptRegisters * interruptRegs){
    
    printf("Interrupt: %i \n", interruptRegs->interrupt);

    // ACK de la interrupcion al PIC
    ackPic(interruptRegs->interrupt-SOFTWARE_INT);
}

void keyboardInterrupt(InterruptRegisters *interruptRegs){
    
    char key = getKeyInput();
    // TODO: en vez de printear enviar al proceso que esta corriendo
    putChar(key);

    // ACK de la interrupcion al PIC
    ackPic(interruptRegs->interrupt-SOFTWARE_INT);
}

void clockInterrupt(InterruptRegisters * interruptRegs){
    // TODO: cambiar de contexto cuando haya multitask

    // ACK de la interrupcion al PIC
    ackPic(interruptRegs->interrupt-SOFTWARE_INT);
}

void floppyDiskInterrupt(InterruptRegisters * interruptRegs){
    setFloppyInt();

    // ACK de la interrupcion al PIC
    ackPic(interruptRegs->interrupt-SOFTWARE_INT);
}

// Hardware interrupt handlers
void (* hardwareHandlers[HARDWARE_INT])(InterruptRegisters * interruptRegs) = {
    &clockInterrupt, &keyboardInterrupt, &interruptHardware,
    &interruptHardware, &interruptHardware, &interruptHardware,
    &floppyDiskInterrupt, &interruptHardware, &interruptHardware,
    &interruptHardware, &interruptHardware, &interruptHardware,
    &interruptHardware, &interruptHardware, &interruptHardware,
    &interruptHardware
};

void syscallInt(InterruptRegisters * interruptRegs){
    interruptRegs->eax = syscallHandler(interruptRegs->eax, interruptRegs->ebx, interruptRegs->ecx, interruptRegs->edx);
}

void kernelPanicInt(InterruptRegisters * interruptRegs){
    printVga("[ERROR] KERNEL PANIC", RED); 
    for(;;);
}

void (* extraIntHandlers[EXTRA_INT])(InterruptRegisters * interruptRegs) ={
    &syscallInt, &kernelPanicInt
};

void interrupthandler(InterruptRegisters * interruptRegs){

    if(interruptRegs->interrupt > (SOFTWARE_INT + HARDWARE_INT + EXTRA_INT)){
        printf("[Error]: interrupcion no soportada\n");
        for(;;);
    }

    if(interruptRegs->interrupt < SOFTWARE_INT){
        interruptSoftware(interruptRegs);
    }else if(interruptRegs->interrupt > SOFTWARE_INT+HARDWARE_INT-1){
        extraIntHandlers[interruptRegs->interrupt - (SOFTWARE_INT+HARDWARE_INT)](interruptRegs);
    }else{
        hardwareHandlers[interruptRegs->interrupt-SOFTWARE_INT](interruptRegs);
    }
}

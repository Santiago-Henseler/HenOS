ENTRY(kernelEntry)

SECTIONS {
    . = 0x100000;

    kernelStart = .;

    .text : {
        kernelEntry.o(.text)
        *(EXCLUDE_FILE (libo/*.o) .text*)
    }

    .rodata : {
        *(EXCLUDE_FILE (libo/*.o) .rodata*)
    }

    .data : {
        *(EXCLUDE_FILE (libo/*.o) .data*)
    }

    .bss : {
        *(EXCLUDE_FILE (libo/*.o) .bss*)
        *(COMMON)
    }

    . = ALIGN(4096);
    libStart = .;

    .libtext : {
        libo/*.o(.text*)
    }

    .librodata : {
        libo/*.o(.rodata*)
    }

    .libdata : {
        libo/*.o(.data*)
    }

    .libbss : {
        libo/*.o(.bss*)
    }

    . = ALIGN(4096);
    libEnd = .;

    . = ALIGN(16);
    stack_bottom = .;
    . += 16384;
    stack_top = .;

    kernelEnd = .;
}
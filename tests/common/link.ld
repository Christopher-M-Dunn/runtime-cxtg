OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    DRAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
    . = 0x80000000;

    .text :
    {
        *(.text.start)      /* _start must come first */
        *(.text*)
    } > DRAM

    .rodata :
    {
        *(.rodata*)
    } > DRAM

    .data :
    {
        *(.data*)
    } > DRAM

    .bss :
    {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        _bss_end = .;
    } > DRAM

    /* 16 KiB stack above .bss */
    . = ALIGN(16);
    _stack_bottom = .;
    . += 16K;
    _stack_top = .;
}
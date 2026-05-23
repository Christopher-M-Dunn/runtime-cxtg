#ifndef UART_H
#define UART_H

/*
 * QEMU virt UART — NS16550A compatible, mapped at 0x10000000.
 * Writing a byte to the base address transmits it immediately
 * in QEMU's nographic mode (no FIFO drain required).
 */

#define UART_BASE 0x10000000UL

static inline void
uart_putchar(int c)
{
    volatile char *uart = (volatile char *)UART_BASE;
    *uart = (char)c;
}

static inline void
uart_puts(const char *s)
{
    while (*s) {
        uart_putchar(*s++);
    }
}

static inline void
uart_print_hex(unsigned long val)
{
    static const char digits[] = "0123456789abcdef";
    int shift;
    uart_puts("0x");
    for (shift = 60; shift >= 0; shift -= 4) {
        uart_putchar(digits[(val >> shift) & 0xf]);
    }
}

#endif /* UART_H */
*← [Main Index](../../../index.md)*

# tests/common/uart.h

QEMU virt UART output helpers. NS16550A mapped at `0x10000000`; writing a byte to the base address transmits it immediately in QEMU's `-nographic` mode.

**Works with:**
- [test.h](test.h.md) — includes `uart.h`; `CHECK` macro uses `uart_puts` for failure messages
- [crt0.S](crt0.S.md) — has its own internal `_puts` using the same UART address; the two are independent

---

### `UART_BASE`

```c
#define UART_BASE 0x10000000UL
```

MMIO base address of the QEMU virt UART. Writing one byte here transmits it; no FIFO status check required under QEMU emulation.

---

### `uart_putchar`

```c
static inline void
uart_putchar(int c)
{
    volatile char *uart = (volatile char *)UART_BASE;
    *uart = (char)c;
}
```

Transmits one character. `volatile` prevents the compiler from eliding the store. Used by `uart_puts` and `uart_print_hex`.

---

### `uart_puts`

```c
static inline void
uart_puts(const char *s)
{
    while (*s) {
        uart_putchar(*s++);
    }
}
```

Writes a null-terminated string character by character. Used by the `CHECK` macro in `test.h` to print failure messages.

---

### `uart_print_hex`

```c
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
```

Prints an unsigned 64-bit value as `0x` followed by 16 hex digits (zero-padded). Useful for printing CSR values in failing tests.
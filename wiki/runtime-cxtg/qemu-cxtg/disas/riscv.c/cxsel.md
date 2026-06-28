*← [Main Index](../../../../index.md)* | *↑ [riscv.c](../riscv.c.md)* | *↗ [view source](../../../../../qemu-cxtg/disas/riscv.c)*

# disas/riscv.c — cxsel

Disassembler name entry for CSR_CXSEL.

```c
static const char *csr_name(int csrno)
{
    switch (csrno) {
    case 0x0CA0: return "cxsel";   /* standard URO user range */
```
*← [Main Index](../../../../index.md)* | *↑ [riscv.c](../riscv.c.md)* | *↗ [view source](../../../../../qemu-cxtg/disas/riscv.c)*

# disas/riscv.c — cxsdata

Disassembler name entry for CSR_CXSDATA.

```c
static const char *csr_name(int csrno)
{
    switch (csrno) {
    case 0x019:  return "cxsdata"; /* URW user range */
```
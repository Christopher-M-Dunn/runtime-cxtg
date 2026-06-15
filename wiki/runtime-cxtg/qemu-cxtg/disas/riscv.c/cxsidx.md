# disas/riscv.c — cxsidx

Disassembler name entry for CSR_CXSIDX.

```c
static const char *csr_name(int csrno)
{
    switch (csrno) {
    case 0x018:  return "cxsidx";  /* URW user range */
```
# disas/riscv.c — cxidx

function: disassembler name translation for CSR cxidx (cxsidx)

```c
static const char *csr_name(int csrno)
{
    switch (csrno) {
    case 0x0801: return "cxidx"; // Unprivileged (User), Custom read-write (64 CSRs)
```

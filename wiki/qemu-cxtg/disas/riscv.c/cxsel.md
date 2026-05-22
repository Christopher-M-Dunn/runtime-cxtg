# disas/riscv.c — cxsel

function: disassembler name translation for CSR cxsel

```c
static const char *csr_name(int csrno)
{
    switch (csrno) {
    case 0x0800: return "cxsel"; // Unprivileged (User), Custom read-only (64 CSRs)
```

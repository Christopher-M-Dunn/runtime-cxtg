# disas/riscv.c — cxdata

function: disassembler name translation for CSR cxdata (cxsdata)

```c
static const char *csr_name(int csrno)
{
    switch (csrno) {
    case 0x0802: return "cxdata"; // Unprivileged (User), Custom read-write (64 CSRs)
```

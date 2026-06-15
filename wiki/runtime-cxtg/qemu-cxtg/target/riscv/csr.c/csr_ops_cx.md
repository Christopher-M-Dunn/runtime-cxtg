# target/riscv/csr.c — csr_ops[] CX entries

Registers CSR_CXSEL, CSR_CXSIDX, CSR_CXSDATA in the `csr_ops[]` dispatch table.

cxsdata uses the `.op` field instead of separate `.read`/`.write`. When `.op` is set, QEMU calls it exclusively (skipping `.read`/`.write`), passing `write_mask` to indicate the instruction variant. This ensures `env->cxsidx++` fires exactly once per instruction even for `csrrw rd≠x0` (which would otherwise invoke both read and write handlers).

```c
/* csr_ops[] is an array of riscv_csr_operations indexed by CSR address */
static riscv_csr_operations csr_ops[CSR_TABLE_SIZE] = {
    ...
    [CSR_CXSEL]   = { "cxsel",   cxsel,   read_cxsel,  write_cxsel },
    [CSR_CXSIDX]  = { "cxsidx",  cxsidx,  read_cxsidx, write_cxsidx },
    [CSR_CXSDATA] = { "cxsdata", cxsdata, .op = op_cxsdata },
```
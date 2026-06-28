*← [Main Index](../../../../../index.md)* | *↑ [csr.c](../csr.c.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/csr.c)*

# target/riscv/csr.c — write_cxsidx

function: CSR write handler for CSR_CXSIDX; delegates to `cxsidx_csr_write()` in `cx.c`

```c
static RISCVException write_cxsidx(CPURISCVState *env, int csrno,
                                       target_ulong new_value, uintptr_t ra)
{
    cxsidx_csr_write(env, csrno, new_value);
    return RISCV_EXCP_NONE;
}
```

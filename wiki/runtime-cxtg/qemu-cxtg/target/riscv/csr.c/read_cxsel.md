*← [Main Index](../../../../../index.md)* | *↑ [csr.c](../csr.c.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/csr.c)*

# target/riscv/csr.c — read_cxsel

function: CSR read handler for CSR_CXSEL; delegates to `cxsel_csr_read()` in `cx.c`

```c
static RISCVException read_cxsel(CPURISCVState *env, int csrno,
                                       target_ulong *val)
{
    cxsel_csr_read(env, csrno, val);
    return RISCV_EXCP_NONE;
}
```

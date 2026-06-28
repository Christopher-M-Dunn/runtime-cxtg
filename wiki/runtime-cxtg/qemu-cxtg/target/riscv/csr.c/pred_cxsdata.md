*← [Main Index](../../../../../index.md)* | *↑ [csr.c](../csr.c.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/csr.c)*

# target/riscv/csr.c — cxsdata (predicate)

Access predicate for CSR_CXSDATA; returns `RISCV_EXCP_ILLEGAL_INST` when `ext_zcx` is not set.

```c
static RISCVException cxsdata(CPURISCVState *env, int csrno)
{
    if (!riscv_cpu_cfg(env)->ext_zcx) {
        return RISCV_EXCP_ILLEGAL_INST;
    }
    return RISCV_EXCP_NONE;
}
```
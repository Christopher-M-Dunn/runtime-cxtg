# target/riscv/csr.c — cxsidx (predicate)

Access predicate for CSR_CXSIDX; returns `RISCV_EXCP_ILLEGAL_INST` when `ext_zcx` is not set.

```c
static RISCVException cxsidx(CPURISCVState *env, int csrno)
{
    if (!riscv_cpu_cfg(env)->ext_zcx) {
        return RISCV_EXCP_ILLEGAL_INST;
    }
    return RISCV_EXCP_NONE;
}
```
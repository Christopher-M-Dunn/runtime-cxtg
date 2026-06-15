# target/riscv/csr.c — cxsel (predicate)

Access predicate for CSR_CXSEL; returns `RISCV_EXCP_ILLEGAL_INST` when `ext_zcx` is not set.

```c
static RISCVException cxsel(CPURISCVState *env, int csrno)
{
    if (!riscv_cpu_cfg(env)->ext_zcx) {
        return RISCV_EXCP_ILLEGAL_INST;
    }
    return RISCV_EXCP_NONE;
}
```
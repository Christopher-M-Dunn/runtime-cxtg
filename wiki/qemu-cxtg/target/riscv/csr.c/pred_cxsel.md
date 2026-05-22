# target/riscv/csr.c — cxsel (predicate)

function: access predicate for CSR_CXSEL; returns `RISCV_EXCP_NONE` when `ext_zicx` is set, `RISCV_EXCP_ILLEGAL_INST` otherwise

```c
static RISCVException cxsel(CPURISCVState *env, int csrno)
{
    if (riscv_cpu_cfg(env)->ext_zicx) {
        return RISCV_EXCP_NONE;
    }

    return RISCV_EXCP_ILLEGAL_INST;
}
```

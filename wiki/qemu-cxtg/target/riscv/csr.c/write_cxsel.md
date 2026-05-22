# target/riscv/csr.c — write_cxsel

function: CSR write handler for CSR_CXSEL; delegates to `cxsel_csr_write()` (which is a no-op) then returns `RISCV_EXCP_ILLEGAL_INST` because cxsel is read-only per spec

```c
static RISCVException write_cxsel(CPURISCVState *env, int csrno,
                                       target_ulong new_value, uintptr_t ra)
{
    cxsel_csr_write(env, csrno, new_value);
    return RISCV_EXCP_ILLEGAL_INST;
}
```

# target/riscv/csr.c — write_cxsdata

function: CSR write handler for CSR_CXSDATA; delegates to `cxsdata_csr_write()` in `cx.c`

```c
static RISCVException write_cxsdata(CPURISCVState *env, int csrno,
                                       target_ulong new_value, uintptr_t ra)
{
    cxsdata_csr_write(env, csrno, new_value);
    return RISCV_EXCP_NONE;
}
```

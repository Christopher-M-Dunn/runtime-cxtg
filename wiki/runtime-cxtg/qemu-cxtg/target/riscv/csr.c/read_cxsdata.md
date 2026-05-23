# target/riscv/csr.c — read_cxsdata

function: CSR read handler for CSR_CXSDATA; delegates to `cxsdata_csr_read()` in `cx.c`

```c
static RISCVException read_cxsdata(CPURISCVState *env, int csrno,
                                       target_ulong *val)
{
    cxsdata_csr_read(env, csrno, val);
    return RISCV_EXCP_NONE;
}
```

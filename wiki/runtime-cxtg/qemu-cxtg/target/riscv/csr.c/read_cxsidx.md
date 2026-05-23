# target/riscv/csr.c — read_cxsidx

function: CSR read handler for CSR_CXSIDX; delegates to `cxsidx_csr_read()` in `cx.c`

```c
static RISCVException read_cxsidx(CPURISCVState *env, int csrno,
                                       target_ulong *val)
{
    cxsidx_csr_read(env, csrno, val);
    return RISCV_EXCP_NONE;
}
```

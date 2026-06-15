# target/riscv/csr.c — op_cxsdata

Thin wrapper registered as `.op` in `csr_ops[CSR_CXSDATA]`; delegates to `cxsdata_csr_op()` in `cx.c`.

```c
static RISCVException op_cxsdata(CPURISCVState *env, int csrno,
                                     target_ulong *ret_value,
                                     target_ulong new_value,
                                     target_ulong write_mask)
{
    return cxsdata_csr_op(env, csrno, ret_value, new_value, write_mask);
}
```
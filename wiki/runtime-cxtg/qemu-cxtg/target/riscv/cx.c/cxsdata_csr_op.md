# target/riscv/cx.c — cxsdata_csr_op

Combined `.op` handler for all cxsdata CSR instruction variants. Registered as `.op` in `csr_ops[]` instead of separate `.read`/`.write` so that `cxsidx` increments exactly once per instruction regardless of variant.

`write_mask` encodes the instruction type: `0` = read only (`csrr`), `-1` = full replace (`csrrw`/`csrw`), partial mask = `csrrs`/`csrrc`.

```c
RISCVException cxsdata_csr_op(CPURISCVState *env, int csrno,
                               target_ulong *ret_value,
                               target_ulong new_value, target_ulong write_mask)
{
    target_ulong old = env->cxsdata;

    if (ret_value) {
        *ret_value = old;
        trace_cxsdata_csr_read(env->mhartid, csrno, old);
    }
    if (write_mask) {
        env->cxsdata = (old & ~write_mask) | (new_value & write_mask);
        trace_cxsdata_csr_write(env->mhartid, csrno, env->cxsdata);
    }
    env->cxsidx++;
    return RISCV_EXCP_NONE;
}
```
# target/riscv/cx.c — cxsidx_csr_write

Writes `val` to `env->cxsidx`; emits `trace_cxsidx_csr_write` with hartid, register index, and value.

```c
void cxsidx_csr_write(CPURISCVState *env, uint32_t reg_index, target_ulong val)
{
    env->cxsidx = val;
    trace_cxsidx_csr_write(env->mhartid, reg_index, val);
}
```
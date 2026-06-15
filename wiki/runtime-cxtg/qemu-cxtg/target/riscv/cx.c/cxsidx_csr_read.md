# target/riscv/cx.c — cxsidx_csr_read

Reads `env->cxsidx` into `*val`; emits `trace_cxsidx_csr_read` with hartid, register index, and value.

```c
void cxsidx_csr_read(CPURISCVState *env, uint32_t reg_index, target_ulong *val)
{
    *val = env->cxsidx;
    trace_cxsidx_csr_read(env->mhartid, reg_index, *val);
}
```
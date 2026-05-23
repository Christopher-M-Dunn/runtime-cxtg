# target/riscv/cx.c — cxsdata_csr_read

function: reads `env->cxdata` into `*val`; emits `trace_cxsdata_csr_read` with hartid, register index, and value

```c
void cxsdata_csr_read(CPURISCVState *env, uint32_t reg_index, target_ulong *val)
{
    *val = env->cxdata;
    trace_cxsdata_csr_read(env->mhartid, reg_index, *val);
}
```

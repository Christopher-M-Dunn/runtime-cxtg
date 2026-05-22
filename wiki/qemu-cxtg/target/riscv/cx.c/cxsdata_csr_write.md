# target/riscv/cx.c — cxsdata_csr_write

function: writes `val` to `env->cxdata`; emits `trace_cxsdata_csr_write` with hartid, register index, and value

```c
void cxsdata_csr_write(CPURISCVState *env, uint32_t reg_index, target_ulong val)
{
    env->cxdata = val;
    trace_cxsdata_csr_write(env->mhartid, reg_index, val);
}
```

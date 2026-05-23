# target/riscv/cx.c — cxsel_csr_read

function: reads `env->cxsel` into `*val`; emits `trace_cxsel_csr_read` with hartid, register index, and value

```c
void cxsel_csr_read(CPURISCVState *env, uint32_t reg_index, target_ulong *val)
{
    *val = env->cxsel;
    trace_cxsel_csr_read(env->mhartid, reg_index, *val);
}
```

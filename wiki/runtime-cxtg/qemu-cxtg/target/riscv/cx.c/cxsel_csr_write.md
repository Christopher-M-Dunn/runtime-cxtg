# target/riscv/cx.c — cxsel_csr_write

function: no-op write handler for cxsel (read-only per spec); emits `trace_cxsel_csr_write` with the attempted value for observability

```c
void cxsel_csr_write(CPURISCVState *env, uint32_t reg_index, target_ulong val)
{
    trace_cxsel_csr_write(env->mhartid, reg_index, val);
    // Note: CSR cxsel is read-only!
}
```

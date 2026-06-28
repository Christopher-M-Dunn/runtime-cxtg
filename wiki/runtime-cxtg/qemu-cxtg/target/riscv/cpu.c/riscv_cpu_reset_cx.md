*← [Main Index](../../../../../index.md)* | *↑ [cpu.c](../cpu.c.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/cpu.c)*

# target/riscv/cpu.c — riscv_cpu_reset (CX zeroing)

CPU reset handler; zeroes all three CX CSR state fields when `ext_zcx` is enabled, implementing the spec requirement that cxsel is 0 at reset.

```c
static void riscv_cpu_reset(CPUState *cs)
{
    ...
    if (riscv_cpu_cfg(env)->ext_zcx) {
        env->cxsel = 0;
        env->cxsidx = 0;
        env->cxsdata = 0;
    }
```
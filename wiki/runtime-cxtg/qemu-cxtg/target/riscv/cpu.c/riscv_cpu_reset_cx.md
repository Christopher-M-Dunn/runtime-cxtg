# target/riscv/cpu.c — riscv_cpu_reset (CX zeroing)

function: CPU reset handler; zeroes all three CX CSR state fields when `ext_zicx` is enabled, implementing the spec requirement that cxsel is 0 at reset

```c
    if (riscv_cpu_cfg(env)->ext_zicx) {
        env->cxsel = 0;
        env->cxidx = 0;
        env->cxdata = 0;
    }
```

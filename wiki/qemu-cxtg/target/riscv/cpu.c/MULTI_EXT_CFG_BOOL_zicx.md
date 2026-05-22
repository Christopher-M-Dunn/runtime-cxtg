# target/riscv/cpu.c — MULTI_EXT_CFG_BOOL("zicx")

macro call: declares `zicx` as a multi-extension boolean CPU property, default off; exposes it as a QEMU CPU property (e.g. `-cpu rv64,zicx=on`)

```c
static const RISCVCPUMultiExtConfig riscv_cpu_extensions[] = {
    ...
    MULTI_EXT_CFG_BOOL("zicx", ext_zicx, false),
```

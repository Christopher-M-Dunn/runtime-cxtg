# target/riscv/cpu.c — MULTI_EXT_CFG_BOOL("zcx")

Declares `zcx` as a multi-extension boolean CPU property, default off; exposes it as a QEMU CPU property (e.g. `-cpu rv64,zcx=on`).

```c
static const RISCVCPUMultiExtConfig riscv_cpu_extensions[] = {
    ...
    MULTI_EXT_CFG_BOOL("zcx", ext_zcx, false),
```
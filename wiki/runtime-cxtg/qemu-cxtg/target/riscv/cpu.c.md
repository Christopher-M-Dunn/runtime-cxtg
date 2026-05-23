# qemu-cxtg/target/riscv/cpu.c

RISC-V CPU initialization and ISA extension registration. Manages CPU config, reset, and extension properties.

## Extension registration

- [ISA_EXT_DATA_ENTRY(zcx)](cpu.c/ISA_EXT_DATA_ENTRY_zicx.md) — registers zcx in the ISA extension table with `PRIV_VERSION_1_10_0` and `ext_zcx` config field
- [MULTI_EXT_CFG_BOOL(zcx)](cpu.c/MULTI_EXT_CFG_BOOL_zicx.md) — declares zcx as a multi-extension boolean config property (default off)

## CPU reset

- [riscv_cpu_reset (cxsel/cxsidx/cxsdata zeroing)](cpu.c/riscv_cpu_reset_cx.md) — zeroes `env->cxsel`, `env->cxsidx`, `env->cxsdata` when `ext_zcx` is set

# qemu-cxtg/target/riscv/cpu.c

RISC-V CPU initialization and ISA extension registration. Manages CPU config, reset, and extension properties.

**Phase 0:** modified — registered `zicx` as an ISA extension and zeroed CX CSRs on CPU reset.

## Extension registration

- [ISA_EXT_DATA_ENTRY(zicx)](cpu.c/ISA_EXT_DATA_ENTRY_zicx.md) — registers zicx in the ISA extension table with `PRIV_VERSION_1_10_0` and `ext_zicx` config field
- [MULTI_EXT_CFG_BOOL(zicx)](cpu.c/MULTI_EXT_CFG_BOOL_zicx.md) — declares zicx as a multi-extension boolean config property (default off)

## CPU reset

- [riscv_cpu_reset (cxsel/cxidx/cxdata zeroing)](cpu.c/riscv_cpu_reset_cx.md) — zeroes `env->cxsel`, `env->cxidx`, `env->cxdata` when `ext_zicx` is set

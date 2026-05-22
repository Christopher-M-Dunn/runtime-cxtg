# qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc

CPU config field macro declarations. Each `BOOL_FIELD(x)` expands to a boolean field in `RISCVCPUConfig`.

**Phase 0:** modified — added `ext_zicx` config field.

## Config fields added

- [ext_zicx](cpu_cfg_fields.h.inc/ext_zicx.md) — boolean flag enabling the Zicx (CX) extension; gates CSR predicates and CPU reset logic

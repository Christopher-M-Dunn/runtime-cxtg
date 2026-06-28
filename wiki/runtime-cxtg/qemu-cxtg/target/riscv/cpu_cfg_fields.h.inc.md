*← [Main Index](../../../../index.md)*

# qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc

CPU config field macro declarations. Each `BOOL_FIELD(x)` expands to a boolean field in `RISCVCPUConfig`.

## Config fields added

- [ext_zcx](cpu_cfg_fields.h.inc/ext_zicx.md) — boolean flag enabling the Zcx extension; gates CSR predicates and CPU reset logic

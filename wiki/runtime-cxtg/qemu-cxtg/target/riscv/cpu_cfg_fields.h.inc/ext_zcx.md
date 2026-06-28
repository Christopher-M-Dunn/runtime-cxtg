*← [Main Index](../../../../../index.md)* | *↑ [cpu_cfg_fields.h.inc](../cpu_cfg_fields.h.inc.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc)*

# target/riscv/cpu_cfg_fields.h.inc — ext_zcx

Boolean flag in `RISCVCPUConfig` that enables the Zcx extension; checked by CSR predicates in `csr.c` and by the CPU reset handler in `cpu.c`.

```c
BOOL_FIELD(ext_zcx)
```
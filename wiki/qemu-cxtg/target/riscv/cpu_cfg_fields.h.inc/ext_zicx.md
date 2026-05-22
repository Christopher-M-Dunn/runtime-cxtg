# target/riscv/cpu_cfg_fields.h.inc — ext_zicx

config field: boolean flag in `RISCVCPUConfig` that enables the Zicx (CX) extension; checked by CSR predicates in `csr.c` and by the CPU reset handler in `cpu.c`

```c
BOOL_FIELD(ext_zicx)
```

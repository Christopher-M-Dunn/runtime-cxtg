# target/riscv/cpu.h — cxdata (CPUArchState field)

struct field: CX state data register state in `CPURISCVState`; read/write window into the active CX's state slot selected by cxidx

```c
/* in CPURISCVState (CPUArchState) */
    target_ulong cxdata;
```

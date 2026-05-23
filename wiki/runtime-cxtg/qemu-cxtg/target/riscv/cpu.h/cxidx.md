# target/riscv/cpu.h — cxidx (CPUArchState field)

struct field: CX state index register state in `CPURISCVState`; selects which slot within the active CX's state array is accessed by cxsdata

```c
/* in CPURISCVState (CPUArchState) */
    target_ulong cxidx;
```

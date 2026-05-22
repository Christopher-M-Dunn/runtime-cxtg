# target/riscv/cpu.h — cxsel (CPUArchState field)

struct field: CX selector register state in `CPURISCVState`; value 0 selects the built-in extension; any other valid value selects a registered CX; invalid values cause CX instructions to trap

```c
/* in CPURISCVState (CPUArchState) */
    target_ulong cxsel;
```

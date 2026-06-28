*← [Main Index](../../../../../index.md)* | *↑ [cpu.h](../cpu.h.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/cpu.h)*

# target/riscv/cpu.h — cxsel (CPUArchState field)

CX selector register state in `CPURISCVState`; value 0 selects the built-in extension; any other valid value selects a registered CX; invalid values cause CX instructions to trap.

```c
typedef struct CPURISCVState {
    ...
    target_ulong cxsel;
    target_ulong cxsidx;
    target_ulong cxsdata;
```

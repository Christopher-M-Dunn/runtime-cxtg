*← [Main Index](../../../../../index.md)* | *↑ [cpu.h](../cpu.h.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/cpu.h)*

# target/riscv/cpu.h — cxsidx (CPUArchState field)

CX state index register in `CPURISCVState`; selects which word of the active CX's state array is accessed by cxsdata.

```c
typedef struct CPURISCVState {
    ...
    target_ulong cxsidx;
```
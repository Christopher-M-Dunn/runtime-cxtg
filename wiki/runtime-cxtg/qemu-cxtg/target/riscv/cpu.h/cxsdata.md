*← [Main Index](../../../../../index.md)* | *↑ [cpu.h](../cpu.h.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/cpu.h)*

# target/riscv/cpu.h — cxsdata (CPUArchState field)

CX state data register in `CPURISCVState`; read/write window into the word of the active CX's state selected by cxsidx.

```c
typedef struct CPURISCVState {
    ...
    target_ulong cxsdata;
```
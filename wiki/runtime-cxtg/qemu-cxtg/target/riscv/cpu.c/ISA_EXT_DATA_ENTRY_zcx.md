*← [Main Index](../../../../../index.md)* | *↑ [cpu.c](../cpu.c.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/cpu.c)*

# target/riscv/cpu.c — ISA_EXT_DATA_ENTRY(zcx)

Registers `zcx` in the ISA extension data table with minimum privilege version `PRIV_VERSION_1_10_0` and config field `ext_zcx`.

```c
static const RISCVIsaExtData isa_edata_arr[] = {
    ...
    ISA_EXT_DATA_ENTRY(zcx, PRIV_VERSION_1_10_0, ext_zcx),
```
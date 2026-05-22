# target/riscv/cpu.c — ISA_EXT_DATA_ENTRY(zicx)

macro call: registers `zicx` in the ISA extension data table with minimum privilege version `PRIV_VERSION_1_10_0` and config field `ext_zicx`

```c
static const RISCVIsaExtData isa_edata_arr[] = {
    ...
    ISA_EXT_DATA_ENTRY(zicx, PRIV_VERSION_1_10_0, ext_zicx),
```

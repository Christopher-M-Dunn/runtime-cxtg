# target/riscv/cpu_bits.h — CXSEL_INV

bitmask: invalid selector flag in the `cxsel` CSR; bit 31; when set, the selector value is invalid and CX instructions trap as illegal

```c
#define CXSEL_INV           0x80000000
```

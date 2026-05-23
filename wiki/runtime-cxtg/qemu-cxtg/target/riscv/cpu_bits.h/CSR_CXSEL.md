# target/riscv/cpu_bits.h — CSR_CXSEL

CSR address for the CX selector register. Placed in the standard user read-only range (bits[11:10]=11, bits[9:8]=00, bits[7:6]=10); read-only enforcement comes from the address encoding alone via `riscv_csrrw_check`. The custom user RO range (0xCC0–0xCFF) is reserved for muxed CX extension registers and cannot be used for CX control registers.

```c
#define CSR_CXSEL           0xCA0
```

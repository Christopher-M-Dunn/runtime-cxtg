# target/riscv/cpu_bits.h — CXSEL field masks

Position, width, and mask constants for the two fields of the cxsel CSR in Direct Mode (Figure 3): `[XLEN-1:16]` reserved | `[15:8]` SID | `[7:0]` CXID.

```c
#define CXSEL_CXID_POSITION 0
#define CXSEL_CXID_BITS     8
#define CXSEL_SID_POSITION  8
#define CXSEL_SID_BITS      8

#define CXSEL_CXID_MASK \
    (((target_ulong)((1U << CXSEL_CXID_BITS) - 1)) << CXSEL_CXID_POSITION)
#define CXSEL_SID_MASK \
    (((target_ulong)((1U << CXSEL_SID_BITS) - 1)) << CXSEL_SID_POSITION)
```

The mask macros use `target_ulong` casts so they sign-extend correctly on both RV32 and RV64 without requiring conditional compilation.
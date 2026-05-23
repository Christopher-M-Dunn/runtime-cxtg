# qemu-cxtg/target/riscv/cpu_bits.h

CSR addresses and bit field definitions for the RISC-V target. The single source of truth for all CSR address constants.

## CSR address constants

- [CSR_CXSEL](cpu_bits.h/CSR_CXSEL.md) — CX selector CSR address (0xCA0, standard URO user range; bits[11:10]=11, [7:6]=10)
- [CSR_CXSIDX](cpu_bits.h/CSR_CXSIDX.md) — CX state index CSR address (0x018)
- [CSR_CXSDATA](cpu_bits.h/CSR_CXSDATA.md) — CX state data CSR address (0x019)

## cxsel field layout (Direct Mode, Figure 3)

`[XLEN-1:16]` reserved | `[15:8]` SID | `[7:0]` CXID

- `CXSEL_CXID_POSITION` / `CXSEL_CXID_BITS` / `CXSEL_CXID_MASK` — CXID field (bits [7:0])
- `CXSEL_SID_POSITION` / `CXSEL_SID_BITS` / `CXSEL_SID_MASK` — SID field (bits [15:8])

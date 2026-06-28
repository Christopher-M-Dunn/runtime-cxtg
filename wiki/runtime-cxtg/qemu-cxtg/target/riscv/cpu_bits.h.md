---
format: B
---

*← [Main Index](../../../../index.md)*

# qemu-cxtg/target/riscv/cpu_bits.h

CSR addresses and bit field definitions for the RISC-V target. The single source of truth for all CSR address constants.

## CSR address constants

- [CSR_CXSEL](cpu_bits.h/CSR_CXSEL.md) — CX selector CSR address (0xCA0, standard URO user range; bits[11:10]=11, [7:6]=10)
- [CSR_CXSIDX](cpu_bits.h/CSR_CXSIDX.md) — CX state index CSR address (0x018)
- [CSR_CXSDATA](cpu_bits.h/CSR_CXSDATA.md) — CX state data CSR address (0x019)

## cxsel field layout

- [CXSEL_CXID_POSITION / CXSEL_CXID_BITS / CXSEL_CXID_MASK / CXSEL_SID_POSITION / CXSEL_SID_BITS / CXSEL_SID_MASK](cpu_bits.h/CXSEL_field_masks.md) — CXID and SID field positions, widths, and masks

## cxsetsel instruction encoding

- [MATCH_CXSETSEL / MASK_CXSETSEL](cpu_bits.h/MATCH_CXSETSEL.md) — binary encoding and mask for the cxsetsel instruction
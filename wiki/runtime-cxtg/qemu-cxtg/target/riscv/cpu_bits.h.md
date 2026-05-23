# qemu-cxtg/target/riscv/cpu_bits.h

CSR addresses and bit field definitions for the RISC-V target. The single source of truth for all CSR address constants.

**Phase 0:** modified — added CSR address constants and field masks for the three CX CSRs.

## CSR address constants

- [CSR_CXSEL](cpu_bits.h/CSR_CXSEL.md) — CX selector CSR address (0x800, stub)
- [CSR_CXSIDX](cpu_bits.h/CSR_CXSIDX.md) — CX state index CSR address (0x801, stub)
- [CSR_CXSDATA](cpu_bits.h/CSR_CXSDATA.md) — CX state data CSR address (0x802, stub)

## cxsel field masks

- [CXSEL_INV](cpu_bits.h/CXSEL_INV.md) — invalid selector bit (bit 31)
- [CXSEL_TYPE](cpu_bits.h/CXSEL_TYPE.md) — extension type field (bits 30:28)
- [CXSEL_SEL](cpu_bits.h/CXSEL_SEL.md) — selector index field (bits 27:0)

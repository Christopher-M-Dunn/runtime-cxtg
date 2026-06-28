*← [Main Index](../../../index.md)*

# qemu-cxtg/disas/riscv.c

QEMU RISC-V disassembler. Translates instruction encodings and CSR numbers to human-readable names.

## CX instruction decoding

- [rv_op_cxsetsel](riscv.c/rv_op_cxsetsel.md) — Opcode enum entry for cxsetsel instruction
- [cxsetsel rvi_opcode_data entry](riscv.c/cxsetsel_opcode_data.md) — Instruction format and codec definition
- [SYSTEM opcode case 4 cxsetsel decode](riscv.c/cxsetsel_decode.md) — Decode guard for cxsetsel SYSTEM variant

## CX CSR disassembler names

- [cxsel 0xCA0](riscv.c/cxsel.md) — CX selector (standard URO user range)
- [cxsidx 0x018](riscv.c/cxsidx.md) — CX state index
- [cxsdata 0x019](riscv.c/cxsdata.md) — CX state data
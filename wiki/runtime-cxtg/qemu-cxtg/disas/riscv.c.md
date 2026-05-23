# qemu-cxtg/disas/riscv.c

QEMU RISC-V disassembler. Translates instruction encodings and CSR numbers to human-readable names.

## CX CSR disassembler names

- `0xCC0` → `"cxsel"` — CX selector (custom URO user range)
- `0x018` → `"cxsidx"` — CX state index
- `0x019` → `"cxsdata"` — CX state data

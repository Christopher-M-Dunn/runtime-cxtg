---
format: B
---

*← [Main Index](../../../../index.md)*

# qemu-cxtg/target/riscv/insn32.decode

Decodetree instruction encoding patterns for the 32-bit RISC-V instruction set. The decodetree tool processes this file at build time and generates a C decoder that dispatches to `trans_*` functions in `translate.c` and its includes.

## Zcx composable extensions

- [cxsetsel](insn32.decode/cxsetsel.md) — decodetree pattern for the cxsetsel instruction; decodes rs1 and rd into arg_r2 and calls trans_cxsetsel
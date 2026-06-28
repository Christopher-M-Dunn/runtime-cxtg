---
format: B
---

*← [Main Index](../../../../index.md)* | *↗ [view source](../../../../../qemu-cxtg/target/riscv/helper.h)*

# qemu-cxtg/target/riscv/helper.h

X-macro file declaring all TCG helper functions for the RISC-V target. Each `DEF_HELPER_*` entry is processed by the QEMU helper macro system to auto-generate both the C prototype and the `gen_helper_*` TCG wrapper used in translation code.

## CX helpers

- [DEF_HELPER_2(cxsetsel, ...)](helper.h/DEF_HELPER_2_cxsetsel.md) — declares the `cxsetsel` TCG helper and its auto-generated wrapper

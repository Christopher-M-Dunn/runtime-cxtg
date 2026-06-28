*← [Main Index](../../../../index.md)*

# qemu-cxtg/target/riscv/translate.c

RISC-V TCG frontend: decodes guest instructions and emits TCG ops. The main entry point is `riscv_tr_translate_insn`; all per-extension translation files are `#include`d here and share a single translation unit.

## Extension includes

- [trans_rvzcx.c.inc include](translate.c/trans_rvzcx_include.md) — includes `insn_trans/trans_rvzcx.c.inc` to add Zcx instruction translation to the RISC-V decoder

# qemu-cxtg/target/riscv/meson.build

RISC-V target Meson build rules. Controls which source files are compiled into the RISC-V system emulator.

**Phase 0:** modified — added `cx.c` to the RISC-V system source set.

## Source files added

- [cx.c source entry](meson.build/cx_c.md) — adds `cx.c` to `riscv_ss` so CX CSR implementations are compiled

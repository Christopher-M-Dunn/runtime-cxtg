# Index

Files added or modified in the Composable Custom Extensions RISC-V Task Group Runtime project.

---

## Format rules

See **[wiki/schema.md](schema.md)** for the authoritative format schema: when to update the wiki, how to choose a format type, file page structure, sub-page structure, and content prohibitions.

Live format examples (read schema.md first, use these as references):
- Format A (small de novo): [tests/common/crt0.S](runtime-cxtg/tests/common/crt0.S.md)
- Format B (pre-existing file, sub-pages): [qemu-cxtg/target/riscv/cpu.c](runtime-cxtg/qemu-cxtg/target/riscv/cpu.c.md)
- Format C (large de novo, anchor index): [Makefile](runtime-cxtg/Makefile.md)
- Format D (self-explanatory): [docs/CXTG_QEMU_Action_Plan.md](runtime-cxtg/docs/CXTG_QEMU_Action_Plan.md.md)

---

## runtime-cxtg

| File | Description |
|------|-------------|
| [CLAUDE.md](runtime-cxtg/CLAUDE.md.md) | Claude Code project guidance |
| [.gitignore](runtime-cxtg/.gitignore.md) | Git ignore rules |
| [.gitmodules](runtime-cxtg/.gitmodules.md) | Submodule configuration |
| [README.md](runtime-cxtg/README.md.md) | Project overview, dependencies, build/run instructions |
| [Makefile](runtime-cxtg/Makefile.md) | Build system: compile test ELFs, run under QEMU |
| [docs/CXTG_QEMU_Action_Plan.md](runtime-cxtg/docs/CXTG_QEMU_Action_Plan.md.md) | Master phased implementation plan |
| [docs/bugs.md](runtime-cxtg/docs/bugs.md.md) | Known bugs and status |
| [docs/Composable-Extensions.md](runtime-cxtg/docs/Composable-Extensions.md.md) | CX requirements (symlink → qemu-cxtg) |
| [docs/flowchart_direct_mode.svg](runtime-cxtg/docs/flowchart_direct_mode.svg.md) | Direct mode state machine diagram |
| [docs/phase0_audit.md](runtime-cxtg/docs/phase0_audit.md.md) | Audit notes: DT schema, CSR inventory, cxsetsel status |
| [docs/progress.md](runtime-cxtg/docs/progress.md.md) | Live block checklist and release notes |
| [docs/spec.md](runtime-cxtg/docs/spec.md.md) | Zicx implementation spec (symlink → qemu-cxtg) |
| [docs/superpowers/plans/2026-05-19-phase0-block0.1-integrate-audit-build.md](runtime-cxtg/docs/superpowers/plans/2026-05-19-phase0-block0.1-integrate-audit-build.md.md) | Phase 0 Block 0.1 implementation plan |
| [docs/todo.md](runtime-cxtg/docs/todo.md.md) | Deferred implementation decisions |
| [include/utils.h](runtime-cxtg/include/utils.h.md) | Inline assembly macros for CX instructions not supported by the standard toolchain assembler |
| [tests/block1_2_cxsetsel.S](runtime-cxtg/tests/block1_2_cxsetsel.S.md) | Assembly test for cxsetsel instruction: five swap behavioral cases |
| [tests/block1_2_cxsetsel_macro.c](runtime-cxtg/tests/block1_2_cxsetsel_macro.c.md) | C test for the CXSETSEL inline-assembly macro: five swap behavioral cases via utils.h |
| [tests/common/crt0.S](runtime-cxtg/tests/common/crt0.S.md) | M-mode startup: stack, mtvec, .bss zero, UART output, test_pass/test_fail |
| [tests/common/uart.h](runtime-cxtg/tests/common/uart.h.md) | QEMU virt UART (0x10000000) output helpers |
| [tests/common/test.h](runtime-cxtg/tests/common/test.h.md) | CHECK macro, _trap_mcause extern, test_pass/test_fail declarations |
| [tests/common/link.ld](runtime-cxtg/tests/common/link.ld.md) | Bare-metal linker script (DRAM base 0x80000000, 16 KiB stack) |

---

## qemu-cxtg

| File                                                                                                                       | Description                                                                    |
| -------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------ |
| [qemu-cxtg/CLAUDE.md](runtime-cxtg/qemu-cxtg/CLAUDE.md.md)                                                                 | Claude Code guidance for the QEMU submodule                                    |
| [qemu-cxtg/Composable-Extensions.md](runtime-cxtg/qemu-cxtg/Composable-Extensions.md.md)                                   | CX extension requirements document                                             |
| [qemu-cxtg/spec.md](runtime-cxtg/qemu-cxtg/spec.md.md)                                                                     | Zicx implementation spec (author: Artur Lojewski)                              |
| [qemu-cxtg/disas/riscv.c](runtime-cxtg/qemu-cxtg/disas/riscv.c.md)                                                         | RISC-V disassembler — instruction decode, CSR name table                       |
| [qemu-cxtg/hw/core/cpu-common.c](runtime-cxtg/qemu-cxtg/hw/core/cpu-common.c.md)                                           | Common CPU utilities                                                           |
| [qemu-cxtg/hw/core/qdev-properties.c](runtime-cxtg/qemu-cxtg/hw/core/qdev-properties.c.md)                                 | QEMU device property system                                                    |
| [qemu-cxtg/linux-headers/asm-riscv/kvm.h](runtime-cxtg/qemu-cxtg/linux-headers/asm-riscv/kvm.h.md)                         | KVM RISC-V ABI header                                                          |
| [qemu-cxtg/system/vl.c](runtime-cxtg/qemu-cxtg/system/vl.c.md)                                                             | QEMU main system emulation entry point                                         |
| [qemu-cxtg/target/riscv/cpu.c](runtime-cxtg/qemu-cxtg/target/riscv/cpu.c.md)                                               | RISC-V CPU initialization and extension registration                           |
| [qemu-cxtg/target/riscv/cpu.h](runtime-cxtg/qemu-cxtg/target/riscv/cpu.h.md)                                               | RISC-V CPU state definition (CPUArchState)                                     |
| [qemu-cxtg/target/riscv/cpu_bits.h](runtime-cxtg/qemu-cxtg/target/riscv/cpu_bits.h.md)                                     | CSR addresses and bit field definitions                                        |
| [qemu-cxtg/target/riscv/insn32.decode](runtime-cxtg/qemu-cxtg/target/riscv/insn32.decode.md)                               | Decodetree instruction encoding patterns for the 32-bit RISC-V instruction set |
| [qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc](runtime-cxtg/qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc.md)                 | CPU config field macro declarations                                            |
| [qemu-cxtg/target/riscv/cpu_helper.c](runtime-cxtg/qemu-cxtg/target/riscv/cpu_helper.c.md)                                 | RISC-V CPU helper functions                                                    |
| [qemu-cxtg/target/riscv/csr.c](runtime-cxtg/qemu-cxtg/target/riscv/csr.c.md)                                               | CSR read/write handlers, predicates, and csr_ops table                         |
| [qemu-cxtg/target/riscv/cx.c](runtime-cxtg/qemu-cxtg/target/riscv/cx.c.md)                                                 | CX extension CSR implementations                                               |
| [qemu-cxtg/target/riscv/cx.h](runtime-cxtg/qemu-cxtg/target/riscv/cx.h.md)                                                 | CX extension CSR interface declarations                                        |
| [qemu-cxtg/target/riscv/helper.h](runtime-cxtg/qemu-cxtg/target/riscv/helper.h.md)                                         | TCG helper function declarations for the RISC-V target                         |
| [qemu-cxtg/target/riscv/kvm/kvm-cpu.c](runtime-cxtg/qemu-cxtg/target/riscv/kvm/kvm-cpu.c.md)                               | KVM RISC-V CPU support                                                         |
| [qemu-cxtg/target/riscv/meson.build](runtime-cxtg/qemu-cxtg/target/riscv/meson.build.md)                                   | RISC-V target Meson build rules                                                |
| [qemu-cxtg/target/riscv/insn_trans/trans_rvzcx.c.inc](runtime-cxtg/qemu-cxtg/target/riscv/insn_trans/trans_rvzcx.c.inc.md) | TCG translation routines for Zcx composable extension instructions             |
| [qemu-cxtg/target/riscv/translate.c](runtime-cxtg/qemu-cxtg/target/riscv/translate.c.md)                                   | RISC-V TCG frontend: instruction decode and TCG op emission                    |
| [qemu-cxtg/target/riscv/trace-events](runtime-cxtg/qemu-cxtg/target/riscv/trace-events.md)                                 | QEMU trace event definitions                                                   |

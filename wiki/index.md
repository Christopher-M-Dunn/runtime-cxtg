# Index

Files added or modified in the Composable Custom Extensions RISC-V Task Group Runtime project.

---

## Format examples

Wiki pages use one of four formats depending on the nature of the file:

**Small de novo file — full text walkthrough**
Every section of the file is documented in order: code block followed by explanatory notes.
Example: [tests/common/crt0.S](runtime-cxtg/tests/common/crt0.S.md)

**Pre-existing file with discrete changes or additions — index to contextual subpages**
File page lists only the symbols/lines the project touched, each linking to a subpage with a code excerpt and context.
Example: [qemu-cxtg/target/riscv/cpu.c](runtime-cxtg/qemu-cxtg/target/riscv/cpu.c.md)

**Large de novo file — anchor-linked index + full text walkthrough on one page**
File page opens with a symbol/section index whose links jump to anchors in the walkthrough below. First index entry is "Jump to full text walkthrough."
Example: [Makefile](runtime-cxtg/Makefile.md)

**Mostly self-explanatory file — customized summary and additional context**
File page provides a high-level description, highlights non-obvious decisions, and adds context not visible from the file itself.
Example: [docs/CXTG_QEMU_Action_Plan.md](runtime-cxtg/docs/CXTG_QEMU_Action_Plan.md)

---

## runtime-cxtg

| File | Description |
|------|-------------|
| [CLAUDE.md](runtime-cxtg/CLAUDE.md) | Claude Code project guidance |
| [.gitignore](runtime-cxtg/.gitignore.md) | Git ignore rules |
| [.gitmodules](runtime-cxtg/.gitmodules.md) | Submodule configuration |
| [README.md](runtime-cxtg/README.md.md) | Project overview, dependencies, build/run instructions |
| [Makefile](runtime-cxtg/Makefile.md) | Build system: compile test ELFs, run under QEMU |
| [docs/CXTG_QEMU_Action_Plan.md](runtime-cxtg/docs/CXTG_QEMU_Action_Plan.md) | Master phased implementation plan |
| [docs/bugs.md](runtime-cxtg/docs/bugs.md) | Known bugs and status |
| [docs/Composable-Extensions.md](runtime-cxtg/docs/Composable-Extensions.md) | CX requirements (symlink → qemu-cxtg) |
| [docs/flowchart_direct_mode.svg](runtime-cxtg/docs/flowchart_direct_mode.svg.md) | Direct mode state machine diagram |
| [docs/phase0_audit.md](runtime-cxtg/docs/phase0_audit.md) | Phase 0 audit: DT schema, CSR inventory, cxsetsel status |
| [docs/progress.md](runtime-cxtg/docs/progress.md) | Live block checklist and release notes |
| [docs/spec.md](runtime-cxtg/docs/spec.md) | Zicx implementation spec (symlink → qemu-cxtg) |
| [docs/superpowers/plans/2026-05-19-phase0-block0.1-integrate-audit-build.md](runtime-cxtg/docs/superpowers/plans/2026-05-19-phase0-block0.1-integrate-audit-build.md) | Phase 0 Block 0.1 implementation plan |
| [docs/todo.md](runtime-cxtg/docs/todo.md) | Deferred decisions and open questions |
| [tests/common/crt0.S](runtime-cxtg/tests/common/crt0.S.md) | M-mode startup: stack, mtvec, .bss zero, UART output, test_pass/test_fail |
| [tests/common/uart.h](runtime-cxtg/tests/common/uart.h.md) | QEMU virt UART (0x10000000) output helpers |
| [tests/common/test.h](runtime-cxtg/tests/common/test.h.md) | CHECK macro, _trap_mcause extern, test_pass/test_fail declarations |
| [tests/common/link.ld](runtime-cxtg/tests/common/link.ld.md) | Bare-metal linker script (DRAM base 0x80000000, 16 KiB stack) |

---

## qemu-cxtg

| File | Description |
|------|-------------|
| [qemu-cxtg/CLAUDE.md](runtime-cxtg/qemu-cxtg/CLAUDE.md) | Claude Code guidance for the QEMU submodule |
| [qemu-cxtg/Composable-Extensions.md](runtime-cxtg/qemu-cxtg/Composable-Extensions.md) | CX extension requirements document |
| [qemu-cxtg/spec.md](runtime-cxtg/qemu-cxtg/spec.md) | Zicx implementation spec (author: Artur Lojewski) |
| [qemu-cxtg/disas/riscv.c](runtime-cxtg/qemu-cxtg/disas/riscv.c.md) | RISC-V disassembler — CSR name table |
| [qemu-cxtg/hw/core/cpu-common.c](runtime-cxtg/qemu-cxtg/hw/core/cpu-common.c.md) | Common CPU utilities |
| [qemu-cxtg/hw/core/qdev-properties.c](runtime-cxtg/qemu-cxtg/hw/core/qdev-properties.c.md) | QEMU device property system |
| [qemu-cxtg/linux-headers/asm-riscv/kvm.h](runtime-cxtg/qemu-cxtg/linux-headers/asm-riscv/kvm.h.md) | KVM RISC-V ABI header |
| [qemu-cxtg/system/vl.c](runtime-cxtg/qemu-cxtg/system/vl.c.md) | QEMU main system emulation entry point |
| [qemu-cxtg/target/riscv/cpu.c](runtime-cxtg/qemu-cxtg/target/riscv/cpu.c.md) | RISC-V CPU initialization and extension registration |
| [qemu-cxtg/target/riscv/cpu.h](runtime-cxtg/qemu-cxtg/target/riscv/cpu.h.md) | RISC-V CPU state definition (CPUArchState) |
| [qemu-cxtg/target/riscv/cpu_bits.h](runtime-cxtg/qemu-cxtg/target/riscv/cpu_bits.h.md) | CSR addresses and bit field definitions |
| [qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc](runtime-cxtg/qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc.md) | CPU config field macro declarations |
| [qemu-cxtg/target/riscv/cpu_helper.c](runtime-cxtg/qemu-cxtg/target/riscv/cpu_helper.c.md) | RISC-V CPU helper functions |
| [qemu-cxtg/target/riscv/csr.c](runtime-cxtg/qemu-cxtg/target/riscv/csr.c.md) | CSR read/write handlers, predicates, and csr_ops table |
| [qemu-cxtg/target/riscv/cx.c](runtime-cxtg/qemu-cxtg/target/riscv/cx.c.md) | CX extension CSR implementations |
| [qemu-cxtg/target/riscv/cx.h](runtime-cxtg/qemu-cxtg/target/riscv/cx.h.md) | CX extension CSR interface declarations |
| [qemu-cxtg/target/riscv/kvm/kvm-cpu.c](runtime-cxtg/qemu-cxtg/target/riscv/kvm/kvm-cpu.c.md) | KVM RISC-V CPU support |
| [qemu-cxtg/target/riscv/meson.build](runtime-cxtg/qemu-cxtg/target/riscv/meson.build.md) | RISC-V target Meson build rules |
| [qemu-cxtg/target/riscv/trace-events](runtime-cxtg/qemu-cxtg/target/riscv/trace-events.md) | QEMU trace event definitions |
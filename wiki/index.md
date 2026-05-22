# Index

Files added or modified in the Composable Custom Extensions RISC-V Task Group Runtime project.

---

## runtime-cxtg

| File | Description |
|------|-------------|
| [CLAUDE.md](CLAUDE.md) | Claude Code project guidance |
| [.gitignore](.gitignore.md) | Git ignore rules |
| [.gitmodules](.gitmodules.md) | Submodule configuration |
| [docs/CXTG_QEMU_Action_Plan.md](docs/CXTG_QEMU_Action_Plan.md) | Master phased implementation plan |
| [docs/bugs.md](docs/bugs.md) | Known bugs and status |
| [docs/Composable-Extensions.md](docs/Composable-Extensions.md) | CX requirements (symlink → qemu-cxtg) |
| [docs/flowchart_direct_mode.svg](docs/flowchart_direct_mode.svg.md) | Direct mode state machine diagram |
| [docs/phase0_audit.md](docs/phase0_audit.md) | Phase 0 audit: DT schema, CSR inventory, cxsetsel status |
| [docs/progress.md](docs/progress.md) | Live block checklist and release notes |
| [docs/spec.md](docs/spec.md) | Zicx implementation spec (symlink → qemu-cxtg) |
| [docs/superpowers/plans/2026-05-19-phase0-block0.1-integrate-audit-build.md](docs/superpowers/plans/2026-05-19-phase0-block0.1-integrate-audit-build.md) | Phase 0 Block 0.1 implementation plan |
| [docs/todo.md](docs/todo.md) | Deferred decisions and open questions |

---

## qemu-cxtg

| File | Description |
|------|-------------|
| [qemu-cxtg/CLAUDE.md](qemu-cxtg/CLAUDE.md) | Claude Code guidance for the QEMU submodule |
| [qemu-cxtg/Composable-Extensions.md](qemu-cxtg/Composable-Extensions.md) | CX extension requirements document |
| [qemu-cxtg/spec.md](qemu-cxtg/spec.md) | Zicx implementation spec (author: Artur Lojewski) |
| [qemu-cxtg/disas/riscv.c](qemu-cxtg/disas/riscv.c.md) | RISC-V disassembler — CSR name table |
| [qemu-cxtg/hw/core/cpu-common.c](qemu-cxtg/hw/core/cpu-common.c.md) | Common CPU utilities |
| [qemu-cxtg/hw/core/qdev-properties.c](qemu-cxtg/hw/core/qdev-properties.c.md) | QEMU device property system |
| [qemu-cxtg/linux-headers/asm-riscv/kvm.h](qemu-cxtg/linux-headers/asm-riscv/kvm.h.md) | KVM RISC-V ABI header |
| [qemu-cxtg/system/vl.c](qemu-cxtg/system/vl.c.md) | QEMU main system emulation entry point |
| [qemu-cxtg/target/riscv/cpu.c](qemu-cxtg/target/riscv/cpu.c.md) | RISC-V CPU initialization and extension registration |
| [qemu-cxtg/target/riscv/cpu.h](qemu-cxtg/target/riscv/cpu.h.md) | RISC-V CPU state definition (CPUArchState) |
| [qemu-cxtg/target/riscv/cpu_bits.h](qemu-cxtg/target/riscv/cpu_bits.h.md) | CSR addresses and bit field definitions |
| [qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc](qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc.md) | CPU config field macro declarations |
| [qemu-cxtg/target/riscv/cpu_helper.c](qemu-cxtg/target/riscv/cpu_helper.c.md) | RISC-V CPU helper functions |
| [qemu-cxtg/target/riscv/csr.c](qemu-cxtg/target/riscv/csr.c.md) | CSR read/write handlers, predicates, and csr_ops table |
| [qemu-cxtg/target/riscv/cx.c](qemu-cxtg/target/riscv/cx.c.md) | CX extension CSR implementations |
| [qemu-cxtg/target/riscv/cx.h](qemu-cxtg/target/riscv/cx.h.md) | CX extension CSR interface declarations |
| [qemu-cxtg/target/riscv/kvm/kvm-cpu.c](qemu-cxtg/target/riscv/kvm/kvm-cpu.c.md) | KVM RISC-V CPU support |
| [qemu-cxtg/target/riscv/meson.build](qemu-cxtg/target/riscv/meson.build.md) | RISC-V target Meson build rules |
| [qemu-cxtg/target/riscv/trace-events](qemu-cxtg/target/riscv/trace-events.md) | QEMU trace event definitions |
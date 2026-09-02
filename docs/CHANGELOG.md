# CXTG QEMU Implementation — Changelog

Prepend-only log of everything done in each phase, newest first. See [`progress.md`](progress.md) for the live block-by-block checklist, and [`CXTG_QEMU_Action_Plan.md`](CXTG_QEMU_Action_Plan.md) for full block specs.

---

## v1.0 — *(pending)*

## v0.phase7 — *(pending)*

## v0.phase6 — *(pending)*

## v0.phase5 — *(pending)*

## v0.phase4 — *(pending)*

## v0.phase3 — *(pending)*

## v0.phase2 — *(pending)*

## v0.phase1 — *(in progress)*

**qemu-cxtg — feat/1.1:**
- modified: `target/riscv/cpu.c` — renamed `ext_zicx` → `ext_zcx` in `ISA_EXT_DATA_ENTRY`, `MULTI_EXT_CFG_BOOL`, and reset handler (`env->cxidx`/`cxdata` → `env->cxsidx`/`cxsdata`)
- modified: `target/riscv/cpu_cfg_fields.h.inc` — renamed `BOOL_FIELD(ext_zicx)` → `BOOL_FIELD(ext_zcx)`
- modified: `target/riscv/cpu.h` — renamed `CPURISCVState` fields `cxidx`/`cxdata` → `cxsidx`/`cxsdata`
- modified: `target/riscv/cpu_bits.h` — updated `CSR_CXSEL` 0x800→0xCA0 (standard URO user range; custom ranges 0x800–0x8FF and 0xCC0–0xCFF are mux space for CX extension registers), `CSR_CXSIDX` 0x801→0x018, `CSR_CXSDATA` 0x802→0x019; removed `CXSEL_INV`/`CXSEL_TYPE`/`CXSEL_SEL`; added `CXSEL_CXID_POSITION`/`BITS`, `CXSEL_SID_POSITION`/`BITS`, `CXSEL_CXID_MASK`, `CXSEL_SID_MASK`
- modified: `target/riscv/csr.c` — predicates `cxsel`/`cxsidx`/`cxsdata`: removed dead PMP copy-paste; condition flipped to `!ext_zcx`; cxsel read-only trap comes from `riscv_csrrw_check` line 5567 (`get_field(csrno, 0xC00) == 3` — CSR address bits [11:10]=11 for 0xCA0), not from the write handler; replaced `read_cxsdata`/`write_cxsdata` wrappers with `op_cxsdata` registered as `.op`
- modified: `target/riscv/cx.c` — renamed `env->cxidx`→`env->cxsidx`, `env->cxdata`→`env->cxsdata`; replaced separate `cxsdata_csr_read`/`cxsdata_csr_write` with single `cxsdata_csr_op` registered as `.op`; `op` uses `write_mask` to handle all CSR variants (`write_mask=0` read-only, `write_mask=-1` full replace, partial mask for csrrs/csrrc); `env->cxsidx++` fires exactly once per instruction regardless of variant
- modified: `target/riscv/cx.h` — removed `cxsdata_csr_read`/`cxsdata_csr_write` prototypes; added `cxsdata_csr_op` with `.op` signature `(env, csrno, *ret_value, new_value, write_mask)`
- modified: `target/riscv/kvm/kvm-cpu.c` — renamed `KVM_EXT_CFG("zicx"→"zcx")`, `KVM_RISCV_ISA_EXT_ZICX`→`ZCX`
- modified: `linux-headers/asm-riscv/kvm.h` — renamed `KVM_RISCV_ISA_EXT_ZICX`→`KVM_RISCV_ISA_EXT_ZCX`
- modified: `disas/riscv.c` — updated CSR addresses 0x800→0xCA0, 0x801→0x018, 0x802→0x019; names `cxidx`→`cxsidx`, `cxdata`→`cxsdata`; `//` comments → `/* */`

**runtime-cxtg — feat/1.1:**
- modified: `Makefile` — added `QEMU_CPU ?= rv64,zcx=on` variable; added `-cpu $(QEMU_CPU)` to `QEMU_FLAGS`; added `-mcmodel=medany` to `CFLAGS` (required for RV64 bare-metal at 0x80000000 — medlow can't address bit-31-set addresses via `lui`)
- created: `tests/block1_1_csr_stubs.c` — Block 1.1 test: cxsel reads 0 at reset; write to cxsel traps (mcause=2, URO); cxsidx round-trip; cxsdata all four CSR variants (csrr/csrw/csrrw/csrrs/csrrc) with auto-increment verification; CSR access via `_XSTR` stringification macros (avoids GCC `"i"` constraint mismatch for RISC-V CSR inline asm)
- created: `tests/output/block1_1_csr_stubs.out` — captured trace output showing all six CX trace events; cxsel at reg 3232 (0xCA0); cxsidx incrementing 1→9 across ten cxsdata accesses; PASSED

**runtime-cxtg — feat/test-infra:**
- created: `README.md` — project overview, dependencies (`riscv64-unknown-elf-gcc`, `qemu-cxtg` build), `make` targets, test infrastructure table, repository layout
- created: `Makefile` — `CROSS_COMPILE`/`CC`/`QEMU` variables; pattern rules `tests/%.elf: tests/%.S` and `tests/%.elf: tests/%.c` (both link `crt0.S`, pass `-T link.ld`); `run-%` target (`-bios`); `all-tests` wildcard; `clean`
- created: `tests/common/crt0.S` — `_start` (sets sp, installs mtvec, zeros .bss, calls main); `_trap_handler` (records `mcause` to `_trap_mcause`, advances mepc+4, mrets); `_puts` (internal UART write); `test_pass`/`test_fail` (global, print PASSED/FAILED, spin); `_trap_mcause` (.bss dword)
- created: `tests/common/uart.h` — `UART_BASE` (0x10000000); `uart_putchar`, `uart_puts`, `uart_print_hex`
- created: `tests/common/test.h` — `CHECK(cond)` macro; `MCAUSE_ILLEGAL_INST` (2UL); `extern volatile unsigned long _trap_mcause`; `test_pass`/`test_fail` declarations
- created: `tests/common/link.ld` — DRAM origin 0x80000000/128 MiB; `.text.start` first; `_bss_start`/`_bss_end` (8-byte aligned); 16 KiB stack (`_stack_bottom`/`_stack_top`)
- modified: `CLAUDE.md` — wiki section: 4 formats with examples, read-examples-first rule, `wiki/runtime-cxtg/` path; release notes: incremental write-on-merge policy, milestone --stat verification
- modified: `docs/progress.md` — checked test-infra block; Phase 1 impl plan marked written
- modified: `docs/superpowers/specs/2026-05-22-phase1-csr-instruction-scaffolding-design.md` — wiki section 4: read-examples-first, `wiki/runtime-cxtg/` paths
- modified: `docs/superpowers/plans/2026-05-22-phase1-csr-instruction-scaffolding.md` — Task 5: format-first wiki instruction, corrected paths throughout
- modified: `wiki/index.md` — 4-format examples section; all links updated to `runtime-cxtg/` prefix
- created: `wiki/runtime-cxtg/README.md.md`, `Makefile.md`, `tests/common/crt0.S.md`, `tests/common/uart.h.md`, `tests/common/test.h.md`, `tests/common/link.ld.md`
- reorganized: wiki pages moved from `wiki/` flat into `wiki/runtime-cxtg/` subtree; orphaned subpage stubs deleted

**qemu-cxtg — feat/1.2:**
- modified: `target/riscv/cpu_bits.h` — added `MATCH_CXSETSEL` (0x00004073U) and `MASK_CXSETSEL` (0xFFF0707FU)
- modified: `target/riscv/trace-events` — added `cxsetsel` trace event with fields `mhartid`, `new_val`, `old_val`
- modified: `target/riscv/insn32.decode` — added `cxsetsel` decode pattern under `@r2` format in the SYSTEM opcode group
- modified: `target/riscv/helper.h` — added `DEF_HELPER_2(cxsetsel, tl, env, tl)`
- modified: `target/riscv/cx.h` — removed `helper_cxsetsel` prototype (redundant with `DEF_HELPER_2` auto-generation; caused `-Werror=redundant-decls`)
- modified: `target/riscv/cx.c` — added `#include "exec/helper-proto.h"`; added `helper_cxsetsel` body: reads `env->cxsel`, writes `new_val`, emits `trace_cxsetsel`, returns old value
- created: `target/riscv/insn_trans/trans_rvzcx.c.inc` — `trans_cxsetsel`: guards on `ctx->cfg_ptr->ext_zcx`; calls `gen_helper_cxsetsel`; returns `do_csr_post(ctx)`
- modified: `target/riscv/translate.c` — added `#include "insn_trans/trans_rvzcx.c.inc"` after `trans_rvzimop.c.inc`
- modified: `disas/riscv.c` — added `rv_op_cxsetsel = 956` to enum; added `{ "cxsetsel", rv_codec_r, rv_fmt_rd_rs1, NULL, 0, 0, 0 }` to `rvi_opcode_data[]`; added decode guard in `case 4` of SYSTEM opcode block (`ext_zcx && (inst >> 20) == 0`)

**runtime-cxtg — feat/1.2:**
- modified: `Makefile` — split `QEMU_FLAGS` into `QEMU_BASE` (always-on) and `QEMU_FLAGS` (user-overridable) to allow `QEMU_FLAGS="..."` appending
- created: `include/utils.h` — `CXSETSEL(rd, rs1)` inline-asm macro using `.insn r 0x73, 4, 0, %0, %1, x0`
- created: `tests/block1_2_cxsetsel.S` — assembly test: 5 behavioral cases for `cxsetsel` (swap from 0, swap non-zero, swap ~0, rd=x0 discard, round-trip)
- created: `tests/block1_2_cxsetsel_macro.c` — C test: same 5 cases via `CXSETSEL` macro from `utils.h`
- created: `tests/output/block1_2_cxsetsel.out` — captured PASSED output for assembly test
- created: `tests/output/block1_2_cxsetsel_macro.out` — captured PASSED output for C macro test

## v0.phase0 — Baseline (Artur's CX branch integrated)

**Root repo — files created:**
- created: `.gitignore`
- created: `.gitmodules` — added `qemu-cxtg` and `linux-cxtg` as submodules
- created: `docs/CXTG_QEMU_Action_Plan.md`
- created: `docs/bugs.md`
- created: `docs/flowchart_direct_mode.svg`
- created: `docs/phase0_audit.md`
- created: `docs/progress.md`
- created: `docs/superpowers/plans/2026-05-19-phase0-block0.1-integrate-audit-build.md`
- created: `docs/todo.md`
  - deferred: build flag architecture — only `ext_zcx` and `ext_zcxmulti` runtime flags needed; no `CONFIG_*` compile-time symbols required
  - deferred: `CSR_CXSEL` address set to `0x800` (stub); spec draft says `0xCC0` — confirm with spec owners
  - deferred: `CSR_CXSIDX` (`0x801`) and `CSR_CXSDATA` (`0x802`) addresses pending spec finalisation
  - deferred: extension name `zicx` change to `zcx`
  - deferred: `cxsetsel` instruction encoding TBD — SYSTEM opcode (`0x73`), funct3 slot unassigned

**qemu-cxtg — `cxtg-dev` vs `cxtg` (+1075 lines, 19 files):**
- created: `CLAUDE.md` — added QEMU build system instructions, RISC-V target overview, and CX extension development guidance for Claude Code
- created: `Composable-Extensions.md` — documented CX extension requirements: cxsel (URO), cxsetsel instruction, cxsidx/cxsdata CSRs, and atomics semantics
- created: `spec.md` — added full Zicx implementation spec: CSR layout, instruction encoding, privilege model, and QEMU integration notes (author: Artur Lojewski)
- modified: `disas/riscv.c` — added disassembler name entries for cxsel (0x800), cxidx (0x801), cxdata (0x802)
- modified: `hw/core/cpu-common.c` — added debug printf in `cpu_common_parse_features` logging feature string at parse time
- modified: `hw/core/qdev-properties.c` — added debug printf in `qdev_prop_check_globals` logging driver/property/value for each global prop
- modified: `linux-headers/asm-riscv/kvm.h` — added `KVM_RISCV_ISA_EXT_ZICX` to the KVM ISA extension enum
- modified: `system/vl.c` — added debug printf in `QEMU_OPTION_cpu` case logging `-cpu` option value at parse time
- modified: `target/riscv/cpu.c` — registered `zicx` as ISA extension (`ISA_EXT_DATA_ENTRY`, `MULTI_EXT_CFG_BOOL`, default off); zeroed `cxsel`/`cxidx`/`cxdata` on CPU reset when `ext_zicx` set
- modified: `target/riscv/cpu.h` — included `cx.h`; added `cxsel`, `cxidx`, `cxdata` fields to `CPUArchState`
- modified: `target/riscv/cpu_bits.h` — defined `CSR_CXSEL` (0x800), `CSR_CXSIDX` (0x801), `CSR_CXSDATA` (0x802); defined `CXSEL_INV`, `CXSEL_TYPE`, `CXSEL_SEL` field masks
- modified: `target/riscv/cpu_cfg_fields.h.inc` — added `ext_zicx` `BOOL_FIELD` to CPU config fields
- modified: `target/riscv/cpu_helper.c` — added `#include "cx.h"`
- modified: `target/riscv/csr.c` — implemented read/write handlers and `ext_zicx`-gated predicates for `CSR_CXSEL`, `CSR_CXSIDX`, `CSR_CXSDATA`; registered in `csr_ops`; `write_cxsel` returns `ILLEGAL_INST` (cxsel is read-only per spec)
- created: `target/riscv/cx.c` — implemented `cxsel_csr_read/write`, `cxsidx_csr_read/write`, `cxsdata_csr_read/write` with trace events; `cxsel` write is a no-op stub (read-only)
- created: `target/riscv/cx.h` — declared read/write prototypes for cxsel, cxsidx, cxsdata CSR handlers
- modified: `target/riscv/kvm/kvm-cpu.c` — added `zicx` entry to `kvm_multi_ext_cfgs` (`KVM_RISCV_ISA_EXT_ZICX`); added debug printf in `kvm_riscv_update_cpu_cfg_isa_ext` and `kvm_riscv_add_cpu_user_properties`
- modified: `target/riscv/meson.build` — added `cx.c` to the RISC-V system source set
- modified: `target/riscv/trace-events` — added trace event definitions for `cxsel`, `cxsidx`, `cxsdata` CSR read and write operations

**qemu-cxtg — workflow documentation:**
- modified: `CLAUDE.md` — replaced "Current Branch: feature/cx" with stable "CX Extension Workflow" section (branch model, per-block workflow, parent repo tagging step)
- modified: `spec.md` — updated `## Commit History` and `## Contact` (maintainer, branch, version)

**Root repo — workflow documentation:**
- created: `CLAUDE.md`, `docs/spec.md` (symlink), `docs/Composable-Extensions.md` (symlink)
- modified: `docs/CXTG_QEMU_Action_Plan.md` — added step 7 (parent repo milestone sync); docs-only exception to workflow rule
- modified: `docs/progress.md` — added `implementation plan written` to all phases
- modified: `docs/todo.md` — `zicx` → `zcx` rename deferred to Phase 1

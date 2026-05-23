# Phase 1 — Unpriv CSR and Instruction Scaffolding Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement the Zcx CSR stubs (cxsel, cxsidx, cxsdata) with correct addresses and behavior, the cxsetsel instruction with clamping, and a bare-metal test harness plus C runtime wrappers.

**Architecture:** Four sequential branches (feat/test-infra → feat/1.1 → feat/1.2 → feat/1.3), each merging to `cxtg-dev` before the next begins. QEMU work lives in `qemu-cxtg/`; harness and runtime wrappers live in `runtime-cxtg/`. Tests are bare-metal RV64 ELFs run under QEMU with `-bios`.

**Tech Stack:** QEMU RISC-V target (C, TCG, decodetree), riscv64-unknown-elf-gcc, bare-metal RISC-V assembly, QEMU virt machine.

**Design doc:** `docs/superpowers/specs/2026-05-22-phase1-csr-instruction-scaffolding-design.md`

---

## Navigation

Jump directly to your branch:

| Branch | Repo | Start at |
|--------|------|----------|
| `feat/test-infra` | runtime-cxtg | [Task 1 — linker script](#task-1-create-test-harness-structure-and-linker-script) |
| `feat/1.1` | qemu-cxtg | [Task 6 — cut branch](#task-6-cut-branch-and-confirm-baseline-build) |
| `feat/1.2` | qemu-cxtg + runtime-cxtg | [Task 12 — cut branch](#task-12-cut-branch-and-add-encoding-constants) |
| `feat/1.3` | runtime-cxtg | [Task 17 — cx library](#task-17-create-runtime-c-library-headers-and-implementation) |
| Phase milestone | both | [Task 20 — tag](#task-20-merge-and-tag-phase-milestone) |

**Note on two `cx` files:** `qemu-cxtg/target/riscv/cx.c` is the QEMU-side CSR implementation. The runtime library is `runtime-cxtg/src/ccex.c` / `include/ccex.h` (named `ccex` to avoid confusion).

---

## File Map

### feat/test-infra (runtime-cxtg)

| Action | Path |
|--------|------|
| Create | `tests/common/link.ld` |
| Create | `tests/common/crt0.S` |
| Create | `tests/common/uart.h` |
| Create | `tests/common/test.h` |
| Create | `tests/smoke.S` |
| Create | `Makefile` |
| Create | `README.md` |

### feat/1.1 (qemu-cxtg)

| Action | Path |
|--------|------|
| Modify | `target/riscv/cpu_bits.h` |
| Modify | `target/riscv/cpu.h` |
| Modify | `target/riscv/cpu.c` |
| Modify | `target/riscv/cpu_cfg_fields.h.inc` |
| Modify | `target/riscv/csr.c` |
| Modify | `target/riscv/cx.c` |
| Modify | `target/riscv/kvm/kvm-cpu.c` |
| Modify | `linux-headers/asm-riscv/kvm.h` |
| Modify | `disas/riscv.c` |
| Create | `tests/block1_1_csr_stubs.S` (in runtime-cxtg) |

### feat/1.2 (qemu-cxtg + runtime-cxtg)

| Action | Path |
|--------|------|
| Modify | `target/riscv/cpu_bits.h` (qemu-cxtg) |
| Modify | `target/riscv/insn32.decode` (qemu-cxtg) |
| Create | `target/riscv/insn_trans/trans_zcx.c.inc` (qemu-cxtg) |
| Modify | `target/riscv/translate.c` (qemu-cxtg) |
| Create | `include/utils.h` (runtime-cxtg) |
| Create | `tests/block1_2_cxsetsel.S` (runtime-cxtg) |

### feat/1.3 (runtime-cxtg)

| Action | Path |
|--------|------|
| Create | `include/ccex.h` |
| Create | `src/ccex.c` |
| Create | `examples/test_wrappers.c` |
| Modify | `Makefile` |

---

## Branch: feat/test-infra

Work in: `runtime-cxtg`

### Task 1: Create test harness structure and linker script

**Files:**
- Create: `tests/common/link.ld`

- [ ] **Step 1: Create directories**

```bash
cd /home/cmd/Github/runtime-cxtg
mkdir -p tests/common examples src include
```

- [ ] **Step 2: Write linker script**

Create `tests/common/link.ld`:

```ld
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY {
    ram (rwx) : ORIGIN = 0x80000000, LENGTH = 4M
}

SECTIONS {
    .text : {
        *(.text.start)
        *(.text .text.*)
    } > ram

    .rodata : {
        *(.rodata .rodata.*)
    } > ram

    .data : {
        *(.data .data.*)
    } > ram

    .bss : {
        _bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        _bss_end = .;
    } > ram

    _stack_top = ORIGIN(ram) + LENGTH(ram);
}
```

- [ ] **Step 3: Commit**

```bash
git add tests/common/link.ld
git commit -m "feat(test-infra): add bare-metal linker script"
```

---

### Task 2: Write startup code (crt0.S)

**Files:**
- Create: `tests/common/crt0.S`

- [ ] **Step 1: Write crt0.S**

Create `tests/common/crt0.S`:

```asm
/*
 * Bare-metal RISC-V M-mode startup for CX extension tests.
 * Linked via tests/common/link.ld; entry point _start at 0x80000000.
 *
 * Provides:
 *   trap_mcause  — global word set by trap handler on each trap
 *   test_pass    — print "PASSED\n" over UART and spin
 *   test_fail    — a0=message, print "FAILED: <msg>\n" and spin
 */

#define UART_BASE 0x10000000

    .section .text.start
    .globl _start
_start:
    la    sp, _stack_top
    la    t0, _trap_handler
    csrw  mtvec, t0
    la    t0, _bss_start
    la    t1, _bss_end
.Lbss_loop:
    beq   t0, t1, .Lbss_done
    sd    zero, 0(t0)
    addi  t0, t0, 8
    j     .Lbss_loop
.Lbss_done:
    call  main
.Lhang:
    j     .Lhang

/*
 * Trap handler: records mcause, advances mepc past the faulting 4-byte
 * instruction, and mrets.  Only correct for 4-byte (non-compressed) insns —
 * tests must not use the C extension.  Clear trap_mcause to 0 before any
 * operation that should NOT trap; check it afterwards.
 */
    .align 4
_trap_handler:
    csrr  t0, mcause
    la    t1, trap_mcause
    sd    t0, 0(t1)
    csrr  t0, mepc
    addi  t0, t0, 4
    csrw  mepc, t0
    mret

/* test_pass: print "PASSED\n" then spin */
    .globl test_pass
test_pass:
    la    a0, .Lstr_passed
    call  .Lputs
.Lpass_spin:
    j     .Lpass_spin

/*
 * test_fail: a0 = NUL-terminated message
 * Prints "FAILED: <a0>\n" then spins.
 */
    .globl test_fail
test_fail:
    mv    s0, a0
    la    a0, .Lstr_failed
    call  .Lputs
    mv    a0, s0
    call  .Lputs
    la    a0, .Lstr_newline
    call  .Lputs
.Lfail_spin:
    j     .Lfail_spin

/* .Lputs: a0 = string; writes bytes to UART until NUL */
.Lputs:
    li    t0, UART_BASE
.Lputs_loop:
    lb    t1, 0(a0)
    beqz  t1, .Lputs_ret
    sb    t1, 0(t0)
    addi  a0, a0, 1
    j     .Lputs_loop
.Lputs_ret:
    ret

    .section .rodata
.Lstr_passed:  .string "PASSED\n"
.Lstr_failed:  .string "FAILED: "
.Lstr_newline: .string "\n"

    .section .bss
    .globl trap_mcause
    .align 8
trap_mcause: .quad 0
```

- [ ] **Step 2: Commit**

```bash
git add tests/common/crt0.S
git commit -m "feat(test-infra): add M-mode startup and trap handler"
```

---

### Task 3: Write C helper headers

**Files:**
- Create: `tests/common/uart.h`
- Create: `tests/common/test.h`

- [ ] **Step 1: Write uart.h**

Create `tests/common/uart.h`:

```c
#ifndef UART_H
#define UART_H

#include <stdint.h>

/* QEMU virt machine 16550 UART — single-byte write to TX register suffices */
#define UART_TX ((volatile uint8_t *)0x10000000UL)

static inline void uart_putchar(char c)
{
    *UART_TX = (uint8_t)c;
}

static inline void uart_puts(const char *s)
{
    while (*s) {
        uart_putchar(*s++);
    }
}

static inline void uart_print_hex(uintptr_t val)
{
    static const char hex[] = "0123456789abcdef";
    int i;
    uart_puts("0x");
    for (i = (int)(sizeof(uintptr_t) * 2) - 1; i >= 0; i--) {
        uart_putchar(hex[(val >> (i * 4)) & 0xfU]);
    }
}

#endif /* UART_H */
```

- [ ] **Step 2: Write test.h**

Create `tests/common/test.h`:

```c
#ifndef TEST_H
#define TEST_H

#include "uart.h"
#include <stdint.h>

/* Defined in crt0.S — set by trap handler on each trap, reset to 0 by tests */
extern volatile unsigned long trap_mcause;

/* Defined in crt0.S */
void test_pass(void);
void test_fail(const char *msg);

/*
 * CHECK_EQ: assert a == b; call test_fail(msg) if not.
 * Both sides are cast to unsigned long to handle target_ulong comparisons.
 */
#define CHECK_EQ(a, b, msg) \
    do { \
        if ((unsigned long)(a) != (unsigned long)(b)) { \
            test_fail(msg); \
        } \
    } while (0)

#endif /* TEST_H */
```

- [ ] **Step 3: Commit**

```bash
git add tests/common/uart.h tests/common/test.h
git commit -m "feat(test-infra): add C headers for UART and test assertions"
```

---

### Task 4: Write smoke test and Makefile

**Files:**
- Create: `tests/smoke.S`
- Create: `Makefile`

- [ ] **Step 1: Write smoke test**

Create `tests/smoke.S`:

```asm
/*
 * Smoke test: verifies that the test harness itself works — UART output and
 * trap_mcause recording.  Does not use any CX CSRs.
 */

    .section .text
    .globl main
main:
    /* Verify trap_mcause starts at 0 */
    la    t0, trap_mcause
    ld    t1, 0(t0)
    bnez  t1, .Lfail_mcause_nonzero

    /* Trigger a trap: write to mscratch from U-mode would trap, but we are
     * in M-mode. Instead, execute an explicitly illegal encoding: 0x00000000
     * is reserved and always traps as illegal instruction (mcause=2). */
    la    t0, trap_mcause
    sd    zero, 0(t0)           /* clear sentinel */
    .word 0x00000000            /* illegal instruction — mcause=2 */
    ld    t1, 0(t0)
    li    t2, 2
    bne   t1, t2, .Lfail_trap

    call  test_pass

.Lfail_mcause_nonzero:
    la    a0, .Lmsg_mcause
    call  test_fail

.Lfail_trap:
    la    a0, .Lmsg_trap
    call  test_fail

    .section .rodata
.Lmsg_mcause: .string "trap_mcause not zero at reset"
.Lmsg_trap:   .string "illegal insn did not set mcause=2"
```

- [ ] **Step 2: Write Makefile**

Create `Makefile`:

```makefile
# runtime-cxtg top-level Makefile
#
# Dependency: riscv64-unknown-elf-gcc must be on PATH.
# QEMU binary built from qemu-cxtg/build/ (see README.md).

CROSS_COMPILE ?= riscv64-unknown-elf-
CC  = $(CROSS_COMPILE)gcc
QEMU ?= qemu-cxtg/build/qemu-system-riscv64

ARCH_FLAGS = -march=rv64ima -mabi=lp64 -mcmodel=medany
CFLAGS     = $(ARCH_FLAGS) -nostdlib -nostartfiles -ffreestanding -g -O0 \
             -Itests/common
LDFLAGS    = $(ARCH_FLAGS) -nostdlib -T tests/common/link.ld

COMMON_OBJS = tests/common/crt0.S

QEMU_BASE_FLAGS = -M virt -nographic -no-reboot

# --- Test ELF targets ---

tests/smoke.elf: tests/smoke.S $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

tests/block1_1_csr_stubs.elf: tests/block1_1_csr_stubs.S $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

tests/block1_2_cxsetsel.elf: tests/block1_2_cxsetsel.S $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# --- Run targets ---

run-smoke: tests/smoke.elf
	$(QEMU) $(QEMU_BASE_FLAGS) -bios $<

run-block1_1: tests/block1_1_csr_stubs.elf
	$(QEMU) $(QEMU_BASE_FLAGS) -cpu rv64,zcx=on -bios $<

run-block1_2: tests/block1_2_cxsetsel.elf
	$(QEMU) $(QEMU_BASE_FLAGS) -cpu rv64,zcx=on -bios $<

# --- Aggregate ---

all-tests: tests/smoke.elf tests/block1_1_csr_stubs.elf \
           tests/block1_2_cxsetsel.elf

.PHONY: run-smoke run-block1_1 run-block1_2 all-tests clean

clean:
	rm -f tests/*.elf
```

- [ ] **Step 3: Write README.md**

Create `README.md`:

```markdown
# runtime-cxtg

Umbrella project for the RISC-V Composable Extensions (Zcx/ZcxMulti) implementation.

## Repository layout

| Path | Purpose |
|------|---------|
| `qemu-cxtg/` | QEMU fork implementing the Zcx/ZcxMulti hardware model (git submodule) |
| `linux-cxtg/` | Linux kernel for S-mode testing — future work (git submodule) |
| `include/` | C headers for the CX runtime library |
| `src/` | C source for the CX runtime library |
| `examples/` | Example programs using the CX runtime |
| `tests/` | Bare-metal RISC-V ELF tests |
| `tests/common/` | Shared test harness (startup, UART, assertions) |
| `docs/` | Project documentation |
| `wiki/` | Symbol-level reference wiki |

## Dependencies

- **riscv64-unknown-elf-gcc** — RISC-V bare-metal cross-compiler
  Install: `sudo apt install gcc-riscv64-unknown-elf` (Ubuntu 22.04+)
  Tested with Ubuntu package version 13.3.
- **QEMU** — built from `qemu-cxtg/` submodule; see `qemu-cxtg/CLAUDE.md`

## Building tests

```bash
# Build QEMU first (once):
cd qemu-cxtg
mkdir -p build && cd build
../configure --target-list=riscv64-softmmu --enable-debug
make -j$(nproc)
cd ../..

# Build all test ELFs:
make all-tests

# Run smoke test (no CX CSRs required):
make run-smoke

# Run Block 1.1 CSR stub test (requires zcx=on):
make run-block1_1

# Run Block 1.2 cxsetsel test (requires zcx=on):
make run-block1_2
```

Each test prints `PASSED` or `FAILED: <message>` to the terminal and exits.
```

- [ ] **Step 4: Build and run smoke test**

```bash
make tests/smoke.elf
```

Expected: no errors, `tests/smoke.elf` created.

```bash
make run-smoke
```

Expected output (last line before QEMU exit):
```
PASSED
```

If you see `FAILED:` or QEMU hangs without output, check: UART_BASE in crt0.S is 0x10000000, `-nographic` is present, `-bios` is used (not `-kernel`).

- [ ] **Step 5: Commit**

```bash
git add tests/smoke.S Makefile README.md
git commit -m "feat(test-infra): add smoke test and Makefile"
```

---

### Task 5: Update wiki and merge

**Files:**
- Modify: `wiki/index.md`
- Create: `wiki/runtime-cxtg/README.md.md`, `wiki/runtime-cxtg/Makefile.md`, `wiki/runtime-cxtg/tests/common/crt0.S.md`, `wiki/runtime-cxtg/tests/common/uart.h.md`, `wiki/runtime-cxtg/tests/common/test.h.md`, `wiki/runtime-cxtg/tests/common/link.ld.md`

- [ ] **Step 1: Create wiki file pages**

Read the four format examples at the top of `wiki/index.md` and open each linked example before writing anything. Create one page per file at `wiki/runtime-cxtg/<path>.md`, choosing the format that fits:
- `crt0.S`, `uart.h`, `test.h`, `link.ld` — small de novo → full text walkthrough
- `Makefile` — large de novo → anchor-linked index + walkthrough on one page
- `README.md` — self-explanatory → summary

- [ ] **Step 2: Update wiki/index.md**

Add a row for each new file in the `## runtime-cxtg` table.

- [ ] **Step 3: Commit wiki**

```bash
git add wiki/
git commit -m "docs(wiki): add test-infra file pages"
```

- [ ] **Step 4: Open PR and merge**

```bash
# In qemu-cxtg — nothing changed
# In runtime-cxtg:
git push origin feat/test-infra
# open PR feat/test-infra → cxtg-dev; review diff; merge
git checkout cxtg-dev && git pull
```

---

## Branch: feat/1.1

Work in: `qemu-cxtg` repo (`/home/cmd/Github/runtime-cxtg/qemu-cxtg`)

**Prerequisite:** feat/test-infra merged to `cxtg-dev`.

### Task 6: Cut branch and confirm baseline build

- [ ] **Step 1: Cut branch**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg
git checkout cxtg-dev && git pull
git checkout -b feat/1.1
```

- [ ] **Step 2: Confirm QEMU builds cleanly**

```bash
cd build
make -j$(nproc) 2>&1 | tail -5
```

Expected: `[100%]` line, no errors, no CX-related warnings.

---

### Task 7: Rename zicx → zcx across all files

**Files:**
- Modify: `target/riscv/cpu.c`
- Modify: `target/riscv/cpu_cfg_fields.h.inc`
- Modify: `target/riscv/csr.c`
- Modify: `target/riscv/kvm/kvm-cpu.c`
- Modify: `linux-headers/asm-riscv/kvm.h`

- [ ] **Step 1: Run sed across all files**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg

# Rename the ISA extension string and C symbol
sed -i \
    -e 's/ISA_EXT_DATA_ENTRY(zicx,/ISA_EXT_DATA_ENTRY(zcx,/g' \
    -e 's/MULTI_EXT_CFG_BOOL("zicx",/MULTI_EXT_CFG_BOOL("zcx",/g' \
    -e 's/ext_zicx/ext_zcx/g' \
    target/riscv/cpu.c \
    target/riscv/cpu_cfg_fields.h.inc \
    target/riscv/csr.c

sed -i \
    -e 's/KVM_EXT_CFG("zicx",/KVM_EXT_CFG("zcx",/g' \
    -e 's/ext_zicx/ext_zcx/g' \
    target/riscv/kvm/kvm-cpu.c

sed -i \
    -e 's/KVM_RISCV_ISA_EXT_ZICX/KVM_RISCV_ISA_EXT_ZCX/g' \
    linux-headers/asm-riscv/kvm.h \
    target/riscv/kvm/kvm-cpu.c
```

- [ ] **Step 2: Verify no zicx/ZICX remains**

```bash
grep -rn "zicx\|ZICX" target/riscv/ linux-headers/
```

Expected: no output.

- [ ] **Step 3: Commit**

```bash
git add target/riscv/cpu.c target/riscv/cpu_cfg_fields.h.inc \
        target/riscv/csr.c target/riscv/kvm/kvm-cpu.c \
        linux-headers/asm-riscv/kvm.h
git commit -m "target/riscv: rename zicx -> zcx extension"
```

---

### Task 8: Update CSR addresses and add bit-field constants

**Files:**
- Modify: `target/riscv/cpu_bits.h`
- Modify: `disas/riscv.c`

- [ ] **Step 1: Update cpu_bits.h**

Find the CX Extension block (around line 610) and replace it entirely:

```c
/* CX Extension */
/* TODO: CSR addresses are placeholders pending spec finalisation (~permanent) */
#define CSR_CXSEL           0xC20  /* was 0x800; standard URO user range */
#define CSR_CXSIDX          0x018  /* was 0x801; standard URW user range */
#define CSR_CXSDATA         0x019  /* was 0x802; standard URW user range */

/* cxsel field layout (Direct Mode, Figure 3):
 * [XLEN-1:16] reserved | [15:8] SID (spec calls this IDX) | [7:0] CXID */
#define CXSEL_CXID_POSITION 0
#define CXSEL_CXID_BITS     8
#define CXSEL_SID_POSITION  8
#define CXSEL_SID_BITS      8

#define CXSEL_CXID_MASK \
    (((target_ulong)((1U << CXSEL_CXID_BITS) - 1)) << CXSEL_CXID_POSITION)
#define CXSEL_SID_MASK \
    (((target_ulong)((1U << CXSEL_SID_BITS) - 1)) << CXSEL_SID_POSITION)
```

Remove the old `CXSEL_INV`, `CXSEL_TYPE`, `CXSEL_SEL` lines — they were Artur's placeholder layout, superseded by the spec's Direct Mode layout.

- [ ] **Step 2: Update disas/riscv.c CSR name table**

Find (around line 2447):
```c
    case 0x0800: return "cxsel"; // Unprivileged (User), Custom read-only (64 CSRs)
    case 0x0801: return "cxidx"; // Unprivileged (User), Custom read-write (64 CSRs)
    case 0x0802: return "cxdata"; // Unprivileged (User), Custom read-write (64 CSRs)
```

Replace with (using C-style comments and updated addresses and names):
```c
    case 0x0C20: return "cxsel";   /* URO user range */
    case 0x018:  return "cxsidx";  /* URW user range */
    case 0x019:  return "cxsdata"; /* URW user range */
```

- [ ] **Step 3: Commit**

```bash
git add target/riscv/cpu_bits.h disas/riscv.c
git commit -m "target/riscv: update CX CSR addresses and add cxsel field constants"
```

---

### Task 9: Rename CPURISCVState fields and fix cx.c

**Files:**
- Modify: `target/riscv/cpu.h`
- Modify: `target/riscv/cpu.c`
- Modify: `target/riscv/cx.c`
- Modify: `target/riscv/csr.c` (field names only, not predicates yet)

- [ ] **Step 1: Rename fields in cpu.h**

Find (around line 514):
```c
    target_ulong cxsel;
    target_ulong cxidx;
    target_ulong cxdata;
```

Replace with:
```c
    target_ulong cxsel;
    target_ulong cxsidx;
    target_ulong cxsdata;
```

- [ ] **Step 2: Rename in cpu.c reset handler**

Find (around line 801):
```c
        env->cxsel = 0;
        env->cxidx = 0;
        env->cxdata = 0;
```

Replace with:
```c
        env->cxsel = 0;
        env->cxsidx = 0;
        env->cxsdata = 0;
```

- [ ] **Step 3: Rewrite cx.c**

Replace the full contents of `target/riscv/cx.c` with:

```c
/*
 * RISC-V Zcx (Composable Extensions) CSR implementations.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "trace.h"

void cxsel_csr_read(CPURISCVState *env, uint32_t reg_index, target_ulong *val)
{
    *val = env->cxsel;
    trace_cxsel_csr_read(env->mhartid, reg_index, *val);
}

void cxsel_csr_write(CPURISCVState *env, uint32_t reg_index, target_ulong val)
{
    /* cxsel is URO — direct writes trap via hardware encoding; belt-and-suspenders */
    trace_cxsel_csr_write(env->mhartid, reg_index, val);
}

void cxsidx_csr_read(CPURISCVState *env, uint32_t reg_index, target_ulong *val)
{
    *val = env->cxsidx;
    trace_cxsidx_csr_read(env->mhartid, reg_index, *val);
}

void cxsidx_csr_write(CPURISCVState *env, uint32_t reg_index, target_ulong val)
{
    env->cxsidx = val;
    trace_cxsidx_csr_write(env->mhartid, reg_index, val);
}

void cxsdata_csr_read(CPURISCVState *env, uint32_t reg_index, target_ulong *val)
{
    *val = env->cxsdata;
    trace_cxsdata_csr_read(env->mhartid, reg_index, *val);
    env->cxsidx++;
}

void cxsdata_csr_write(CPURISCVState *env, uint32_t reg_index, target_ulong val)
{
    env->cxsdata = val;
    trace_cxsdata_csr_write(env->mhartid, reg_index, val);
    env->cxsidx++;
}
```

Key changes from Artur's original:
- `env->cxidx` → `env->cxsidx`, `env->cxdata` → `env->cxsdata`
- `// comment` → `/* comment */`
- Added `env->cxsidx++` to both cxsdata handlers (spec §4.4: after every cxsdata access, cxsidx increments)

- [ ] **Step 4: Verify no old field names remain**

```bash
grep -rn "env->cxidx\|env->cxdata" target/riscv/
```

Expected: no output.

- [ ] **Step 5: Commit**

```bash
git add target/riscv/cpu.h target/riscv/cpu.c target/riscv/cx.c
git commit -m "target/riscv: rename cxidx/cxdata fields to cxsidx/cxsdata; add cxsdata auto-increment"
```

---

### Task 10: Fix predicate functions in csr.c

**Files:**
- Modify: `target/riscv/csr.c`

The three predicate functions (cxsel, cxsidx, cxsdata) currently contain dead copy-paste from the PMP predicate. Replace all three.

- [ ] **Step 1: Replace the cxsel predicate**

Find (around line 5856):
```c
static RISCVException cxsel(CPURISCVState *env, int csrno)
{
    if (riscv_cpu_cfg(env)->ext_zicx) {
////////////
        if (csrno <= CSR_PMPCFG3) {
            uint32_t reg_index = csrno - CSR_PMPCFG0;

            /* TODO: RV128 restriction check */
            if ((reg_index & 1) && (riscv_cpu_mxl(env) == MXL_RV64)) {
                return RISCV_EXCP_ILLEGAL_INST;
            }
        }
///////////////
        return RISCV_EXCP_NONE;
    }

    return RISCV_EXCP_ILLEGAL_INST;
}
```

Replace with:
```c
static RISCVException cxsel(CPURISCVState *env, int csrno)
{
    if (!riscv_cpu_cfg(env)->ext_zcx) {
        return RISCV_EXCP_ILLEGAL_INST;
    }
    return RISCV_EXCP_NONE;
}
```

- [ ] **Step 2: Replace the cxsidx predicate**

Find (around line 5875):
```c
static RISCVException cxsidx(CPURISCVState *env, int csrno)
{
    if (riscv_cpu_cfg(env)->ext_zicx) {
////////////
        if (csrno <= CSR_PMPCFG3) {
            uint32_t reg_index = csrno - CSR_PMPCFG0;

            /* TODO: RV128 restriction check */
            if ((reg_index & 1) && (riscv_cpu_mxl(env) == MXL_RV64)) {
                return RISCV_EXCP_ILLEGAL_INST;
            }
        }
///////////////
        return RISCV_EXCP_NONE;
    }

    return RISCV_EXCP_ILLEGAL_INST;
}
```

Replace with:
```c
static RISCVException cxsidx(CPURISCVState *env, int csrno)
{
    if (!riscv_cpu_cfg(env)->ext_zcx) {
        return RISCV_EXCP_ILLEGAL_INST;
    }
    return RISCV_EXCP_NONE;
}
```

- [ ] **Step 3: Replace the cxsdata predicate**

Find (around line 5894):
```c
static RISCVException cxsdata(CPURISCVState *env, int csrno)
{
    if (riscv_cpu_cfg(env)->ext_zicx) {
////////////
        if (csrno <= CSR_PMPCFG3) {
            uint32_t reg_index = csrno - CSR_PMPCFG0;

            /* TODO: RV128 restriction check */
            if ((reg_index & 1) && (riscv_cpu_mxl(env) == MXL_RV64)) {
                return RISCV_EXCP_ILLEGAL_INST;
            }
        }
///////////////
        return RISCV_EXCP_NONE;
    }

    return RISCV_EXCP_ILLEGAL_INST;
}
```

Replace with:
```c
static RISCVException cxsdata(CPURISCVState *env, int csrno)
{
    if (!riscv_cpu_cfg(env)->ext_zcx) {
        return RISCV_EXCP_ILLEGAL_INST;
    }
    return RISCV_EXCP_NONE;
}
```

- [ ] **Step 4: Commit**

```bash
git add target/riscv/csr.c
git commit -m "target/riscv: fix CX CSR predicate functions — remove dead PMP copy-paste"
```

---

### Task 11: Build QEMU and run Block 1.1 test

- [ ] **Step 1: Rebuild QEMU**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg/build
make -j$(nproc) 2>&1 | tail -10
```

Expected: clean build, no errors. If compilation fails, check that all `ext_zicx` → `ext_zcx` and `cxidx`/`cxdata` → `cxsidx`/`cxsdata` renames are consistent.

- [ ] **Step 2: Verify zcx appears in CPU help**

```bash
./qemu-system-riscv64 -cpu rv64,help 2>&1 | grep zcx
```

Expected: `zcx` appears in the list.

- [ ] **Step 3: Write Block 1.1 test**

Create `runtime-cxtg/tests/block1_1_csr_stubs.S`:

```asm
/*
 * Block 1.1 test: Zcx CSR stub behavior.
 *
 * Tests:
 *   1. cxsel reads 0 at reset
 *   2. direct write to cxsel traps with mcause=2
 *   3. cxsidx round-trip (write/read)
 *   4. cxsdata round-trip (write/read)
 *   5. cxsdata auto-increment: write increments cxsidx; read increments again
 *
 * Run: make run-block1_1  (requires zcx=on QEMU binary)
 */

/* CSR addresses — must match cpu_bits.h values post Block 1.1 */
#define CSR_CXSEL   0xC20
#define CSR_CXSIDX  0x018
#define CSR_CXSDATA 0x019

    .section .text
    .globl main
main:
    /* --- Test 1: cxsel == 0 at reset --- */
    csrr  t0, CSR_CXSEL
    bnez  t0, .Lfail_1

    /* --- Test 2: direct write to cxsel traps with mcause=2 ---
     * Clear trap_mcause, attempt csrwi to URO cxsel, check mcause=2. */
    la    t0, trap_mcause
    sd    zero, 0(t0)
    csrwi CSR_CXSEL, 0
    ld    t1, 0(t0)
    li    t2, 2
    bne   t1, t2, .Lfail_2

    /* Confirm cxsel is still 0 after write attempt */
    csrr  t0, CSR_CXSEL
    bnez  t0, .Lfail_2b

    /* --- Test 3: cxsidx round-trip --- */
    li    t0, 7
    csrw  CSR_CXSIDX, t0
    csrr  t1, CSR_CXSIDX
    li    t2, 7
    bne   t1, t2, .Lfail_3

    li    t0, 255
    csrw  CSR_CXSIDX, t0
    csrr  t1, CSR_CXSIDX
    li    t2, 255
    bne   t1, t2, .Lfail_3b

    /* --- Test 4: cxsdata round-trip (reset cxsidx first) --- */
    csrwi CSR_CXSIDX, 0
    li    t0, 0xAB
    csrw  CSR_CXSDATA, t0      /* write: cxsidx should auto-increment to 1 */

    /* Reset cxsidx=0 to read back at known index */
    csrwi CSR_CXSIDX, 0
    csrr  t1, CSR_CXSDATA      /* read: should return 0xAB; cxsidx→1 again */
    li    t2, 0xAB
    bne   t1, t2, .Lfail_4

    /* --- Test 5: cxsdata auto-increment ---
     * Start: cxsidx=0. Write cxsdata → cxsidx should be 1. Read cxsdata → 2. */
    csrwi CSR_CXSIDX, 0
    csrr  t0, CSR_CXSIDX
    bnez  t0, .Lfail_5_pre     /* sanity: confirm cxsidx is 0 */

    li    t0, 0x55
    csrw  CSR_CXSDATA, t0      /* write: cxsidx 0 → 1 */
    csrr  t1, CSR_CXSIDX
    li    t2, 1
    bne   t1, t2, .Lfail_5a

    csrr  t0, CSR_CXSDATA      /* read: cxsidx 1 → 2 */
    csrr  t1, CSR_CXSIDX
    li    t2, 2
    bne   t1, t2, .Lfail_5b

    call  test_pass

.Lfail_1:
    la    a0, .Lmsg_1
    call  test_fail
.Lfail_2:
    la    a0, .Lmsg_2
    call  test_fail
.Lfail_2b:
    la    a0, .Lmsg_2b
    call  test_fail
.Lfail_3:
    la    a0, .Lmsg_3
    call  test_fail
.Lfail_3b:
    la    a0, .Lmsg_3b
    call  test_fail
.Lfail_4:
    la    a0, .Lmsg_4
    call  test_fail
.Lfail_5_pre:
    la    a0, .Lmsg_5_pre
    call  test_fail
.Lfail_5a:
    la    a0, .Lmsg_5a
    call  test_fail
.Lfail_5b:
    la    a0, .Lmsg_5b
    call  test_fail

    .section .rodata
.Lmsg_1:     .string "cxsel != 0 at reset"
.Lmsg_2:     .string "write cxsel: mcause != 2"
.Lmsg_2b:    .string "cxsel changed after write attempt"
.Lmsg_3:     .string "cxsidx round-trip failed (val=7)"
.Lmsg_3b:    .string "cxsidx round-trip failed (val=255)"
.Lmsg_4:     .string "cxsdata round-trip failed"
.Lmsg_5_pre: .string "cxsidx not 0 before auto-increment test"
.Lmsg_5a:    .string "cxsdata write did not increment cxsidx"
.Lmsg_5b:    .string "cxsdata read did not increment cxsidx"
```

- [ ] **Step 4: Build and run test**

```bash
cd /home/cmd/Github/runtime-cxtg
make tests/block1_1_csr_stubs.elf
make run-block1_1
```

Expected output:
```
PASSED
```

If you see `FAILED: cxsel != 0 at reset` — check CSR address 0xC20 in cpu_bits.h and that the binary was rebuilt after the address change.

If you see `FAILED: write cxsel: mcause != 2` — check that write_cxsel in csr.c returns RISCV_EXCP_ILLEGAL_INST and that the CSR address 0xC20 has csr[11:10]=11 (read-only bit encoding in the address itself — 0xC20 = 0b110000100000, bits 11:10 = 11 ✓).

- [ ] **Step 5: Update todo.md**

Open `docs/todo.md` and:
1. In the "CSR Address Stubs" table, update values from 0x800/0x801/0x802 to 0xC20/0x018/0x019
2. Confirm the cxsetsel encoding entry exists (if not, add it)
3. Add new entry:

```markdown
## cxsidx WARL range enforcement (Block 4.2)

Valid range for cxsidx is 0 to state_size-1 (spec §4.3). Phase 1 accepts
all values — no range clamping. Enforcement deferred to Block 4.2 when
state_size is wired up.

## SID vs IDX naming (Phase 1 decision)

This implementation calls the cxsel[15:8] field SID (State ID); the spec
(Figure 3) calls it IDX. TODO: align with TG/spec terminology once naming
is finalised (~permanent until spec stabilises).
```

- [ ] **Step 6: Update wiki**

Update existing wiki pages for all modified files:
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu_bits.h.md` — update CSR address values, add new constant symbols
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu.h.md` — update field names cxsidx, cxsdata
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu.c.md` — update reset handler field names
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cx.c.md` — update field names; add auto-increment note
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/csr.c.md` — update predicate descriptions

Add new wiki pages for files first touched by this block:
- `wiki/runtime-cxtg/qemu-cxtg/disas/riscv.c.md` (if not present — it is, check index)

Add a row to `wiki/index.md` for `tests/block1_1_csr_stubs.S`.

- [ ] **Step 7: Commit wiki and test**

```bash
cd /home/cmd/Github/runtime-cxtg
git add tests/block1_1_csr_stubs.S docs/todo.md wiki/
git commit -m "feat(1.1): add CSR stub test and wiki updates"
```

- [ ] **Step 8: Commit qemu-cxtg and merge**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg
git add -p   # review all changes
git commit -m "target/riscv: Zcx Block 1.1 — rename, addresses, field names, predicates, auto-increment"
git push origin feat/1.1
# open PR feat/1.1 → cxtg-dev; review; merge
git checkout cxtg-dev && git pull
```

After merging qemu-cxtg, update the submodule pointer in runtime-cxtg:

```bash
cd /home/cmd/Github/runtime-cxtg
git add qemu-cxtg
git commit -m "chore: update qemu-cxtg submodule to feat/1.1 merge"
git push origin cxtg-dev
```

---

## Branch: feat/1.2

Work in: `qemu-cxtg` (instruction) + `runtime-cxtg` (utils.h + test).

**Prerequisite:** feat/1.1 merged to `cxtg-dev`.

### Task 12: Cut branch and add encoding constants

**Files:**
- Modify: `target/riscv/cpu_bits.h` (qemu-cxtg)

- [ ] **Step 1: Cut branch in qemu-cxtg**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg
git checkout cxtg-dev && git pull
git checkout -b feat/1.2
```

- [ ] **Step 2: Add cxsetsel encoding constants to cpu_bits.h**

Append after the `CXSEL_SID_MASK` definition:

```c
/* cxsetsel instruction encoding — SYSTEM opcode, funct3=4
 * TODO: funct7 and rs2 pending spec assignment (~permanent) */
#define MATCH_CXSETSEL  0x00004073UL
#define MASK_CXSETSEL   0xFE007FFFUL  /* funct7+rs2+funct3+opcode fixed; rd+rs1 free */
```

- [ ] **Step 3: Commit**

```bash
git add target/riscv/cpu_bits.h
git commit -m "target/riscv: add cxsetsel encoding constants to cpu_bits.h"
```

---

### Task 13: Add cxsetsel to insn32.decode

**Files:**
- Modify: `target/riscv/insn32.decode`

- [ ] **Step 1: Append cxsetsel entry**

Add at the end of `insn32.decode` (before any EOF comment if present):

```
# Zcx (Composable Extensions)
# TODO: cxsetsel encoding pending spec assignment -- SYSTEM opcode, funct3=4, funct7=0
cxsetsel    0000000  00000 ..... 100 ..... 1110011 @r
```

The `@r` format decodes `%rs2 %rs1 %rd`; rs2 is always 0 here (fixed in the pattern) and is ignored by the handler.

- [ ] **Step 2: Commit**

```bash
git add target/riscv/insn32.decode
git commit -m "target/riscv: add cxsetsel instruction to insn32.decode"
```

---

### Task 14: Implement cxsetsel translation handler

**Files:**
- Create: `target/riscv/insn_trans/trans_zcx.c.inc`
- Modify: `target/riscv/translate.c`

- [ ] **Step 1: Create trans_zcx.c.inc**

Create `target/riscv/insn_trans/trans_zcx.c.inc`:

```c
/*
 * RISC-V translation routines for Zcx (Composable Extensions).
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#define REQUIRE_ZCX(ctx) do {            \
    if (!ctx->cfg_ptr->ext_zcx) {        \
        return false;                    \
    }                                    \
} while (0)

/*
 * cxsetsel rd, rs1
 *
 * Atomically swaps cxsel and rs1, applying clamping rules before writing:
 *   1. CXID==0 (legacy) -> SID field clamped to 0
 *   2. sign bit set -> clamped to ~0UL (CX_SEL_INVALID)
 *
 * If rd != x0, the old cxsel value is written to rd.
 * rs2 is reserved (always x0 in the encoding); the handler ignores a->rs2.
 */
static bool trans_cxsetsel(DisasContext *ctx, arg_r *a)
{
    TCGv old_cxsel;
    TCGv new_val;
    TCGv val;
    TCGv cxid;
    TCGLabel *skip_sid_clamp;
    TCGLabel *skip_neg_clamp;

    REQUIRE_ZCX(ctx);

    old_cxsel = tcg_temp_new();
    new_val = get_gpr(ctx, a->rs1, EXT_NONE);
    val = tcg_temp_new();
    cxid = tcg_temp_new();

    tcg_gen_ld_tl(old_cxsel, tcg_env, offsetof(CPURISCVState, cxsel));
    tcg_gen_mov_tl(val, new_val);

    /* Clamping rule 1: CXID==0 (legacy) -> clear SID */
    tcg_gen_andi_tl(cxid, val, CXSEL_CXID_MASK);
    skip_sid_clamp = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_NE, cxid, 0, skip_sid_clamp);
    tcg_gen_andi_tl(val, val, ~(target_ulong)CXSEL_SID_MASK);
    gen_set_label(skip_sid_clamp);

    /* Clamping rule 2: sign bit set -> ~0UL (CX_SEL_INVALID) */
    skip_neg_clamp = gen_new_label();
    tcg_gen_brcondi_tl(TCG_COND_GE, val, 0, skip_neg_clamp);
    tcg_gen_movi_tl(val, -1);
    gen_set_label(skip_neg_clamp);

    tcg_gen_st_tl(val, tcg_env, offsetof(CPURISCVState, cxsel));

    /* gen_set_gpr is a no-op when rd==0 */
    gen_set_gpr(ctx, a->rd, old_cxsel);

    return true;
}
```

- [ ] **Step 2: Add include to translate.c**

In `target/riscv/translate.c`, after the last `#include "insn_trans/..."` line (around line 1225), add:

```c
#include "insn_trans/trans_zcx.c.inc"
```

- [ ] **Step 3: Commit**

```bash
git add target/riscv/insn_trans/trans_zcx.c.inc target/riscv/translate.c
git commit -m "target/riscv: add cxsetsel instruction translation (Zcx Block 1.2)"
```

---

### Task 15: Build QEMU and verify cxsetsel assembles and disassembles

- [ ] **Step 1: Rebuild**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg/build
make -j$(nproc) 2>&1 | tail -10
```

Expected: clean build. If decodetree fails, check that the `cxsetsel` line in insn32.decode uses exactly two spaces before `@r` and that the `@r` format is already defined at the top of the file (it is — line 65).

---

### Task 16: Create utils.h and Block 1.2 test

**Files:**
- Create: `include/utils.h` (runtime-cxtg)
- Create: `tests/block1_2_cxsetsel.S` (runtime-cxtg)

- [ ] **Step 1: Cut branch in runtime-cxtg**

```bash
cd /home/cmd/Github/runtime-cxtg
git checkout cxtg-dev && git pull
git checkout -b feat/1.2
```

- [ ] **Step 2: Create include/utils.h**

Create `include/utils.h`:

```c
#ifndef UTILS_H
#define UTILS_H

/*
 * CXSETSEL(rd, rs1) — inline assembly wrapper for the cxsetsel instruction.
 *
 * cxsetsel rd, rs1: atomically writes rs1 to cxsel and returns old cxsel in rd.
 * Clamping is applied by QEMU before writing (CXID=0 -> SID=0; sign bit -> ~0UL).
 *
 * Usage:
 *   uintptr_t old;
 *   CXSETSEL(old, new_sel);
 *
 * TODO: encoding pending spec assignment -- SYSTEM opcode, funct3=4, funct7=0
 */
#define CXSETSEL(rd, rs1) \
    __asm__ volatile(".insn r 0x73, 0x4, 0x0, %0, %1, x0" \
                     : "=r"(rd) : "r"(rs1))

#endif /* UTILS_H */
```

- [ ] **Step 3: Write Block 1.2 test**

Create `tests/block1_2_cxsetsel.S`:

```asm
/*
 * Block 1.2 test: cxsetsel instruction behavior including clamping.
 *
 * Tests:
 *   1. Basic swap: cxsetsel with 0 returns 0 (reset value)
 *   2. Write non-zero: csrr cxsel confirms new value
 *   3. Atomic swap: rd gets old value, cxsel gets new
 *   4. rd=x0: write still occurs, no exception
 *   5. Clamping: CXID=0, SID!=0 -> SID cleared (stored as 0x0000)
 *   6. Clamping: sign bit set -> stored as ~0UL
 *
 * Run: make run-block1_2
 */

#define CSR_CXSEL   0xC20

/* cxsetsel rd, rs1 — .insn r opcode, funct3, funct7, rd, rs1, rs2 */
.macro cxsetsel rd, rs1
    .insn r 0x73, 0x4, 0x0, \rd, \rs1, x0
.endm

    .section .text
    .globl main
main:
    /* --- Test 1: cxsetsel a1, a0 with a0=0 -> a1=0, cxsel=0 --- */
    li    a0, 0
    cxsetsel a1, a0
    bnez  a1, .Lfail_1
    csrr  t0, CSR_CXSEL
    bnez  t0, .Lfail_1b

    /* --- Test 2: write 0x0102 (CXID=2, SID=1), read back --- */
    li    a0, 0x0102
    cxsetsel a1, a0
    csrr  t0, CSR_CXSEL
    li    t1, 0x0102
    bne   t0, t1, .Lfail_2

    /* --- Test 3: atomic swap — rd gets old value --- */
    /* cxsel is currently 0x0102 */
    li    a0, 0x0203
    cxsetsel a1, a0            /* a1 should be 0x0102 (old), cxsel -> 0x0203 */
    li    t1, 0x0102
    bne   a1, t1, .Lfail_3
    csrr  t0, CSR_CXSEL
    li    t1, 0x0203
    bne   t0, t1, .Lfail_3b

    /* --- Test 4: rd=x0 — write still occurs --- */
    li    a0, 0x0001
    cxsetsel x0, a0            /* write 0x0001 to cxsel; discard old value */
    csrr  t0, CSR_CXSEL
    li    t1, 0x0001
    bne   t0, t1, .Lfail_4

    /* --- Reset cxsel to 0 for clamping tests --- */
    li    a0, 0
    cxsetsel x0, a0

    /* --- Test 5a: CXID=0, SID=1 (0x0100) -> stored as 0x0000 --- */
    li    a0, 0x0100
    cxsetsel x0, a0
    csrr  t0, CSR_CXSEL
    bnez  t0, .Lfail_5a

    /* --- Test 5b: CXID=0, SID=0xFF (0xFF00) -> stored as 0x0000 --- */
    li    a0, 0xFF00
    cxsetsel x0, a0
    csrr  t0, CSR_CXSEL
    bnez  t0, .Lfail_5b

    /* --- Test 6a: sign bit set -> ~0UL --- */
    li    a0, 1
    slli  a0, a0, 63           /* a0 = 0x8000000000000000 */
    cxsetsel x0, a0
    csrr  t0, CSR_CXSEL
    li    t1, -1               /* ~0UL */
    bne   t0, t1, .Lfail_6a

    /* --- Test 6b: ~0UL passes through (already invalid) --- */
    li    a0, -1
    cxsetsel x0, a0
    csrr  t0, CSR_CXSEL
    li    t1, -1
    bne   t0, t1, .Lfail_6b

    call  test_pass

.Lfail_1:   la a0, .Lmsg_1;   call test_fail
.Lfail_1b:  la a0, .Lmsg_1b;  call test_fail
.Lfail_2:   la a0, .Lmsg_2;   call test_fail
.Lfail_3:   la a0, .Lmsg_3;   call test_fail
.Lfail_3b:  la a0, .Lmsg_3b;  call test_fail
.Lfail_4:   la a0, .Lmsg_4;   call test_fail
.Lfail_5a:  la a0, .Lmsg_5a;  call test_fail
.Lfail_5b:  la a0, .Lmsg_5b;  call test_fail
.Lfail_6a:  la a0, .Lmsg_6a;  call test_fail
.Lfail_6b:  la a0, .Lmsg_6b;  call test_fail

    .section .rodata
.Lmsg_1:   .string "cxsetsel 0: rd should be 0"
.Lmsg_1b:  .string "cxsetsel 0: cxsel should be 0"
.Lmsg_2:   .string "cxsetsel 0x0102: cxsel wrong"
.Lmsg_3:   .string "cxsetsel swap: rd did not get old value"
.Lmsg_3b:  .string "cxsetsel swap: cxsel did not get new value"
.Lmsg_4:   .string "cxsetsel rd=x0: cxsel not updated"
.Lmsg_5a:  .string "clamp: CXID=0,SID=1 not zeroed"
.Lmsg_5b:  .string "clamp: CXID=0,SID=0xFF not zeroed"
.Lmsg_6a:  .string "clamp: sign bit set not clamped to ~0UL"
.Lmsg_6b:  .string "clamp: ~0UL should pass through"
```

- [ ] **Step 4: Build and run test**

```bash
cd /home/cmd/Github/runtime-cxtg
make tests/block1_2_cxsetsel.elf
make run-block1_2
```

Expected output:
```
PASSED
```

- [ ] **Step 5: Update wiki**

New wiki pages:
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/insn_trans/trans_zcx.c.inc.md` — list `trans_cxsetsel`, `REQUIRE_ZCX`
- `wiki/runtime-cxtg/include/utils.h.md` — list `CXSETSEL` macro

Update existing:
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/insn32.decode.md` — add `cxsetsel` entry
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/translate.c.md` — add trans_zcx.c.inc include
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu_bits.h.md` — add MATCH_CXSETSEL, MASK_CXSETSEL

Update `wiki/index.md` with new rows.

- [ ] **Step 6: Commit and merge**

```bash
# runtime-cxtg changes:
git add include/utils.h tests/block1_2_cxsetsel.S wiki/
git commit -m "feat(1.2): add utils.h CXSETSEL macro and cxsetsel test"
git push origin feat/1.2
# PR feat/1.2 → cxtg-dev; review; merge

# qemu-cxtg changes (on feat/1.2 branch there):
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg
git push origin feat/1.2
# PR feat/1.2 → cxtg-dev; review; merge
cd ..
git checkout cxtg-dev && git pull
git add qemu-cxtg
git commit -m "chore: update qemu-cxtg submodule to feat/1.2 merge"
git push origin cxtg-dev
```

---

## Branch: feat/1.3

Work in: `runtime-cxtg`

**Prerequisite:** feat/1.2 merged to `cxtg-dev`.

### Task 17: Create runtime C library headers and implementation

**Files:**
- Create: `include/ccex.h`
- Create: `src/ccex.c`

- [ ] **Step 1: Cut branch**

```bash
cd /home/cmd/Github/runtime-cxtg
git checkout cxtg-dev && git pull
git checkout -b feat/1.3
```

- [ ] **Step 2: Create include/ccex.h**

Create `include/ccex.h`:

```c
#ifndef CCEX_H
#define CCEX_H

#include "utils.h"
#include <stdint.h>

/*
 * cxsel field layout — Direct Mode (Figure 3 of spec):
 *   [XLEN-1:16] reserved | [15:8] SID | [7:0] CXID
 *
 * Note: spec calls the [15:8] field IDX; this implementation uses SID (State ID)
 * for clarity.  See docs/todo.md for the alignment TODO.
 */
#define CXSEL_CXID_POSITION  0
#define CXSEL_CXID_BITS      8
#define CXSEL_SID_POSITION   8
#define CXSEL_SID_BITS       8
#define CXSEL_RSVD_POSITION  16
#define CXSEL_RSVD_BITS      ((int)(sizeof(uintptr_t) * 8) - CXSEL_RSVD_POSITION)

/*
 * cx_sel_t — type-safe CX selector value.
 *
 * Use .sel.cxid / .sel.sid for field access; use .raw for asm / comparisons.
 */
typedef union {
    struct {
        uintptr_t cxid : CXSEL_CXID_BITS;
        uintptr_t sid  : CXSEL_SID_BITS;
        uintptr_t rsvd : CXSEL_RSVD_BITS;
    } sel;
    uintptr_t raw;
} cx_sel_t;

#define CX_LEGACY      ((cx_sel_t){ .raw = 0 })
#define CX_SEL_INVALID ((cx_sel_t){ .raw = ~(uintptr_t)0 })

/*
 * cx_select: atomically write sel to cxsel, return old cxsel.
 *   Clamping rules are applied by the hardware (cxsetsel instruction).
 */
cx_sel_t cx_select(cx_sel_t sel);

/* cx_get_sel: read current cxsel value. */
cx_sel_t cx_get_sel(void);

/* cx_valid: return 1 if sel is not CX_SEL_INVALID, 0 otherwise. */
int cx_valid(cx_sel_t sel);

#endif /* CCEX_H */
```

- [ ] **Step 3: Create src/ccex.c**

Create `src/ccex.c`:

```c
/*
 * CX runtime — wrapper functions for Zcx CSR access.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "ccex.h"

cx_sel_t cx_select(cx_sel_t sel)
{
    cx_sel_t prev;
    CXSETSEL(prev.raw, sel.raw);
    return prev;
}

cx_sel_t cx_get_sel(void)
{
    cx_sel_t val;
    /* TODO: replace literal address once spec assigns a stable value */
    __asm__ volatile("csrr %0, 0xC20" : "=r"(val.raw));
    return val;
}

int cx_valid(cx_sel_t sel)
{
    return sel.raw != CX_SEL_INVALID.raw;
}
```

- [ ] **Step 4: Commit**

```bash
git add include/ccex.h src/ccex.c
git commit -m "feat(1.3): add ccex.h and ccex.c CX runtime wrappers"
```

---

### Task 18: Write wrapper test and update Makefile

**Files:**
- Create: `examples/test_wrappers.c`
- Modify: `Makefile`

- [ ] **Step 1: Write test_wrappers.c**

Create `examples/test_wrappers.c`:

```c
/*
 * examples/test_wrappers.c — exercise all ccex.h / ccex.c wrapper functions.
 *
 * Compiled as a bare-metal ELF; linked with tests/common/link.ld.
 * Run: make run-test_wrappers
 */

#include "ccex.h"
#include "test.h"

int main(void)
{
    cx_sel_t sel;
    cx_sel_t old;

    /* cx_get_sel() == CX_LEGACY at reset */
    sel = cx_get_sel();
    CHECK_EQ(sel.raw, CX_LEGACY.raw, "cx_get_sel: not 0 at reset");

    /* cx_valid(CX_LEGACY) == 1 (0 is the builtin, not invalid) */
    CHECK_EQ(cx_valid(CX_LEGACY), 1, "cx_valid(CX_LEGACY) should be 1");

    /* cx_valid(CX_SEL_INVALID) == 0 */
    CHECK_EQ(cx_valid(CX_SEL_INVALID), 0, "cx_valid(CX_SEL_INVALID) should be 0");

    /* cx_select: write 0x0102, returns old (0), cxsel becomes 0x0102 */
    cx_sel_t new_sel;
    new_sel.raw = 0x0102;
    old = cx_select(new_sel);
    CHECK_EQ(old.raw, CX_LEGACY.raw, "cx_select: old should be CX_LEGACY");
    sel = cx_get_sel();
    CHECK_EQ(sel.raw, 0x0102UL, "cx_select: cxsel should be 0x0102");

    /* cx_select: restore to CX_LEGACY */
    old = cx_select(CX_LEGACY);
    CHECK_EQ(old.raw, 0x0102UL, "cx_select restore: old should be 0x0102");
    sel = cx_get_sel();
    CHECK_EQ(sel.raw, CX_LEGACY.raw, "cx_select restore: cxsel should be 0");

    /* cx_sel_t bitfield access */
    cx_sel_t s;
    s.raw = 0;
    s.sel.cxid = 3;
    s.sel.sid = 5;
    CHECK_EQ(s.raw, 0x0503UL, "cx_sel_t: bitfield encoding wrong");

    test_pass();
    return 0;
}
```

- [ ] **Step 2: Update Makefile**

Add to the Makefile (after the existing ELF targets):

```makefile
CX_LIB_SRCS = src/ccex.c

examples/test_wrappers.elf: examples/test_wrappers.c $(CX_LIB_SRCS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -Iinclude $^ -o $@

run-test_wrappers: examples/test_wrappers.elf
	$(QEMU) $(QEMU_BASE_FLAGS) -cpu rv64,zcx=on -bios $<
```

Also update `all-tests`:
```makefile
all-tests: tests/smoke.elf tests/block1_1_csr_stubs.elf \
           tests/block1_2_cxsetsel.elf examples/test_wrappers.elf
```

And add `run-test_wrappers` to the `.PHONY` line.

- [ ] **Step 3: Build and run**

```bash
make examples/test_wrappers.elf
make run-test_wrappers
```

Expected output:
```
PASSED
```

If `cx_sel_t` bitfield encoding test fails: bitfield ordering is implementation-defined in C. If the compiler packs `cxid` at the MSB instead of LSB, swap the struct field order or switch to explicit masking. On GCC for RISC-V (little-endian), bitfields are packed LSB-first, so `cxid` (declared first) occupies bits 7:0. This should be correct.

- [ ] **Step 4: Commit**

```bash
git add examples/test_wrappers.c Makefile
git commit -m "feat(1.3): add test_wrappers example and Makefile targets"
```

---

### Task 19: Update wiki and merge feat/1.3

- [ ] **Step 1: Create wiki pages**

New pages:
- `wiki/runtime-cxtg/include/ccex.h.md` — list `cx_sel_t`, `CX_LEGACY`, `CX_SEL_INVALID`, `cx_select`, `cx_get_sel`, `cx_valid`
- `wiki/runtime-cxtg/src/ccex.c.md` — list implementation of each function
- `wiki/runtime-cxtg/examples/test_wrappers.c.md` — one-line description

Update pages:
- `wiki/runtime-cxtg/include/utils.h.md` — already created in feat/1.2; confirm `CXSETSEL` is listed

Update `wiki/index.md` with new rows.

- [ ] **Step 2: Commit wiki**

```bash
git add wiki/
git commit -m "docs(wiki): add feat/1.3 file pages"
```

- [ ] **Step 3: Open PR and merge**

```bash
git push origin feat/1.3
# PR feat/1.3 → cxtg-dev; review; merge
git checkout cxtg-dev && git pull
```

---

## Phase Milestone: tag cxtg-v0.phase1

**Prerequisite:** all four branches merged to `cxtg-dev` in both repos.

### Task 20: Merge and tag phase milestone

- [ ] **Step 1: Final full test run**

```bash
cd /home/cmd/Github/runtime-cxtg
make all-tests
make run-smoke && make run-block1_1 && make run-block1_2 && make run-test_wrappers
```

Expected: all print `PASSED`.

- [ ] **Step 2: Merge qemu-cxtg cxtg-dev → cxtg and tag**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg
git checkout cxtg && git merge --no-ff cxtg-dev
git tag cxtg-v0.phase1
git push origin cxtg cxtg-v0.phase1
```

- [ ] **Step 3: Update submodule pointer and tag runtime-cxtg**

```bash
cd /home/cmd/Github/runtime-cxtg
git checkout cxtg-dev
git add qemu-cxtg
git commit -m "chore: update qemu-cxtg submodule to cxtg-v0.phase1"
git checkout cxtg && git merge --no-ff cxtg-dev
git tag cxtg-v0.phase1
git push origin cxtg cxtg-v0.phase1
```

- [ ] **Step 4: Update progress.md**

In `docs/progress.md`:
1. Check off all Phase 1 block items
2. Fill in the `### v0.phase1` release notes entry following the format from `### v0.phase0`

```markdown
### v0.phase1

**qemu-cxtg** (`git diff --stat cxtg-v0.phase0..cxtg-v0.phase1` run inside qemu-cxtg/):

- modified: `target/riscv/cpu_bits.h` — updated CSR_CXSEL/CSR_CXSIDX/CSR_CXSDATA from 0x800/0x801/0x802 to 0xC20/0x018/0x019; removed CXSEL_INV/TYPE/SEL; added CXSEL_CXID_POSITION/BITS, CXSEL_SID_POSITION/BITS, CXSEL_CXID_MASK, CXSEL_SID_MASK, MATCH_CXSETSEL, MASK_CXSETSEL
- modified: `target/riscv/cpu.h` — renamed fields cxidx→cxsidx, cxdata→cxsdata in CPURISCVState
- modified: `target/riscv/cpu.c` — renamed ext_zicx→ext_zcx; ISA_EXT_DATA_ENTRY/MULTI_EXT_CFG_BOOL string "zicx"→"zcx"; reset handler uses cxsidx/cxsdata
- modified: `target/riscv/cpu_cfg_fields.h.inc` — renamed BOOL_FIELD(ext_zicx)→BOOL_FIELD(ext_zcx)
- modified: `target/riscv/csr.c` — predicates cxsel/cxsidx/cxsdata: removed dead PMP copy-paste; condition flipped to !ext_zcx
- modified: `target/riscv/cx.c` — renamed env->cxidx→env->cxsidx, env->cxdata→env->cxsdata; fixed // comments; added env->cxsidx++ to cxsdata_csr_read and cxsdata_csr_write
- modified: `target/riscv/insn32.decode` — added cxsetsel R-type entry (SYSTEM/funct3=4/funct7=0)
- created: `target/riscv/insn_trans/trans_zcx.c.inc` — trans_cxsetsel handler with REQUIRE_ZCX macro and clamping TCG logic
- modified: `target/riscv/translate.c` — added #include "insn_trans/trans_zcx.c.inc"
- modified: `target/riscv/kvm/kvm-cpu.c` — renamed KVM_EXT_CFG("zicx"→"zcx"), KVM_RISCV_ISA_EXT_ZICX→ZCX
- modified: `linux-headers/asm-riscv/kvm.h` — renamed KVM_RISCV_ISA_EXT_ZICX→KVM_RISCV_ISA_EXT_ZCX
- modified: `disas/riscv.c` — updated CSR addresses 0x800→0xC20, 0x801→0x018, 0x802→0x019; names cxidx→cxsidx, cxdata→cxsdata

**runtime-cxtg** changes:

- created: `tests/common/link.ld` — bare-metal linker script; DRAM at 0x80000000; 4M RAM; _stack_top at top
- created: `tests/common/crt0.S` — M-mode startup: _start, _trap_handler, trap_mcause global, test_pass, test_fail, .Lputs
- created: `tests/common/uart.h` — uart_putchar, uart_puts, uart_print_hex via UART_TX at 0x10000000
- created: `tests/common/test.h` — CHECK_EQ macro; extern trap_mcause; test_pass/test_fail declarations
- created: `Makefile` — CROSS_COMPILE, QEMU path, per-test ELF and run-* targets, all-tests, clean
- created: `README.md` — repository layout, dependency documentation, build instructions
- created: `tests/smoke.S` — harness smoke test (UART output, trap recording)
- created: `tests/block1_1_csr_stubs.S` — Block 1.1: cxsel reset/write-trap, cxsidx round-trip, cxsdata round-trip, auto-increment
- created: `include/utils.h` — CXSETSEL inline asm macro
- created: `tests/block1_2_cxsetsel.S` — Block 1.2: cxsetsel swap, rd=x0, clamping (CXID=0→SID=0, sign bit→~0UL)
- created: `include/ccex.h` — cx_sel_t union, CX_LEGACY, CX_SEL_INVALID, cx_select/cx_get_sel/cx_valid declarations
- created: `src/ccex.c` — cx_select (CXSETSEL macro), cx_get_sel (csrr 0xC20), cx_valid (~0UL check)
- created: `examples/test_wrappers.c` — C-level test of all ccex.h wrappers

**New deferred items added to docs/todo.md:**
- `cxsidx WARL range (0 to state_size-1) — deferred to Block 4.2`
- `SID vs IDX naming alignment with TG/spec (~permanent until spec stabilises)`
```

- [ ] **Step 5: Commit progress.md**

```bash
git add docs/progress.md
git commit -m "docs: Phase 1 complete — update progress.md with v0.phase1 release notes"
git push origin cxtg-dev
```
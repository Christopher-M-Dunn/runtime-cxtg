# Block 1.2 — cxsetsel Instruction Stub Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement `cxsetsel rd, rs1` as a decoded QEMU instruction that atomically swaps `rs1` into `env->cxsel` and returns the old value in `rd`.

**Architecture:** Decodetree entry in `insn32.decode` wires to `trans_cxsetsel` in a new `insn_trans/trans_rvzcx.c.inc` file; a TCG helper declared via `DEF_HELPER_2` in `helper.h` and implemented in `cx.c` does the actual swap; `runtime-cxtg` provides a `CXSETSEL()` macro and two test programs (assembly first, C macro after QEMU is working).

**Tech Stack:** QEMU decodetree, TCG helper X-macro system, bare-metal RISC-V ELF tests (riscv64-unknown-elf-gcc + qemu-system-riscv64).

## Global Constraints

- QEMU code style: 4-space indent, 80-char lines, `/* */` comments only (no `//`), always use braces
- All `.c.inc` files `#include`-d into `translate.c` — do NOT add them to `meson.build`
- `cx.c` is already in `meson.build` from block 1.1 — no meson change needed
- `cxsel` is XLEN-wide — use `target_ulong` / `tl` throughout, never `uint64_t`
- No WARL clamping in this block — `env->cxsel` accepts any value as-is
- RV128: no special handling needed; `get_gpr`/`gen_set_gpr` operate on low 64 bits, consistent with all non-base QEMU extensions
- Wiki: update immediately after each source file write, before moving to the next file
- Ask "Ready to commit?" and wait for user confirmation before every `git add` / `git commit`
- Branch rule: all work on `feat/1.2`; never commit directly to `cxtg-dev`

---

## File Map

| File | Repo | Action |
|------|------|--------|
| `target/riscv/cpu_bits.h` | qemu-cxtg | Modify — add `MATCH_CXSETSEL`, `MASK_CXSETSEL` |
| `target/riscv/trace-events` | qemu-cxtg | Modify — add `cxsetsel` trace event |
| `target/riscv/insn32.decode` | qemu-cxtg | Modify — add `cxsetsel` decode pattern |
| `target/riscv/helper.h` | qemu-cxtg | Modify — add `DEF_HELPER_2(cxsetsel, tl, env, tl)` |
| `target/riscv/cx.h` | qemu-cxtg | Modify — add `helper_cxsetsel` prototype |
| `target/riscv/cx.c` | qemu-cxtg | Modify — add `helper_cxsetsel` body |
| `target/riscv/insn_trans/trans_rvzcx.c.inc` | qemu-cxtg | Create — `trans_cxsetsel` |
| `target/riscv/translate.c` | qemu-cxtg | Modify — add `#include "insn_trans/trans_rvzcx.c.inc"` |
| `disas/riscv.c` | qemu-cxtg | Modify — add enum value, opcode list entry, decode case |
| `tests/block1_2_cxsetsel.S` | runtime-cxtg | Create — raw assembly test (written before QEMU impl) |
| `tests/output/block1_2_cxsetsel.out` | runtime-cxtg | Create — captured run output |
| `include/utils.h` | runtime-cxtg | Create — `CXSETSEL(rd, rs1)` macro (after QEMU impl) |
| `tests/block1_2_cxsetsel_macro.c` | runtime-cxtg | Create — C macro test (after QEMU impl) |
| `tests/output/block1_2_cxsetsel_macro.out` | runtime-cxtg | Create — captured run output |
| `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu_bits.h.md` | runtime-cxtg | Update |
| `wiki/runtime-cxtg/qemu-cxtg/target/riscv/trace-events.md` | runtime-cxtg | Update |
| `wiki/runtime-cxtg/qemu-cxtg/target/riscv/insn32.decode.md` | runtime-cxtg | Create |
| `wiki/runtime-cxtg/qemu-cxtg/target/riscv/helper.h.md` | runtime-cxtg | Create |
| `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cx.h.md` | runtime-cxtg | Update |
| `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cx.c.md` | runtime-cxtg | Update |
| `wiki/runtime-cxtg/qemu-cxtg/target/riscv/insn_trans/trans_rvzcx.c.inc.md` | runtime-cxtg | Create |
| `wiki/runtime-cxtg/qemu-cxtg/target/riscv/translate.c.md` | runtime-cxtg | Create |
| `wiki/runtime-cxtg/qemu-cxtg/disas/riscv.c.md` | runtime-cxtg | Create |
| `wiki/runtime-cxtg/include/utils.h.md` | runtime-cxtg | Create |
| `wiki/runtime-cxtg/tests/block1_2_cxsetsel.S.md` | runtime-cxtg | Create |
| `wiki/runtime-cxtg/tests/block1_2_cxsetsel_macro.c.md` | runtime-cxtg | Create |
| `wiki/index.md` | runtime-cxtg | Update — add all new wiki pages |

---

## Task 1: Branch Setup

**Files:** none (git operations only)

- [ ] **Step 1: Cut feat/1.2 in qemu-cxtg**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg
git checkout cxtg-dev
git pull
git checkout -b feat/1.2
```

- [ ] **Step 2: Cut feat/1.2 in runtime-cxtg**

```bash
cd /home/cmd/Github/runtime-cxtg
git checkout cxtg-dev
git pull
git checkout -b feat/1.2
```

- [ ] **Step 3: Verify both are on feat/1.2**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg && git branch --show-current
cd /home/cmd/Github/runtime-cxtg && git branch --show-current
```

Expected: `feat/1.2` for both.

---

## Task 2: Write Assembly Test (TDD — test first)

Write the `.S` test before touching QEMU. Running it now should produce an illegal instruction trap, confirming the instruction is not yet implemented.

**Files:**
- Create: `tests/block1_2_cxsetsel.S`

- [ ] **Step 1: Create `tests/block1_2_cxsetsel.S`**

Create `/home/cmd/Github/runtime-cxtg/tests/block1_2_cxsetsel.S`:

```asm
/* Block 1.2 test -- cxsetsel raw instruction encoding
 *
 * Tests all five behavioral cases:
 *   1. Swap from 0: old=0, cxsel=0
 *   2. Swap in 0x0102: old=0, cxsel=0x0102
 *   3. Swap in ~0: old=0x0102, cxsel=~0 (no clamping in stub)
 *   4. rd=x0 discards return but still writes cxsel=0x55
 *   5. Round-trip: swap then swap back, value restored
 *
 * cxsetsel encoding: .insn r 0x73, 4, 0, rd, rs1, x0
 *   MATCH=0x00004073  MASK=0xFFF0707F
 *   opcode=0x73 (SYSTEM) funct3=4 funct7=0 rs2=x0
 */

.section .text
.global main
main:

/* Case 1: cxsetsel a1, a0  (a0=0) -> a1=0, cxsel=0 */
    li      a0, 0
    .insn r 0x73, 4, 0, a1, a0, x0
    bnez    a1, fail                    /* old cxsel must be 0 */
    csrr    t1, 0xCA0                   /* read cxsel (CSR address 0xCA0) */
    bnez    t1, fail                    /* cxsel must still be 0 */

/* Case 2: cxsetsel a1, a0  (a0=0x0102) -> a1=0, cxsel=0x0102 */
    li      a0, 0x0102
    .insn r 0x73, 4, 0, a1, a0, x0
    bnez    a1, fail                    /* old cxsel must be 0 (from case 1) */
    csrr    t1, 0xCA0
    li      t0, 0x0102
    bne     t1, t0, fail               /* cxsel must now be 0x0102 */

/* Case 3: cxsetsel a1, a0  (a0=~0) -> a1=0x0102, cxsel=~0 */
    li      a0, -1
    .insn r 0x73, 4, 0, a1, a0, x0
    li      t0, 0x0102
    bne     a1, t0, fail               /* old cxsel must be 0x0102 */
    csrr    t1, 0xCA0
    li      t0, -1
    bne     t1, t0, fail               /* cxsel must be ~0; no clamping in stub */

/* Case 4: cxsetsel x0, a0  (rd=x0, a0=0x55) -> cxsel=0x55, return discarded */
    li      a0, 0x55
    .insn r 0x73, 4, 0, x0, a0, x0
    csrr    t1, 0xCA0
    li      t0, 0x55
    bne     t1, t0, fail               /* cxsel must be 0x55 */

/* Case 5: round-trip */
    li      a0, 0xABCD
    .insn r 0x73, 4, 0, a1, a0, x0    /* swap in 0xABCD; get back 0x55 */
    li      t0, 0x55
    bne     a1, t0, fail               /* old must be 0x55 */
    .insn r 0x73, 4, 0, a2, a1, x0    /* swap in 0x55; get back 0xABCD */
    li      t0, 0xABCD
    bne     a2, t0, fail               /* must have gotten back 0xABCD */
    csrr    t1, 0xCA0
    li      t0, 0x55
    bne     t1, t0, fail               /* cxsel must be 0x55 again */

    j       pass

fail:
    call    test_fail

pass:
    call    test_pass
```

- [ ] **Step 2: Build the test ELF**

```bash
cd /home/cmd/Github/runtime-cxtg
make tests/block1_2_cxsetsel.elf
```

Expected: compiles cleanly. GAS accepts `.insn r` regardless of QEMU support.

- [ ] **Step 3: Run — expect illegal instruction (not yet implemented)**

```bash
make run-block1_2_cxsetsel
```

Expected: `FAILED` (the trap handler fires on the unrecognized instruction) or QEMU reports an unimplemented instruction. This confirms the test is wired and cxsetsel is not yet in QEMU.

- [ ] **Step 4: Update wiki for `tests/block1_2_cxsetsel.S`**

Check `wiki/schema.md` for format rules. Create `wiki/runtime-cxtg/tests/block1_2_cxsetsel.S.md` and add an entry to `wiki/index.md`.

---

## Task 3: QEMU — Encoding Constants, Decode Entry, Trace Event

**Files:**
- Modify: `target/riscv/cpu_bits.h`
- Modify: `target/riscv/insn32.decode`
- Modify: `target/riscv/trace-events`

### Interfaces

- Produces: `MATCH_CXSETSEL` / `MASK_CXSETSEL` constants; decodetree pattern that generates `arg_r2` struct and calls `trans_cxsetsel`; `trace_cxsetsel()` callable in `cx.c`

- [ ] **Step 1: Add encoding constants to `cpu_bits.h`**

Find the CX CSR definitions block (search for `CSR_CXSEL`). Add immediately after the existing CX definitions:

```c
/* cxsetsel instruction encoding:
 * SYSTEM opcode 0x73, funct3=100, funct7=0000000, rs2=x0
 */
#define MATCH_CXSETSEL  0x00004073U
#define MASK_CXSETSEL   0xFFF0707FU
```

- [ ] **Step 2: Update wiki for `cpu_bits.h`**

Update `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu_bits.h.md` to document the new constants.

- [ ] **Step 3: Add decode entry to `insn32.decode`**

Find the SYSTEM opcode group (search for `ecall` near line 115). Add `cxsetsel` in the same group, after the existing system instructions:

```
# *** Zcx composable extensions ***
cxsetsel  0000000 00000 ..... 100 ..... 1110011  @r2
```

The `@r2` format (existing, defined earlier in the file at the `@r2` line) expands to `arg_r2 { rs1, rd }`. Decodetree generates the dispatch and calls `trans_cxsetsel(ctx, &arg)`.

- [ ] **Step 4: Create wiki for `insn32.decode`**

Create `wiki/runtime-cxtg/qemu-cxtg/target/riscv/insn32.decode.md`. Update `wiki/index.md`.

- [ ] **Step 5: Add trace event to `trace-events`**

Find the CX section (search for `cxsel_csr_read`). Add after the existing CX events:

```
cxsetsel(uint64_t hartid, uint64_t new_val, uint64_t old_val) "hart=%" PRId64 " new=0x%" PRIx64 " old=0x%" PRIx64
```

- [ ] **Step 6: Update wiki for `trace-events`**

Update `wiki/runtime-cxtg/qemu-cxtg/target/riscv/trace-events.md`.

---

## Task 4: QEMU — Helper Declaration and Body

**Files:**
- Modify: `target/riscv/helper.h`
- Modify: `target/riscv/cx.h`
- Modify: `target/riscv/cx.c`

### Interfaces

- Consumes: `trace_cxsetsel()` from Task 3; `env->cxsel` field (existing from block 1.1)
- Produces: `helper_cxsetsel(env, new_val)` C function; `gen_helper_cxsetsel(dest, tcg_env, src)` TCG wrapper (auto-generated by the `DEF_HELPER_2` X-macro — no manual wrapper needed)

- [ ] **Step 1: Declare helper in `helper.h`**

Find the CSR helper declarations (search for `DEF_HELPER_2(csrr` — near line 125). Add immediately after:

```c
DEF_HELPER_2(cxsetsel, tl, env, tl)
```

This means: function name `cxsetsel`, return type `tl` (target_ulong = XLEN bits), args `env` (CPURISCVState*) and `tl` (target_ulong new value). The X-macro system auto-generates `gen_helper_cxsetsel(TCGv dest, TCGv_ptr env, TCGv src)` for use in `trans_rvzcx.c.inc`.

- [ ] **Step 2: Create wiki for `helper.h`**

Create `wiki/runtime-cxtg/qemu-cxtg/target/riscv/helper.h.md`. Update `wiki/index.md`.

- [ ] **Step 3: Add prototype to `cx.h`**

Find the existing CX function prototypes (search for `cxsdata_csr_op`). Add:

```c
target_ulong helper_cxsetsel(CPURISCVState *env, target_ulong new_val);
```

- [ ] **Step 4: Update wiki for `cx.h`**

Update `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cx.h.md`.

- [ ] **Step 5: Add helper body to `cx.c`**

Add at the end of `cx.c`, after `cxsdata_csr_op`:

```c
target_ulong helper_cxsetsel(CPURISCVState *env, target_ulong new_val)
{
    target_ulong old = env->cxsel;
    env->cxsel = new_val;
    trace_cxsetsel(env->mhartid, new_val, old);
    return old;
}
```

No clamping. `env->cxsel` is XLEN-wide (`target_ulong`) and accepts any value. WARL validation is deferred to block 2.3.

- [ ] **Step 6: Update wiki for `cx.c`**

Update `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cx.c.md`.

---

## Task 5: QEMU — Translate Function

**Files:**
- Create: `target/riscv/insn_trans/trans_rvzcx.c.inc`
- Modify: `target/riscv/translate.c`

### Interfaces

- Consumes: `gen_helper_cxsetsel()` (auto-generated in Task 4's compilation unit); `do_csr_post(ctx)` (static in `trans_rvi.c.inc`, accessible in same TU); `get_gpr()`, `dest_gpr()`, `gen_set_gpr()`, `translator_io_start()` (all in `translate.c`)
- Produces: `trans_cxsetsel(ctx, arg_r2*)` called by decodetree dispatch

- [ ] **Step 1: Create `insn_trans/trans_rvzcx.c.inc`**

Create `/home/cmd/Github/runtime-cxtg/qemu-cxtg/target/riscv/insn_trans/trans_rvzcx.c.inc`:

```c
/*
 * RISC-V translation routines for Zcx (Composable Extensions).
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

static bool trans_cxsetsel(DisasContext *ctx, arg_r2 *a)
{
    TCGv src, dest;

    if (!ctx->cfg_ptr->ext_zcx) {
        return false;
    }
    src  = get_gpr(ctx, a->rs1, EXT_NONE);
    dest = dest_gpr(ctx, a->rd);
    translator_io_start(&ctx->base);
    gen_helper_cxsetsel(dest, tcg_env, src);
    gen_set_gpr(ctx, a->rd, dest);
    return do_csr_post(ctx);
}
```

Notes:
- `do_csr_post` saves the opcode for exception unwind, advances PC, and forces a TB exit. Required because `env->cxsel` affects how subsequent CX instructions execute. It is `static` in `trans_rvi.c.inc` but accessible here because both files are `#include`-d into the same `translate.c` translation unit.
- No RV128 special case: `get_gpr`/`gen_set_gpr` operate on the low 64 bits in RV128 mode, consistent with all non-base QEMU extensions. Only `trans_rvi.c.inc`, `trans_rvm.c.inc`, and `trans_rvzacas.c.inc` handle RV128 explicitly.

- [ ] **Step 2: Create wiki for `trans_rvzcx.c.inc`**

Create `wiki/runtime-cxtg/qemu-cxtg/target/riscv/insn_trans/trans_rvzcx.c.inc.md`. Update `wiki/index.md`.

- [ ] **Step 3: Add include to `translate.c`**

Find the Z-extension include block (search for `trans_rvzimop` — near line 1206). Add our include immediately after:

```c
#include "insn_trans/trans_rvzimop.c.inc"
#include "insn_trans/trans_rvzcx.c.inc"        /* Zcx composable extensions */
```

- [ ] **Step 4: Create wiki for `translate.c`**

Create `wiki/runtime-cxtg/qemu-cxtg/target/riscv/translate.c.md`. Update `wiki/index.md`.

---

## Task 6: QEMU — Disassembler

**Files:**
- Modify: `disas/riscv.c`

### Interfaces

- Produces: `cxsetsel rd, rs1` mnemonic in `qemu -d in_asm` output when `zcx=on`

- [ ] **Step 1: Add enum value**

Find the end of the `rv_op_*` enum. The last assigned value is `rv_op_c_sspopchk = 955` (near line 986). Add immediately after:

```c
    rv_op_cxsetsel = 956,
```

- [ ] **Step 2: Add opcode list entry**

The `rv_opcode_list[]` array is indexed by `rv_op_*` enum value — entries must appear in the same order as the enum. Find `rv_op_c_sspopchk`'s entry in the array (search for `"c.sspopchk"` or `rv_op_c_sspopchk`). Add immediately after:

```c
    { "cxsetsel", rv_codec_r, rv_fmt_rd_rs1, NULL, 0, 0, 0 },
```

`rv_codec_r` decodes the R-type fields. `rv_fmt_rd_rs1` prints `rd, rs1` (rs2 is fixed zero and omitted). Both constants exist in `disas/riscv.c` — `rv_fmt_rd_rs1` is confirmed at line 1573+.

- [ ] **Step 3: Add decode case**

Find `case 4:` inside the SYSTEM opcode decode block (around line 4068 — the funct3=100 case). Add our check *before* the existing Zimop block. Zimop instructions always require bit 31 = 1; cxsetsel has bit 31 = 0, so there is no overlap:

```c
            case 4:
                /* cxsetsel: SYSTEM funct3=4, funct7=0, rs2=0 (bits[31:20] all zero) */
                if (dec->cfg && dec->cfg->ext_zcx &&
                    ((inst >> 20) == 0)) {
                    op = rv_op_cxsetsel;
                    break;
                }
                if (dec->cfg && dec->cfg->ext_zimop) {
                    /* existing Zimop handling -- leave unchanged */
```

The check `(inst >> 20) == 0` verifies bits[31:20] are all zero (funct7=0 + rs2=0). This is the complete discriminator: Zimop requires bit 31 = 1, cxsetsel requires bit 31 = 0.

- [ ] **Step 4: Create wiki for `disas/riscv.c`**

Create `wiki/runtime-cxtg/qemu-cxtg/disas/riscv.c.md`. Update `wiki/index.md`.

---

## Task 7: Build QEMU and Verify Assembly Test

**Files:** none (build and verification only)

- [ ] **Step 1: Build QEMU**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg/build
make -j$(nproc) qemu-system-riscv64
```

Expected: clean build, no errors. If you see `warning: trace event 'cxsetsel' does not exist` — the `trace-events` file was not saved correctly; re-check Task 3 Step 5.

- [ ] **Step 2: Smoke-test disassembler**

Verify cxsetsel disassembles correctly. The encoding of `cxsetsel a0, a0` is:
`funct7=0 rs2=x0 rs1=a0(10) funct3=100 rd=a0(10) opcode=1110011`
= `0000000 00000 01010 100 01010 1110011` = `0x00054073`

```bash
cd /tmp
printf '\x73\x40\x05\x00' > cxsetsel_test.bin
/home/cmd/Github/runtime-cxtg/qemu-cxtg/build/qemu-system-riscv64 \
    -cpu rv64,zcx=on -M virt -bios cxsetsel_test.bin -d in_asm -nographic 2>&1 | head -10
```

Expected output contains: `cxsetsel a0,a0`

- [ ] **Step 3: Run assembly test — expect PASSED**

```bash
cd /home/cmd/Github/runtime-cxtg
make run-block1_2_cxsetsel
```

Expected: `PASSED`

- [ ] **Step 4: Capture output**

```bash
make run-block1_2_cxsetsel > tests/output/block1_2_cxsetsel.out 2>&1
cat tests/output/block1_2_cxsetsel.out
```

- [ ] **Step 5: Verify trace events fire**

```bash
make run-block1_2_cxsetsel QEMU_FLAGS="-d trace:cxsetsel" 2>&1 | grep "hart="
```

Expected: one line per `cxsetsel` execution showing `hart=0 new=0x... old=0x...`.

---

## Task 8: C Macro Test

**Files:**
- Create: `include/utils.h`
- Create: `tests/block1_2_cxsetsel_macro.c`

- [ ] **Step 1: Create `include/utils.h` with the CXSETSEL macro**

Create `/home/cmd/Github/runtime-cxtg/include/utils.h`:

```c
#ifndef UTILS_H
#define UTILS_H

/*
 * CXSETSEL rd, rs1 -- atomically swap rs1 into cxsel, return old in rd.
 *
 * Encoding: SYSTEM opcode 0x73, funct3=4 (100), funct7=0, rs2=x0
 * MATCH=0x00004073  MASK=0xFFF0707F
 *
 * Uses GAS .insn r directive -- no custom assembler required.
 */
#define CXSETSEL(rd, rs1) \
    __asm__ volatile (".insn r 0x73, 4, 0, %0, %1, x0" \
                      : "=r"(rd) : "r"(rs1))

#endif /* UTILS_H */
```

- [ ] **Step 2: Update wiki for `include/utils.h`**

Create `wiki/runtime-cxtg/include/utils.h.md`. Update `wiki/index.md`.

- [ ] **Step 3: Create `tests/block1_2_cxsetsel_macro.c`**

Create `/home/cmd/Github/runtime-cxtg/tests/block1_2_cxsetsel_macro.c`:

```c
#include "common/uart.h"
#include "common/test.h"
#include "../include/utils.h"

int main(void)
{
    unsigned long prev, cur;

    /* Case 1: swap from 0 -- prev=0, cxsel=0 */
    CXSETSEL(prev, 0UL);
    CHECK(prev == 0UL);
    __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
    CHECK(cur == 0UL);

    /* Case 2: swap in 0x0102 -- prev=0, cxsel=0x0102 */
    CXSETSEL(prev, 0x0102UL);
    CHECK(prev == 0UL);
    __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
    CHECK(cur == 0x0102UL);

    /* Case 3: swap in ~0 -- prev=0x0102, cxsel=~0 (no clamping in stub) */
    CXSETSEL(prev, ~0UL);
    CHECK(prev == 0x0102UL);
    __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
    CHECK(cur == ~0UL);

    /* Case 4: rd temp used, effectively discarding return; cxsel=0x55 */
    {
        unsigned long discard;
        CXSETSEL(discard, 0x55UL);
        __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
        CHECK(cur == 0x55UL);
        (void)discard;
    }

    /* Case 5: round-trip */
    {
        unsigned long old1, old2;
        CXSETSEL(old1, 0xABCDUL);
        CHECK(old1 == 0x55UL);
        CXSETSEL(old2, old1);
        CHECK(old2 == 0xABCDUL);
        __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
        CHECK(cur == 0x55UL);
    }

    test_pass();
    return 0;
}
```

- [ ] **Step 4: Build and run**

```bash
cd /home/cmd/Github/runtime-cxtg
make tests/block1_2_cxsetsel_macro.elf
make run-block1_2_cxsetsel_macro
```

Expected: `PASSED`

- [ ] **Step 5: Capture output**

```bash
make run-block1_2_cxsetsel_macro > tests/output/block1_2_cxsetsel_macro.out 2>&1
cat tests/output/block1_2_cxsetsel_macro.out
```

- [ ] **Step 6: Update wiki for `tests/block1_2_cxsetsel_macro.c`**

Create `wiki/runtime-cxtg/tests/block1_2_cxsetsel_macro.c.md`. Update `wiki/index.md`.

---

## Task 9: Update Progress, Commit, and Merge

**Files:**
- Modify: `docs/progress.md`

- [ ] **Step 1: Check off block 1.2 in progress.md**

In `docs/progress.md`, mark block 1.2 complete:

```markdown
- [x] **1.2** — cxsetsel instruction stub
```

- [ ] **Step 2: Ask user "Ready to commit?" — wait for confirmation**

Do not proceed until the user confirms.

- [ ] **Step 3: Commit qemu-cxtg**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg
git add target/riscv/cpu_bits.h \
        target/riscv/trace-events \
        target/riscv/insn32.decode \
        target/riscv/helper.h \
        target/riscv/cx.h \
        target/riscv/cx.c \
        target/riscv/insn_trans/trans_rvzcx.c.inc \
        target/riscv/translate.c \
        disas/riscv.c
git commit -m "zcx: add cxsetsel instruction stub (Block 1.2)"
```

- [ ] **Step 4: Commit runtime-cxtg**

```bash
cd /home/cmd/Github/runtime-cxtg
git add include/utils.h \
        tests/block1_2_cxsetsel.S \
        tests/block1_2_cxsetsel_macro.c \
        tests/output/block1_2_cxsetsel.out \
        tests/output/block1_2_cxsetsel_macro.out \
        docs/progress.md \
        wiki/
git commit -m "block1.2: cxsetsel tests, CXSETSEL macro, wiki updates"
```

- [ ] **Step 5: Merge feat/1.2 → cxtg-dev in qemu-cxtg**

```bash
cd /home/cmd/Github/runtime-cxtg/qemu-cxtg
git checkout cxtg-dev
git merge feat/1.2
```

- [ ] **Step 6: Merge feat/1.2 → cxtg-dev in runtime-cxtg**

```bash
cd /home/cmd/Github/runtime-cxtg
git checkout cxtg-dev
git merge feat/1.2
```

- [ ] **Step 7: Write release notes**

In `docs/progress.md`, under `### v0.phase1 — (in progress)`, add block 1.2 release notes immediately after the block 1.1 entries. One bullet per discrete action per file, past tense, naming every function/symbol touched. Follow the exact format of existing block 1.1 entries.

---

## Self-Review

**Spec coverage check:**

| Spec requirement | Task |
|---|---|
| `cxsetsel rd, rs1` atomic swap semantics | Tasks 4, 5 |
| SYSTEM opcode funct3=100, MATCH/MASK constants | Task 3 |
| `@r2` decodetree pattern | Task 3 |
| `DEF_HELPER_2(cxsetsel, tl, env, tl)` | Task 4 |
| `helper_cxsetsel` body (no clamping, XLEN-wide) | Task 4 |
| `trace_cxsetsel` trace event | Tasks 3, 4 |
| `trans_cxsetsel` in `trans_rvzcx.c.inc` | Task 5 |
| `do_csr_post` TB exit | Task 5 |
| `disas/riscv.c` enum + list + decode | Task 6 |
| Assembly test — 5 cases | Task 2 |
| `CXSETSEL(rd, rs1)` macro in `utils.h` | Task 8 |
| C macro test — 5 cases | Task 8 |
| Both `.out` captures | Tasks 7, 8 |
| Wiki for all new/modified files | Tasks 2–8 |
| Progress.md checked off | Task 9 |
| Release notes written | Task 9 |
| TDD order respected (S test before impl) | Tasks 2 → 3–7 |

No gaps found.

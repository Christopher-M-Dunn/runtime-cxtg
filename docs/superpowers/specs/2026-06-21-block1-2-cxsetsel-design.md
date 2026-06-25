# Block 1.2 Design — cxsetsel Instruction Stub

**Date:** 2026-06-21
**Phase:** 1 — Unpriv CSR and Instruction Scaffolding
**Block:** 1.2 — cxsetsel instruction stub
**Prerequisite:** Block 1.1 (merged)

---

## Objective

Implement `cxsetsel rd, rs1` as a QEMU-decoded instruction. The instruction atomically writes `x[rs1]` to `cxsel` and returns the previous value of `cxsel` in `rd` (if rd ≠ x0). This block is a **stub**: no WARL clamping, no mode checks. Clamping comes in block 2.3.

---

## Instruction Encoding

SYSTEM major opcode (`0x73`), funct3=`100` (unused slot — confirmed no conflict with existing QEMU instructions). funct7 and rs2 fixed at zero.

```
[31:25] funct7  = 0000000  (fixed)
[24:20] rs2     = 00000    (fixed)
[19:15] rs1                (variable)
[14:12] funct3  = 100      (fixed)
[11:7]  rd                 (variable)
[6:0]   opcode  = 1110011  (SYSTEM, fixed)
```

**insn32.decode entry:**
```
cxsetsel  0000000 00000 ..... 100 ..... 1110011  @r2
```

Uses existing `@r2` format struct (`{ rs1, rd }`) — no new arg struct needed.

**Constants for cpu_bits.h:**
```c
#define MATCH_CXSETSEL  0x00004073
#define MASK_CXSETSEL   0xFFF0707F
```

### Encoding conflict check

Zimop (`mop_r_n`, `mop_rr_n`) also uses SYSTEM funct3=100 but requires bit 31 = 1. Our funct7=0 sets bit 31 = 0. No overlap. Zimop is also off by default (`MULTI_EXT_CFG_BOOL("zimop", ext_zimop, false)`).

---

## QEMU Implementation

### Helper registration (`helper.h`)

```c
DEF_HELPER_2(cxsetsel, tl, env, tl)
```

The `DEF_HELPER_N` X-macro system generates:
- A C prototype via `helper-proto.h` (included in `cx.c`)
- An inline TCG wrapper `gen_helper_cxsetsel(dest, tcg_env, src)` via `helper-gen.h` (included in `translate.c`)

The function body is written manually.

### Helper body (`cx.c`)

```c
target_ulong helper_cxsetsel(CPURISCVState *env, target_ulong new_val)
{
    target_ulong old = env->cxsel;
    env->cxsel = new_val;
    trace_cxsetsel(env->mhartid, new_val, old);
    return old;
}
```

No clamping. `env->cxsel` accepts any value; validation is deferred to block 2.3.

**Width:** `cxsel` is XLEN-wide per spec (section 3.2). `env->cxsel` is declared as `target_ulong`, which resolves to 32 bits in RV32 builds and 64 bits in RV64 builds — correct. `tl` in `DEF_HELPER_2(cxsetsel, tl, env, tl)` maps to `target_ulong` for the same reason, so helper width tracks XLEN automatically across builds.

**RV128 note:** QEMU represents RV128 registers as two paired `TCGv` at runtime, but `target_ulong` remains 64-bit in a 64-bit QEMU build. In an RV128 hart, `get_gpr`/`gen_set_gpr` operate on the low 64 bits of the register — the same behavior as nearly all non-base extensions (only `trans_rvi.c.inc`, `trans_rvm.c.inc`, and `trans_rvzacas.c.inc` handle RV128 explicitly, because they have specific 128-bit semantics). RV128 `cxsel` semantics are unspecified by the current spec; operating on the low 64 bits is acceptable for this stub. No explicit RV128 guard needed.

### Prototype (`cx.h`)

```c
target_ulong helper_cxsetsel(CPURISCVState *env, target_ulong new_val);
```

### Trace event (`trace-events`)

```
cxsetsel(uint64_t hartid, uint64_t new_val, uint64_t old_val) "hart=%" PRId64 " new=0x%" PRIx64 " old=0x%" PRIx64
```

### Translate function (`insn_trans/trans_rvzcx.c.inc`)

New file, following the QEMU convention of one `.c.inc` per extension group.

```c
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

`do_csr_post` is a static function in `trans_rvi.c.inc`, accessible here because all `.c.inc` files are `#include`-d into the same `translate.c` translation unit. It saves the current opcode for exception unwind, advances PC, and forces a TB exit — required because `env->cxsel` affects how subsequent CX instructions execute.

### translate.c include

One line added alongside the other extension trans includes:

```c
#include "insn_trans/trans_rvzcx.c.inc"
```

---

## runtime-cxtg Side

### Macro (`include/utils.h`)

```c
/*
 * CXSETSEL rd, rs1 — atomically swap rs1 into cxsel, return old in rd.
 * Encoding: SYSTEM opcode 0x73, funct3=4, funct7=0, rs2=x0
 */
#define CXSETSEL(rd, rs1) \
    __asm__ volatile (".insn r 0x73, 4, 0, %0, %1, x0" \
                      : "=r"(rd) : "r"(rs1))
```

Uses the GAS `.insn r` directive: `.insn r opcode, funct3, funct7, rd, rs1, rs2`. No custom assembler required.

### Tests

**`tests/block1_2_cxsetsel.S`** — raw assembly, tests instruction encoding directly:

| Case | Setup | Expected |
|------|-------|----------|
| Swap from 0 | a0=0 | a1=0, cxsel=0 |
| Swap in value | a0=0x0102 | a1=0, cxsel=0x0102 |
| Swap ~0 | a0=~0 | cxsel=~0 (no clamping) |
| rd=x0 discards | a0=0x55, rd=x0 | cxsel=0x55, return discarded |
| Round-trip | swap then swap back | original value restored |

**`tests/block1_2_cxsetsel_macro.c`** — C, tests the `CXSETSEL()` macro from `utils.h`. Uses `CHECK()` from `test.h` and `uart_print_hex()` from `uart.h`, consistent with block 1.1.

Both tests go under `tests/`, captured output under `tests/output/`. Both run via the existing Makefile wildcard `run-%` target.

---

## Files Touched

### qemu-cxtg

| File | Change |
|------|--------|
| `target/riscv/cpu_bits.h` | Add `MATCH_CXSETSEL`, `MASK_CXSETSEL` |
| `target/riscv/helper.h` | Add `DEF_HELPER_2(cxsetsel, tl, env, tl)` |
| `target/riscv/cx.h` | Add `helper_cxsetsel` prototype |
| `target/riscv/cx.c` | Add `helper_cxsetsel` body + trace call |
| `target/riscv/trace-events` | Add `cxsetsel` trace event |
| `target/riscv/insn32.decode` | Add `cxsetsel` decode entry |
| `target/riscv/insn_trans/trans_rvzcx.c.inc` | New file — `trans_cxsetsel` |
| `target/riscv/translate.c` | Add `#include "insn_trans/trans_rvzcx.c.inc"` |
| `disas/riscv.c` | Add `rv_op_cxsetsel` enum, mnemonic entry, decode case (manual — not tied to decodetree) |

### runtime-cxtg

| File | Change |
|------|--------|
| `include/utils.h` | Add `CXSETSEL(rd, rs1)` macro |
| `tests/block1_2_cxsetsel.S` | New — raw assembly test |
| `tests/block1_2_cxsetsel_macro.c` | New — C macro test |
| `tests/output/block1_2_cxsetsel.out` | Captured run output |
| `tests/output/block1_2_cxsetsel_macro.out` | Captured run output |

Wiki pages created/updated for all new and modified files per project rule.

---

## Definition of Done

- All five swap semantics correct in `.S` test (including rd=x0, rs1=x0, ~0)
- `CXSETSEL` macro test passes in `.c` test
- No WARL clamping — any value accepted (validation deferred to 2.3)
- Trace event fires on each cxsetsel execution
- `make run-block1_2_cxsetsel` and `make run-block1_2_cxsetsel_macro` both print `PASSED`
- Merge `feat/1.2` → `cxtg-dev` in both repos

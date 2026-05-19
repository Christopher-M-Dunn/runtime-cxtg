# Phase 0 Audit — Block 0.1

**Date:** 2026-05-19
**Branch audited:** `qemu-cxtg:cxtg-dev` (Artur's feat/cx integration)
**QEMU version:** 10.2.90 (v11.0.0-rc0-38-ga279c0ff59)

---

## 1. Branch Integration Status

Artur's `feature/cx` commits are fully cherry-picked into `cxtg-dev`. All 10 commits
confirmed present (different hashes due to rebase onto a different upstream base, same
commit messages):

| Hash (cxtg-dev) | Commit message |
|---|---|
| `83a4000ef8` | target/riscv: add Zicx extension |
| `109b8d5cde` | target/riscv: add CSR cxsel |
| `ccdc43197d` | [DEBUG]: Trace feature string |
| `df1458f00d` | Add CLAUDE.md guidance |
| `546e86a1c3` | target/riscv: add CSR cxsetsel, cxidx and cxdata |
| `9de82f5a36` | target/riscv: add tracing options for all CX CSR registers |
| `6f9f35bd0f` | target/riscv: read/write CX CSRs from/into RISC-V CPU state |
| `02ae7f935a` | target/riscv: remove CX CSR cxsetsel and fix typos |
| `2afb472f9d` | target/riscv: add Composable Extension document |
| `a279c0ff59` | target/riscv: add Claude Code spec.md |

`feature/cx` has been retired (deleted). `feat/0.1` cut from `cxtg-dev`.

---

## 2. Device Tree CXU Registration

**Finding:** None. No DT node generation code for CX exists anywhere in `hw/riscv/`.
`grep -rn "cx\|CX" hw/riscv/` returns zero CX-related results.

**What needs to change for Block 5.3:**
- Add `cx_dt_info_t` struct to `target/riscv/cx/cx.h`
- Emit `/cx/<name>` DT nodes from machine model (`hw/riscv/virt.c`)
- Schema: `compatible = "riscv,cx-v1"; uuid; cxid; num_contexts; state_words`

---

## 3. CSR Inventory

| CSR | Address | Privilege | Reset | Read handler | Write handler |
|-----|---------|-----------|-------|--------------|---------------|
| `cxsel` | `0x800` | URO | 0 | returns `env->cxsel` | returns `RISCV_EXCP_ILLEGAL_INST` (write always traps) |
| `cxsidx` | `0x801` | URW | 0 | returns `env->cxidx` | writes `env->cxidx`; no WARL enforcement yet |
| `cxsdata` | `0x802` | URW | 0 | returns `env->cxdata` | writes `env->cxdata`; no auto-increment yet |

**CPURISCVState fields added:**
```c
target_ulong cxsel;   /* field name matches CSR name */
target_ulong cxidx;   /* note: field is cxidx not cxsidx */
target_ulong cxdata;  /* note: field is cxdata not cxsdata */
```

**Cross-check against plan:**
- cxsel: address mismatch — Artur used `0x800`, spec draft says `0xCC0`. TODO added in `cpu_bits.h`.
- cxsidx WARL (clamp to valid range): not yet implemented — Block 1.1 scope.
- cxsdata auto-increment: not yet implemented — Block 4.2 scope.
- All three CSRs registered in `csr_ops[]` in `target/riscv/csr.c` at lines 6779-6781.

---

## 4. cxsetsel Status

`cxsetsel` was initially added as a CSR by Artur and then correctly **removed** in commit
`02ae7f935a` with the note: "There is no register cxsetsel. It is a RISC-V custom instr
for the CX extension mechanism."

Confirmed absent from:
- `target/riscv/insn32.decode` — no entry
- `target/riscv/translate.c` — no handler
- `target/riscv/csr.c` — no CSR table entry

**Implication for Block 1.2:** Full implementation required — encoding (SYSTEM opcode
0x73, funct3 TBD), `translate.c` handler, `utils.h` inline asm macro.

---

## 5. Other CX Logic

| Feature | Status | First appears |
|---------|--------|--------------|
| `scxstp` | Absent | Block 2.1 |
| `scxxs0-3` | Absent | Block 3.1 |
| `cxdiscard` / `scxdiscard` | Absent | Block 4.3 |
| Indirect mode table walk | Absent | Block 6.3 |
| `scxNxs` (ZcxMulti) | Absent | Block 6.1 |
| Sscsrind wiring | Absent | Block 6.4 |
| Smstateen / Sstateen gating | Absent | Block 7.1 |
| DT CXU registration | Absent | Block 5.3 |

All of the above are expected gaps — no Phase 2+ work was attempted by Artur.

---

## 6. Extension and Field Name Discrepancies

| # | Symbol | Current (Artur) | Plan/Spec | Action |
|---|--------|-----------------|-----------|--------|
| 1 | CSR_CXSEL address | `0x800` | `0xCC0` (spec draft) | TODO comment in `cpu_bits.h`; resolve when spec finalised |
| 2 | Extension name | `zicx` / `ext_zicx` | `zcx` / `ext_zcx` | Confirm correct ISA name with spec owners; recorded in `todo.md` |
| 3 | CPURISCVState field | `cxidx`, `cxdata` | `cxsidx`, `cxsdata` | Minor; leave for now, rename in bulk when addresses finalised |
| 4 | cxsetsel | Not present as instruction | Block 1.2 | Expected gap — new work |
| 5 | Debug printf in cpu.c | Present | Should be removed | Removed in Block 0.1 (feat/0.1) |

---

## 7. Build and Boot Status

- **Binary:** `qemu-cxtg/build/qemu-system-riscv64`
- **QEMU version:** 10.2.90 (v11.0.0-rc0-38-ga279c0ff59)
- **`--version` exits 0:** YES
- **Clean build (make -j):** YES
- **Ubuntu boot:** SKIPPED — system reboot during testing; bare-metal ELF tests (Phase 1+) do not require Ubuntu
- **Compile warnings on CX files:** None observed

---

## 8. Build Flag Architecture Decision

Recorded separately in `todo.md`. Summary: only two new runtime extension flags are
needed (`ext_zcx`, `ext_zcxmulti`). No Kconfig compile-time CONFIG_* flags required.
Behavior gating uses existing QEMU runtime checks (`riscv_has_ext(env, RVS)`,
`ext_smstateen`, `ext_sscsrind`). The action plan's CONFIG_* labels are phase milestone
markers, not `#ifdef` symbols. See `todo.md` for full rationale and code pattern.

---

## 9. Phase 1 Block Scope Adjustments

Artur's work is limited to stub CSRs with no behavioral wiring. No Phase 1 blocks are
reduced in scope. All blocks proceed as written in the action plan, with one note:

- **Block 1.1** may rename `cxidx`/`cxdata` fields to `cxsidx`/`cxsdata` in
  `CPURISCVState` for consistency with CSR names — low-risk mechanical change.

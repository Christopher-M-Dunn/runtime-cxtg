# CXTG QEMU Implementation Action Plan

*Zcx & ZcxMulti incremental build plan | cxtg-dev → cxtg workflow*

---

## 1. Overview and Goals

This plan delivers a clean, incrementally testable implementation of Zcx/ZcxMulti in QEMU, a matching userspace runtime, and an automated CX registration flow. Each numbered block is a single shippable feature: bounded file set, clear objective, pass/fail tests. A block merges from cxtg-dev to cxtg only when all its tests pass.

End-state: one repo that builds correctly with any reasonable combination of Zcx M-mode, S-mode, ZcxMulti, Sscsrind, and M/S/H/UStateen.

---

## Supporting Documents

| File | Purpose |
| --- | --- |
| [`progress.md`](progress.md) | Block-by-block checklist and release notes |
| [`bugs.md`](bugs.md) | Known bugs and their status |
| [`todo.md`](todo.md) | Deferred implementation decisions: hardcoded addresses, TBD values |

**Hardcoded address rule:** Any CSR address, encoding constant, or other value that is hardcoded pending spec finalisation must be accompanied by a `// TODO: address TBD` comment at the point of use, and an entry added to `todo.md` with the symbol name, current placeholder value, and what spec section will resolve it.

---

## 2. Repository Map

`qemu-cxtg` and `linux-cxtg` live as subdirectories inside the `runtime-cxtg/` umbrella repo.

| Repo | Purpose | Key files |
| --- | --- | --- |
| `qemu-cxtg` (in `runtime-cxtg/`) | QEMU hardware model | `target/riscv/csr.c`, `target/riscv/insn32.decode`, `target/riscv/translate.c`, `target/riscv/cpu.h`, `hw/riscv/<machine>.c`, `target/riscv/cpu_bits.h` |
| `runtime-cxtg` | Userspace runtime + CX zoo + test programs | `src/ci.c`, `include/ci.h`, `include/utils.h`, `zoo/*/`, `examples/` |
| `linux-cxtg` (in `runtime-cxtg/`) | Kernel for S-mode testing *(future)* | `arch/riscv/kernel/syscalls/syscall.tbl`, `drivers/riscv/cx/` |

---

## 3. Branch and Workflow Strategy

Same model applies to both repos:

```
main          ← do not touch
cxtg          ← stable; push only when a full block passes all tests
cxtg-dev      ← integration branch; merge feature branches here
feat/<block>  ← one branch per block, cut from cxtg-dev
```

Per-block workflow:

1. Cut `feat/<block-id>` from `cxtg-dev`.
2. Implement the feature (QEMU and/or runtime).
3. Write or update the block's test programs.
4. All block tests pass on a clean QEMU boot.
5. PR `feat/<block-id>` → `cxtg-dev`; review diff; merge.
6. At phase milestone (all blocks in phase green): merge `cxtg-dev` → `cxtg` and tag (e.g. `cxtg-v0.phase2`).
7. After tagging `qemu-cxtg`: in `runtime-cxtg`, commit the updated submodule pointer on `cxtg-dev`, merge `cxtg-dev` → `cxtg`, and apply the same tag.

**Rule:** never commit directly to `cxtg` or `cxtg-dev` in `qemu-cxtg`. All code changes come through a `feat/` branch. Exception: documentation-only changes — in either repo, including `qemu-cxtg`'s `Composable-Extensions.md`, `spec.md`, and `CLAUDE.md` — go directly to `cxtg-dev`.

---

## 4. Build Configuration Matrix

Two ISA extensions are implemented: **Zcx** (base) and **ZcxMulti**. Both follow the standard QEMU RISC-V runtime extension pattern — no compile-time Kconfig flags are required.

**Runtime extension flags** (added to `cpu_cfg_fields.h.inc`, enabled via `-cpu rv64,zcx=on`):

| Flag | First Phase | Controls |
| --- | --- | --- |
| `ext_zcx` | Phase 1 | All Zcx CSRs and instructions; enables cxsel, cxsetsel, cxsidx, cxsdata, scxstp, scxxsn, cxdiscard |
| `ext_zcxmulti` | Phase 6 | ZcxMulti additions: scxNxsn (N=1–63), Indirect mode; predicate enforces `ext_zcx` and `ext_sscsrind` also set |

**Behavior gating at runtime** (no new flags — check existing QEMU extension state):

| Check | Controls |
| --- | --- |
| `riscv_has_ext(env, RVS)` | Gates S-mode CSRs (scxstp, scxxsn, scxdiscard) |
| `riscv_cpu_cfg(env)->ext_zcxmulti` | Enables per-context scxNxsn tracking and Indirect mode |
| `riscv_cpu_cfg(env)->ext_sscsrind` | Gates Sscsrind indirect access to scxNxsn (Block 6.4) |
| `riscv_cpu_cfg(env)->ext_smstateen` | Gates mstateen0.C enforcement (Block 7.1) |
| `riscv_cpu_cfg(env)->ext_ssstateen` | Gates sstateen0.C / hstateen0.C enforcement (Block 7.2) |

**Note on S-mode:** `RVS` is baked into the virt machine CPU model and is always present. The "no S-mode" case applies to M-only embedded targets using a different CPU definition. Test programs control privilege levels explicitly via `mstatus.MPP` + `mret` — no need to disable S-mode at the machine level.

**Block 8.1 test matrix** uses `-cpu` flag combinations against a single binary, not separate recompile variants.

---

## 5. Implementation Blocks

Each block is one shippable feature. Format: Objective / Prerequisites / Repos+Files / Implementation notes / Tests / Definition of Done / Merge action.

---

### Phase 0 — Foundation

#### Block 0.1 — Integrate Artur's branch, audit, build, and boot

**Objective:** Absorb Artur's starting-point work cleanly into the `cxtg-dev` workflow, audit exactly what he put in, confirm the build works, and document the baseline so Phase 1 blocks start from a known state.

> **Note:** bare-metal ELF test programs (Phase 1+) are run as separate QEMU payloads via `-M virt -bios <test.elf>` and do **not** use the Ubuntu boot target.

**Prerequisites:** None.

**Repos / Files:** `qemu-cxtg`: all files touched by `feat/cx`.

**Implementation notes:**

*Branch integration:*
- Cut `feat/0.1` from `cxtg-dev`.
- Cherry-pick (or rebase) all commits from Artur's `qemu-cxtg:feat/cx` onto `feat/0.1`.
- Confirm clean build after integration.
- Once merged to `cxtg-dev`, delete `feat/cx` to avoid confusion.

*Audit — answer each of the following and record findings in `docs/phase0_audit.md`:*
- **Device tree CXU registration:** How does Artur hardcode CXUs to be recognised? Locate the machine model DT node generation code, identify the schema (fields, compatible string, how CXID/UUID/context-count are expressed), and note what would need to change to make it data-driven later (Block 5.3).
- **CSRs present:** List every CX-related CSR added (address, privilege, reset value, any read/write handler logic). Cross-check against the plan's expected set: cxsel (0xCC0), cxsidx (0x801), cxsdata (0x802). Note any extras or gaps.
- **cxsetsel stub:** Is `cxsetsel` present in `insn32.decode` and `translate.c`? If yes, record encoding (opcode, funct3, funct7) and current behavior — this may reduce or replace Block 1.2.
- **Any other CX logic:** Note anything else present (scxstp, scxxs, discard instructions, etc.) that overlaps with later blocks, so those blocks can be scoped as "verify and extend" rather than "implement from scratch".

*After audit:*
- Define named constants for all discovered CSR addresses in `target/riscv/cpu_bits.h` if not already done.
- Draw and commit a one-page Direct mode flowchart based on actual code: hart → cxsetsel → scxstp.mode check → CXID+IDX extraction → CX dispatch. Store as `docs/flowchart_direct_mode.svg`.

**Tests:**
- `QEMU --version` exits 0.
- QEMU boots Ubuntu: reaches login prompt without errors.
- `grep` confirms CX CSR entries in `csr.c`.
- No compile errors or warnings on modified files.

**Definition of Done:** `feat/cx` retired; clean build; boots to Ubuntu login; `docs/phase0_audit.md` committed with DT schema, CSR inventory, and cxsetsel status; flowchart committed. Phase 1 block scopes adjusted if audit reveals work already done.

**Merge action:** Merge `feat/0.1` → `cxtg-dev`. Delete `qemu-cxtg:feat/cx`.

---

### Phase 1 — Unpriv CSR and Instruction Scaffolding

#### Block 1.1 — CSR stubs: cxsel, cxsidx, cxsdata

**Objective:** All three CSRs are accessible without trapping. cxsel is URO (writes trap). cxsidx and cxsdata are URW. All reset to 0.

**Prerequisites:** Block 0.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c`, `target/riscv/cpu.h` (add cx_cxsel, cx_cxsidx, cx_cxsdata to CPURISCVState).

**Implementation notes:**
- Add CSR entries with correct privilege (URO for cxsel, URW for cxsidx/cxsdata).
- cxsidx WARL: valid range 0 to (state_size-1); out-of-range writes clamp or wrap — document chosen behavior.
- No behavioral wiring yet — just storage. Behavioral enforcement added in Phase 4.

**Tests:**
- `csrr cxsel` → 0, no trap. [After reset only; cannot write via csrw]
- `csrw cxsel, <any>` → illegal instruction trap (mcause=2). [Values: 0, 1, 0xFF, ~0UL]
- `csrw cxsidx, N` then `csrr cxsidx` → N. [Values: 0, 1, 7, 15, 255]
- `csrw cxsidx` out-of-range → consistent WARL behavior; no trap.
- `csrw cxsdata, V` then `csrr cxsdata` → V. [Values: 0, 0xDEADBEEF, ~0UL]
- Test file: `tests/block1_1_csr_stubs.S`.

**Definition of Done:** All CSR access tests pass. `csrw cxsel` always traps mcause=2.

**Merge action:** Merge `feat/1.1` → `cxtg-dev`.

---

#### Block 1.2 — cxsetsel instruction stub

**Objective:** `cxsetsel rd, rs1` atomically writes rs1 to cxsel and returns old cxsel in rd. Encoded in SYSTEM opcode space.

**Prerequisites:** Block 1.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/insn32.decode`, SYSTEM opcode handler in `translate.c`, `target/riscv/cpu_bits.h` (MATCH/MASK constants). `runtime-cxtg`: `include/utils.h` (CXSETSEL inline asm macro).

**Implementation notes:**
- Encoding: SYSTEM major opcode (0x73), funct3 = available slot (TBD; define MATCH_CXSETSEL). Stub behavior: no validation — just swap.
- rd=x0: still writes cxsel; return value discarded.
- rs1=x0: writes 0 to cxsel (select builtin).
- Add `CXSETSEL(rd, rs1)` macro in `utils.h` using inline asm.

**Tests:**
- `cxsetsel a1, a0` with a0=0 → a1=0, `csrr cxsel`=0.
- `cxsetsel a1, a0` with a0=0x0102 → a1=0, `csrr cxsel`=0x0102.
- `cxsetsel a1, a0` with a0=~0UL → `csrr cxsel`=~0UL (stored as-is; validation comes in block 2.3).
- `cxsetsel x0, a0` with a0=0x55 → `csrr cxsel`=0x55 (rd=x0 discards return only).
- Back-to-back cxsetsel round-trip: values swap correctly.
- Test file: `tests/block1_2_cxsetsel.S`.

**Definition of Done:** All swap semantics correct including rd=x0, rs1=x0 edge cases.

**Merge action:** Merge `feat/1.2` → `cxtg-dev`.

---

#### Block 1.3 — Runtime wrappers (cx_select, cx_get_sel, cx_valid)

**Objective:** cx_runtime provides thin wrappers around cxsetsel/cxsel. cx_open/cx_close untouched here — addressed in Phase 5.

**Prerequisites:** Block 1.2.

**Repos / Files:** `runtime-cxtg`: `include/ci.h` (new Zcx API signatures), `include/utils.h` (macros), `src/ci.c`.

**Implementation notes:**
- Replace old `cx_sel()` (void) with `cx_select(cx_sel_t sel)` → cx_sel_t returning previous value (uses cxsetsel).
- `cx_get_sel()`: reads cxsel via csrr.
- `cx_valid(sel)`: returns false if sel == cx_sel_invalid (~0UL), else true.
- Constants: `CX_LEGACY=0`, `cx_sel_invalid=~0UL`. Keep backward-compatible `CX_LEGACY` for existing examples.

**Tests:**
- `cx_select(0)` returns prev; `cx_get_sel()==0`.
- `cx_select(0x0102)` returns prev; `cx_get_sel()==0x0102`.
- `cx_valid(0)==true`, `cx_valid(1)==true`, `cx_valid(~0UL)==false`.
- Round-trip: `prev = cx_select(new); cx_select(prev)` restores original value.
- Test file: `examples/test_wrappers.c`. Build and run with existing Makefile.

**Definition of Done:** All wrapper tests pass. cx_select/cx_get_sel/cx_valid behave correctly.

**Merge action:** Merge `feat/1.3` → `cxtg-dev`. Phase 1 milestone → tag `cxtg-v0.phase1`.

---

### Phase 2 — Direct Mode Semantics

#### Block 2.1 — scxstp CSR stub

**Objective:** scxstp (SRW) exists, resets to 0 (Disabled), is accessible from S/M-mode, and has a valid mode field (upper 4 bits) with WARL on reserved values.

**Prerequisites:** Block 1.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c` (CSR_SCXSTP, placeholder address e.g. 0x5C0), `target/riscv/cpu.h` (cx_scxstp field).

**Implementation notes:**
- Mode field = scxstp[XLEN-1:XLEN-4]. Legal: 0=Disabled, 1=Direct, 2=Indirect (Phase 6). Write 3–15 → WARL clamp to 0; document.
- Lower bits: WARL zero for Disabled/Direct; PPN field reserved until Phase 6.
- No behavioral enforcement yet. U-mode access must trap (SRW).

**Tests:**
- `csrr scxstp` (M-mode) → 0 after reset.
- `csrw scxstp, (1<<28)` then csrr → mode field=1.
- `csrw scxstp, (2<<28)` → mode=2 stored (Indirect, used in Phase 6).
- `csrw scxstp, (15<<28)` → WARL; mode=0 or documented clamped value; no trap.
- `csrr scxstp` from U-mode → illegal instruction trap.
- Test file: `tests/block2_1_scxstp.S`.

**Definition of Done:** scxstp r/w correct from M-mode. U-mode traps. WARL behavior on mode field documented.

**Merge action:** Merge `feat/2.1` → `cxtg-dev`.

---

#### Block 2.2 — scxstp.mode=0 (Disabled) enforcement

**Objective:** When scxstp.mode=Disabled, any custom-0..3 instruction and cxsetsel raise an illegal instruction exception.

**Prerequisites:** Block 2.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/translate.c` (check scxstp.mode on decode of CUSTOM_0..3 and cxsetsel).

**Implementation notes:**
- On CUSTOM_0..3 decode: read scxstp.mode; if Disabled → RISCV_EXCP_ILLEGAL_INST.
- cxsetsel in Disabled mode also traps.
- Default after reset is Disabled — update all Phase 1 tests to first set scxstp=Direct from M-mode.

**Tests:**
- Reset: custom-0 instruction → mcause=2.
- Explicit scxstp=0: same trap.
- scxstp=(1<<28) (Direct): custom-0 with cxsel=0 → no trap.
- cxsetsel in Disabled mode → mcause=2.
- Test file: `tests/block2_2_disabled.S`. Verify Phase 1 tests still pass after adding scxstp=Direct setup.

**Definition of Done:** Disabled mode traps on all CX and cxsetsel. Direct mode does not trap.

**Merge action:** Merge `feat/2.2` → `cxtg-dev`.

---

#### Block 2.3 — Direct mode: cxsel=0 builtin / invalid trap / unregistered trap

**Objective:** In Direct mode: cxsel=0 → builtin; cxsel=~0UL → illegal instruction; cxsel with unregistered CXID → illegal instruction.

**Prerequisites:** Block 2.2.

**Repos / Files:** `qemu-cxtg`: `target/riscv/translate.c`, `target/riscv/cpu_bits.h` (CX_SEL_INVALID=~0UL), helper functions `riscv_cx_cxid(env)` and `riscv_cx_idx(env)`.

**Implementation notes:**
- `CX_SEL_INVALID = ~0UL`. On CX instruction in Direct mode: if cxsel==CX_SEL_INVALID → trap.
- cxsel=0 → execute as builtin (normal custom instruction or NOP).
- Any other value: extract CXID=cxsel[15:8], IDX=cxsel[7:0]. No CX registered → trap. Real dispatch wired in Phase 4.

**Tests:**
- scxstp=Direct, cxsel=0, run custom-0 → no trap.
- scxstp=Direct, cxsel=~0UL → illegal instruction trap.
- scxstp=Direct, cxsel=0x0100 (CXID=1, not yet registered) → illegal instruction trap.
- Invalid values to test: ~0UL, 0xFFFF0000, 0xFF (all-ones subfields).
- Test file: `tests/block2_3_direct_routing.S`.

**Definition of Done:** cxsel=0 no-trap, invalid trap, unregistered trap all verified.

**Merge action:** Merge `feat/2.3` → `cxtg-dev`. Phase 2 milestone → merge `cxtg-dev` → `cxtg`, tag `cxtg-v0.phase2`.

---

### Phase 3 — Context Status (scxxs0-3)

#### Block 3.1 — scxxs0-3 CSR stubs

**Objective:** Four SRW CSRs hold 2-bit XS status per CX ID (up to 64 IDs in RV32). All reset to Off (0b00).

**Prerequisites:** Block 2.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c` (CSR_SCXXS0..3), `target/riscv/cpu.h` (`cx_scxxs[4]` array). Helper macros `SCXXS_GET(env, cxid)` and `SCXXS_SET(env, cxid, val)`.

**Implementation notes:**
- CX ID N is in scxxs[N/16], bits [2*(N%16)+1 : 2*(N%16)]. Values: 00=Off, 01=Initial, 10=Clean, 11=Dirty.
- RV64: scxxs1 and scxxs3 (odd registers) absent/zero. Writes to them are ignored.
- No behavioral enforcement yet. Helper macros used by later blocks.
- Draw and commit the XS state machine diagram showing all transitions (Off↔Initial, Initial→Dirty, Dirty→Clean, Clean→Dirty, any→Off with state preserved). Store as `docs/flowchart_xs_states.svg`.

**Tests:**
- `csrr scxxs0..3` → 0 after reset.
- `csrw scxxs0, 0b11` (CXID=0 Dirty); csrr → lower 2 bits = 0b11.
- Field isolation: setting CXID=5 does not affect CXID=4 or CXID=6.
- CX IDs to test: 0, 1, 7, 15 (scxxs0); 16, 31 (scxxs1); 32, 47 (scxxs2); 48, 63 (scxxs3).
- U-mode csrr/csrw → illegal instruction trap.
- Test file: `tests/block3_1_scxxs_stubs.S`.

**Definition of Done:** All four CSRs r/w correct. Field isolation verified. U-mode traps. XS state machine flowchart committed.

**Merge action:** Merge `feat/3.1` → `cxtg-dev`.

---

#### Block 3.2 — scxxs Off-state enforcement

**Objective:** When scxxs bits for a CX ID are Off (0b00), any CX instruction targeting that CXID raises an illegal instruction exception.

> **Spec clarification (Guy Lemieux, May 2026):** Off status **blocks access** but hardware must **maintain CXU state** (not discard it). This enables efficient OS thread/process switching — the OS sets a thread's CX to Off to deny access without destroying state, so state does not need to be reloaded when access is later restored. Do **not** zero or invalidate state on the Off transition.

**Prerequisites:** Blocks 3.1 + 2.3.

**Repos / Files:** `qemu-cxtg`: `target/riscv/translate.c` (add XS check after CXID extraction in Direct mode).

**Implementation notes:**
- After extracting CXID from cxsel in Direct mode: `SCXXS_GET(env, cxid)`. If Off → illegal instruction.
- CXID=0 (builtin, cxsel=0) is exempt from scxxs enforcement.
- Supervisor must explicitly set XS >= Initial before a CX is usable.
- Draw and commit an OS context switch sequence diagram showing how the OS uses scxxs to protect CX state across thread switches: thread A running (Dirty) → OS sets Off (state preserved in hardware) → thread B runs → OS restores A (sets Initial/Dirty, no reload needed). Store as `docs/flowchart_os_context_switch.svg`.

**Tests:**
- scxxs all zeros, cxsel=0x0100 (CXID=1) → illegal instruction trap.
- `csrw scxxs0` to set CXID=1 = Initial (0b01); cxsel=0x0100 → no trap (routing attempted).
- Verify CXID=0 (cxsel=0) never gated by scxxs.
- All four XS values for one CXID: only Off (0b00) traps.
- Test file: `tests/block3_2_xs_off.S`.

**Definition of Done:** Off state traps reliably. Non-Off states do not trap on CX instructions. OS context switch flowchart committed.

**Merge action:** Merge `feat/3.2` → `cxtg-dev`.

---

#### Block 3.3 — scxxs dirty tracking

**Objective:** Hardware sets XS=Dirty when a CX state write occurs (cxsdata write). cxdiscard sets XS=Initial.

**Prerequisites:** Blocks 3.2 + 4.2 (can be stubbed as "Dirty on any cxsdata write" before 4.2 is done).

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c` (cxsdata write handler calls `SCXXS_SET(Dirty)`), insn handler for cxdiscard.

**Implementation notes:**
- On cxsdata CSR write: `SCXXS_SET(env, current_cxid, DIRTY)`.
- scxdiscard (Phase 4.3): `SCXXS_SET(env, target_cxid, INITIAL)`; state value UNSPECIFIED.
- Clean transition is implementation-defined; simplest is "remains Dirty until supervisor explicitly sets Clean". Document.

**Tests:**
- Write cxsdata with scxxs=Initial → `csrr scxxs` for that CXID = Dirty.
- cxdiscard → XS = Initial.
- Dirty bit does not spread to adjacent CXID.
- Test file: `tests/block3_3_dirty_track.S`.

**Definition of Done:** XS transitions Initial→Dirty on cxsdata write. cxdiscard→Initial. Field isolation verified.

**Merge action:** Merge `feat/3.3` → `cxtg-dev`. Phase 3 milestone → merge `cxtg-dev` → `cxtg`, tag `cxtg-v0.phase3`.

---

### Phase 4 — CX State Access: First CX and cxsidx/cxsdata

#### Block 4.1 — Hardcode first CX (mulacc) in QEMU

**Objective:** QEMU contains a mulacc CX object at CXID=1 with 2 contexts. cxsetsel with CXID=1 routes mulacc custom instructions to it. State storage is per context.

**Prerequisites:** Block 2.3.

**Repos / Files:** `qemu-cxtg`: `target/riscv/cx/` (new dir: `cx_mulacc.c`, `cx_mulacc.h`, `cx.h` interface), `target/riscv/translate.c` (CXID=1 dispatch), `target/riscv/cpu.h` (cx_state per context).

**Implementation notes:**
- Define CX object interface in `cx.h`: `{ uint32_t cxid; uint32_t num_contexts; uint32_t state_words; uint32_t **state; fn_ptr execute; }`.
- mulacc state: one uint32_t accumulator per context (state_words=1). Start with 2 contexts (IDX=0 and IDX=1).
- On custom-0 with mulacc opcode and CXID=1: call `cx_mulacc_execute(env, insn)`.
- IDX >= num_contexts → illegal instruction trap.
- Lazy state allocation: allocate on first access.

**Tests:**
- scxstp=Direct, scxxs CXID=1=Initial, cxsel=0x0100 (CXID=1,IDX=0), mac(3,5) → result=15.
- cxsel=0x0101 (IDX=1): mac(3,5)→15; then on IDX=0: mac(2,2)→19. IDX=1 still at 15.
- cxsel=0x0102 (IDX=2, out of range) → illegal instruction trap.
- Test file: `tests/block4_1_mulacc.c` using CXSETSEL macro.

**Definition of Done:** mulacc computes correctly on two independent contexts. Invalid IDX traps.

**Merge action:** Merge `feat/4.1` → `cxtg-dev`.

---

#### Block 4.2 — cxsidx + cxsdata wired to CX state

**Objective:** `csrw cxsidx` sets the word index into the active CX context state. `csrr`/`csrw cxsdata` reads/writes that word and auto-increments cxsidx.

**Prerequisites:** Block 4.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c` (cxsdata read/write handlers access CX state array), `target/riscv/cpu.h`.

**Implementation notes:**
- cxsdata read: return `state[cxid][idx][cxsidx]`; then cxsidx++.
- cxsdata write: `state[cxid][idx][cxsidx] = val`; cxsidx++; `SCXXS_SET(Dirty)`.
- cxsidx wraps at state_words (WARL). After cxsetsel, cxsidx is UNDEFINED — software must reset it.
- cxsidx >= state_words on access: trap or WARL; document and test.

**Tests:**
- `csrw cxsidx,0; csrw cxsdata, 0xABCD; csrw cxsidx,0; csrr cxsdata` → 0xABCD.
- Auto-increment: `csrw cxsidx,0`; write 1,2,3; `csrw cxsidx,0`; read → 1,2,3.
- After cxsetsel to different CXID: software resets cxsidx before reading/writing.
- Full save/restore round-trip: save all words of IDX=0; restore to IDX=1; verify identity.
- Test file: `tests/block4_2_cxsdata.c`.

**Definition of Done:** Read/write/auto-increment correct. Save/restore round-trip passes.

**Merge action:** Merge `feat/4.2` → `cxtg-dev`.

---

#### Block 4.3 — cxdiscard and scxdiscard

**Objective:** cxdiscard (U-mode): sets XS=Initial for current CX context; state UNSPECIFIED. scxdiscard (S-mode): same for any specified context.

**Prerequisites:** Blocks 3.3 + 4.2.

**Repos / Files:** `qemu-cxtg`: `target/riscv/insn32.decode` (cxdiscard, scxdiscard), insn handler.

**Implementation notes:**
- cxdiscard: CXID+IDX from cxsel; `SCXXS_SET(Initial)`. State value left unchanged (UNSPECIFIED per spec; simplest correct implementation).
- scxdiscard: operand is selector (CXID+IDX explicit); S-mode only. Invalid selector → trap.
- ABI use: call cxdiscard before returning from CX-using function to avoid unnecessary state save.

**Tests:**
- Write known value; cxdiscard; `csrr scxxs` → XS=Initial for that CXID.
- cxdiscard from U-mode → no trap. scxdiscard from U-mode → trap.
- scxdiscard from S-mode with valid selector → XS=Initial.
- scxdiscard with invalid CXID or out-of-range IDX → illegal instruction trap.
- Test file: `tests/block4_3_cxdiscard.S`.

**Definition of Done:** Discard sets XS=Initial. Privilege enforcement correct. Invalid selector traps.

**Merge action:** Merge `feat/4.3` → `cxtg-dev`. Phase 4 milestone → merge `cxtg-dev` → `cxtg`, tag `cxtg-v0.phase4`.

---

### Phase 5 — Runtime, API, and Device Tree

#### Block 5.1 — New Zcx runtime API stubs

**Objective:** `cx_open(uuid, flags)`, `cx_close(sel)`, `cx_valid(sel)`, `cx_get_sel()`, `cx_select(sel)`, `cx_save(buf,size)`, `cx_restore(buf,size)` exist with stub bodies. No OS interaction yet.

**Prerequisites:** Block 1.3.

**Repos / Files:** `runtime-cxtg`: `include/ci.h` (new signatures), `src/ci.c` (stub bodies).

**Implementation notes:**
- cx_open: hardcoded UUID→CXID table for now (UUID_MULACC→1). IDX allocated from a simple counter. Returns cx_sel_invalid on unknown UUID or no free IDX.
- cx_open_flags: cx_open_shared=0, cx_open_exclusive=1, cx_open_ucontext=2. Stub: all allocate a new IDX.
- cx_save / cx_restore: loop over cxsdata words. Reset cxsidx before each.
- cx_close: free IDX slot (decrement counter). No-op for now.

**Tests:**
- `cx_open(UUID_MULACC, cx_open_exclusive) > 0`.
- `cx_valid(result) == true`. `cx_valid(cx_sel_invalid) == false`.
- `cx_select(result)` returns CX_LEGACY; `cx_get_sel() == result`.
- cx_open with unknown UUID → cx_sel_invalid.
- cx_save + cx_restore round-trip: write known state; save; write different values; restore; verify original.
- Test file: `examples/test_new_api.c`.

**Definition of Done:** All API stubs callable. cx_open/close/select/save/restore functional with mulacc.

**Merge action:** Merge `feat/5.1` → `cxtg-dev`.

---

#### Block 5.2 — End-to-end smoke test program

**Objective:** A single program exercises the full Phase 1–5 stack: open two mulacc contexts, compute, save/restore, close.

**Prerequisites:** Block 5.1 + 4.2.

**Repos / Files:** `runtime-cxtg`: `examples/zcx_mulacc_test.c`.

**Implementation notes:**
- Mirrors `cx_open.c` / `state_test.c` logic but uses the new Zcx API throughout.
- Demonstrates: open, select, compute, save, switch, compute, restore, verify, close.
- This is the green smoke test for all Phase 1–5 work together.

**Tests:**
- Two `cx_open(UUID_MULACC, cx_open_exclusive)` succeed; return different selectors.
- mac(3,5) on ctx0 → 15; mac(3,5) on ctx1 → 15 (independent).
- mac(2,2) on ctx0 → 19; ctx1 still at 15.
- cx_save ctx0; cxdiscard ctx0; cx_restore ctx0; mac(1,1) → 20 (accumulates from saved 19).
- cx_close both; cx_open again succeeds (slots reused).
- `make run-zcx_mulacc_test` exits 0 and prints PASSED.

**Definition of Done:** End-to-end test prints PASSED and exits 0.

**Merge action:** Merge `feat/5.2` → `cxtg-dev`.

---

#### Block 5.3 — Device tree CX registration

**Objective:** QEMU machine model emits a DT node for each registered CX (UUID, CXID, context count, state size). Runtime can parse it.

**Prerequisites:** Block 4.1.

**Repos / Files:** `qemu-cxtg`: `hw/riscv/<machine>.c` (DT node generation), `target/riscv/cx/cx.h` (add cx_dt_info_t struct).

**Implementation notes:**
- DT node: `/cx/<name> { compatible = "riscv,cx-v1"; uuid = <128-bit>; cxid = <N>; num_contexts = <M>; state_words = <W>; };`
- Machine model iterates registered CX objects and emits one node each.
- Keep hardcoded for now; YAML automation in 5.5.

**Tests:**
- Boot with `-dtb` dump; verify `/cx/mulacc` node present with correct uuid, cxid, num_contexts, state_words.
- Add muldiv stub as CXID=2; verify two nodes appear.
- Parse DT from bare-metal C using libfdt; assert field values.
- Test file: `tests/block5_3_dt_parse.c`.

**Definition of Done:** DT nodes present and parseable for all registered CXs.

**Merge action:** Merge `feat/5.3` → `cxtg-dev`.

---

#### Block 5.4 — Runtime cx_open DT query (UUID → CXID)

**Objective:** `cx_open()` resolves UUID to CXID by walking the DT at first call. Hardcoded table removed.

**Prerequisites:** Blocks 5.1 + 5.3.

**Repos / Files:** `runtime-cxtg`: `src/ci.c` (replace hardcoded table with DT walk), `src/dt.c` (new: DT query helpers).

**Implementation notes:**
- On first cx_open: locate DT (firmware pointer in bare-metal; `/proc/device-tree` under Linux). Walk `/cx/*` nodes matching uuid field.
- Cache: UUID→CXID, up to 64 entries.
- cx_open_shared: if UUID already open in this thread, return same selector (reference count).
- cx_open_exclusive: always allocate a new IDX.
- Draw and commit a cx_open flow diagram: call entry → DT walk → UUID match → cache hit/miss path → IDX allocation → shared vs. exclusive branch → return selector or cx_sel_invalid. Store as `docs/flowchart_cx_open.svg`.

**Tests:**
- `cx_open(UUID_MULACC, cx_open_exclusive)` on bare metal resolves CXID=1 from DT; returns valid selector.
- cx_open with unknown UUID → cx_sel_invalid.
- cx_open_shared called twice → same selector returned both times.
- cx_open_exclusive called twice → two different IDX values.
- Smoke test (5.2) still passes without any hardcoded UUID table.
- Test file: `examples/zcx_mulacc_test.c` updated; still prints PASSED.

**Definition of Done:** DT query works. Shared/exclusive flag handled. Smoke test still passes. cx_open flowchart committed.

**Merge action:** Merge `feat/5.4` → `cxtg-dev`.

---

#### Block 5.5 — YAML-driven DT and CX auto-registration *(LOW PRIORITY / NICE-TO-HAVE)*

**Objective:** A YAML file per CX drives DT node generation and QEMU registration. `make cx <name>` adds a CX without recompiling QEMU.

**Prerequisites:** Block 5.3.

**Repos / Files:** `qemu-cxtg`: `scripts/gen_cx_dt.py` (new), `target/riscv/cx/cx_registry.c`. `runtime-cxtg`: `zoo/mulacc/mulacc.yaml` (new), `zoo/muldiv/muldiv.yaml`.

**Implementation notes:**
- YAML schema: name, uuid (128-bit hex), cxid, num_contexts, state_words, custom_opcode_space (0–3), insn_patterns (match/mask list).
- `gen_cx_dt.py`: reads `zoo/**/*.yaml`; emits DT overlay (`.dtso`) + C registration stub (`cx_<name>_gen.c`).
- `make cx <name>`: runs gen_cx_dt.py for that CX; no CPU recompile. Overlay loaded at QEMU boot via `-dtbo` or baked via `make cx-bake`.
- `make cx-remake`: regenerates all CX DT fragments.
- Stretch goal: QEMU scans `zoo/**/*.yaml` at startup and dlopen()s CX plugins; no make step required.

**Tests:**
- `make cx mulacc`: generates fragment; QEMU boot shows `/cx/mulacc` node.
- `make cx muldiv`: both nodes appear.
- Remove `mulacc.yaml`; `make cx-remake`; mulacc node absent.
- `cx_open(UUID_MULACC)` still resolves via DT.
- Test file: `scripts/test_yaml_gen.sh` (build, boot, dump DT, grep nodes).

**Definition of Done:** YAML pipeline works. New CX needs only a YAML file + `make cx <name>`.

**Merge action:** Merge `feat/5.5` → `cxtg-dev`. Phase 5 milestone → merge `cxtg-dev` → `cxtg`, tag `cxtg-v0.phase5`.

---

### Phase 6 — ZcxMulti: Per-context Status and Indirect Mode

#### Block 6.1 — scxNxs0/1 CSR stubs (N=1-63)

**Objective:** scxNxs registers provide 2-bit XS per context index within each CX ID. When ZcxMulti present, scxxsN bits for CX IDs > 0 become read-only max-status summaries.

**Prerequisites:** Block 3.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c`, `target/riscv/cpu.h` (`uint8_t cx_ctx_xs[64][MAX_IDX]`), helpers `CX_CTX_XS_GET/SET`.

**Implementation notes:**
- Represent internally as a 2D array (64 CX IDs × MAX_IDX contexts). Expose scx1xs0 and scx2xs0 as concrete CSR addresses for now; remainder via Sscsrind (block 6.4).
- When ZcxMulti present: scxxsN bits for CX IDs > 0 become RO = max(XS across all contexts).
- scx0xs (CXID=0, builtin) not defined.

**Tests:**
- `csrw scx1xs0` to set ctx0 of CXID=1 to Dirty; `csrr scx1xs0` → correct bits.
- `csrr scxxs0`: CXID=1 field = Dirty (max summary).
- Set ctx0=Dirty, ctx1=Clean: scxxs summary = Dirty.
- Set both Off: scxxs summary = Off.
- CX IDs: 1, 2, 32, 63. Context indices: 0, 1, 15, 31.
- Test file: `tests/block6_1_scxnxs_stubs.S`.

**Definition of Done:** scxNxs readable/writable. Summary bits in scxxsN are correct read-only max values.

**Merge action:** Merge `feat/6.1` → `cxtg-dev`.

---

#### Block 6.2 — Per-context dirty tracking via scxNxs

**Objective:** Hardware updates scxNxs[cxid][idx] on cxsdata write and cxdiscard. This replaces the coarser per-CX tracking from block 3.3.

**Prerequisites:** Blocks 6.1 + 4.2.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c` (cxsdata write handler updates `CX_CTX_XS_SET` instead of `SCXXS_SET`), cxdiscard handler.

**Implementation notes:**
- On cxsdata write: `CX_CTX_XS_SET(env, cxid, idx, DIRTY)`; then update scxxs summary.
- cxdiscard: `CX_CTX_XS_SET(env, cxid, idx, INITIAL)`; update summary.

**Tests:**
- Open CXID=1 IDX=0 and IDX=1. Write IDX=0 state → scx1xs0 IDX=0=Dirty, IDX=1=Initial.
- Write IDX=1 → both Dirty. scxxs0 CXID=1 summary = Dirty.
- cxdiscard IDX=0 → IDX=0=Initial, IDX=1=Dirty. Summary still Dirty.
- cxdiscard IDX=1 → both Initial. Summary = Initial.
- Test file: `tests/block6_2_ctx_dirty.c`.

**Definition of Done:** Per-context XS tracking correct. scxxs summary always reflects max across all contexts.

**Merge action:** Merge `feat/6.2` → `cxtg-dev`.

---

#### Block 6.3 — Indirect mode (scxstp.mode=2)

**Objective:** When scxstp.mode=Indirect, cxsel is a 0–1023 index into a 4 KiB OS-managed translation table (PPN in scxstp). Each entry resolves to a real \<CXID, IDX\>.

**Prerequisites:** Blocks 2.3 + 6.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c` (scxstp PPN field), `target/riscv/translate.c` (table walk on CX instruction decode).

**Implementation notes:**
- Table entry (32-bit): V(1) | reserved(15) | IDX(8) | CXID(8). Table = 1024 entries = 4 KiB.
- On CX instruction in Indirect mode: physical read at `(scxstp.PPN << 12) + cxsel*4`. Check V bit; extract CXID+IDX; proceed as Direct.
- V=0 or invalid CXID/IDX → illegal instruction trap.
- scxstp.mode=2 when `ext_zcxmulti` is absent: resolved by WARL — clamp to mode=1 (Direct) on write, with all other bits = 0; any write with a reserved mode (3–15) is ignored. Implementation choice. Spec just indicates it is illegal. See `Composable-Extensions.md` Discussion.
- Draw and commit an Indirect mode table walk flowchart: CX instruction decode → read scxstp.mode=Indirect → compute table address → physical read → V-bit check → CXID/IDX extraction → proceed as Direct or trap. Store as `docs/flowchart_indirect_mode.svg`.

**Tests:**
- M-mode: 4 KiB aligned buffer; entry[5] = V=1,CXID=1,IDX=0; scxstp=Indirect+PPN; cxsel=5; mac(3,5)→15.
- Entry with V=0: illegal instruction trap.
- CXID pointing to unregistered CX: illegal instruction trap.
- cxsel >= 1024: WARL behavior documented and tested.
- Switch back to Direct mode; Direct tests still pass.
- Test file: `tests/block6_3_indirect.c`.

**Definition of Done:** Indirect mode table walk works. V=0 and invalid entries trap. Direct/Indirect switching verified. Indirect mode flowchart committed.

**Merge action:** Merge `feat/6.3` → `cxtg-dev`.

---

#### Block 6.4 — Sscsrind for scxNxs

**Objective:** scxNxs registers accessible via Sscsrind indirect CSR mechanism, eliminating the need for up to 126 concrete CSR addresses.

**Prerequisites:** Block 6.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c` (Sscsrind selector + data CSRs for CX domain). Note: defer until Sscsrind spec stabilizes.

**Implementation notes:**
- Follow RISC-V Sscsrind: define CX domain indirect CSR addresses.
- scx1xs0 / scx2xs0 from block 6.1 become accessible via indirect path; direct addresses become aliases.
- All 63 CX IDs addressable without 63 unique concrete CSR addresses.

**Tests:**
- Set indirect selector to scx1xs0 address; read indirect data → same as direct `csrr scx1xs0`.
- Write via indirect path; direct read confirms new value.
- 63 CX IDs all addressable; no CSR aliasing conflicts.
- Test file: `tests/block6_4_sscsrind.S`.

**Definition of Done:** All scxNxs accessible via Sscsrind. Direct aliases consistent.

**Merge action:** Merge `feat/6.4` → `cxtg-dev`. Phase 6 milestone → merge `cxtg-dev` → `cxtg`, tag `cxtg-v0.phase6`.

---

### Phase 7 — Stateen Integration

#### Block 7.1 — mstateen0.C bit

**Objective:** When mstateen0.C=0, U-mode and S-mode access to CX CSRs and instructions raises an illegal instruction exception. M-mode is unaffected.

**Prerequisites:** Block 2.2.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c` (mstateen0 CX bit check in CX CSR read/write and decode path).

**Implementation notes:**
- Add CX bit to mstateen0 (bit position per Smstateen spec).
- When bit=0: cxsel, cxsidx, cxsdata, cxsetsel, cxdiscard → illegal instruction from U or S mode.
- M-mode always has access regardless.

**Tests:**
- mstateen0.C=0 (default): U-mode `csrr cxsel` → trap; S-mode `csrr cxsel` → trap.
- mstateen0.C=1: U-mode `csrr cxsel` → 0; S-mode works normally.
- M-mode accesses always succeed.
- Test file: `tests/block7_1_mstateen.S`.

**Definition of Done:** mstateen0.C correctly gates CX access at U and S privilege levels.

**Merge action:** Merge `feat/7.1` → `cxtg-dev`.

---

#### Block 7.2 — sstateen0.C and hstateen0.C

**Objective:** sstateen0.C gates U-mode CX access from the supervisor perspective. hstateen0.C gates VS-mode CX access.

**Prerequisites:** Block 7.1.

**Repos / Files:** `qemu-cxtg`: `target/riscv/csr.c` (sstateen0, hstateen0 CX bit checks). Requires H-extension in QEMU build for hstateen.

**Implementation notes:**
- sstateen0.C=0: deny U-mode CX even if mstateen0.C=1.
- hstateen0.C=0: deny VS-mode CX in hypervisor context.
- Check order: mstateen0 → hstateen0 (VS-mode) → sstateen0.

**Tests:**
- mstateen0.C=1, sstateen0.C=0: U-mode `csrr cxsel` → trap.
- mstateen0.C=1, sstateen0.C=1: U-mode `csrr cxsel` → 0.
- hstateen0.C=0 (H-ext enabled): VS-mode `csrr cxsel` → trap.
- Test file: `tests/block7_2_sstateen.S`.

**Definition of Done:** Three-level stateen gating works. Each level independently blocks CX access.

**Merge action:** Merge `feat/7.2` → `cxtg-dev`. Phase 7 milestone → merge `cxtg-dev` → `cxtg`, tag `cxtg-v0.phase7`.

---

### Phase 8 — Build Matrix and Full Integration

#### Block 8.1 — Runtime extension matrix

**Objective:** Codebase compiles without errors or warnings, and behaves correctly for every combination of runtime `-cpu` extension flags.

**Prerequisites:** All previous blocks.

**Repos / Files:** `runtime-cxtg`: `scripts/test_matrix.sh` (new), QEMU launch wrapper.

**Implementation notes:**
- All CX code is always compiled in; features are gated at runtime via `ext_zcx` / `ext_zcxmulti` predicates.
- Dependency enforced at runtime: ZcxMulti predicate checks `ext_zcx` is also set.
- `ext_zcx=off` (default): no CX CSRs visible; all custom instrs behave as plain custom instructions.

**Tests:**
- `test_matrix.sh` iterates `-cpu` flag combinations and runs applicable block tests for each; exits 0 if all pass.
- `zcx=on` only: cxsel/cxsetsel/cxsidx/cxsdata/scxstp/scxxsn present; scxNxsn absent; Indirect mode traps.
- `zcx=on,zcxmulti=on,sscsrind=on`: scxNxsn accessible; Indirect mode functional. (ZcxMulti requires Sscsrind per spec.)
- `zcx=on,smstateen=on`: mstateen0.C enforcement active.
- `zcx=on,sscsrind=on,zcxmulti=on`: Sscsrind access to scxNxsn functional.
- `zcx=off` (default): no CX registers; all block tests correctly skipped or report SKIP.
- Test: `scripts/test_matrix.sh` exits 0.

**Definition of Done:** All `-cpu` flag combinations produce correct behavior. No regressions when adding flags.

**Merge action:** Merge `feat/8.1` → `cxtg-dev`.

---

#### Block 8.2 — Full integration test suite

**Objective:** `make test` runs all block test programs against all relevant CONFIG combinations and reports a pass/fail matrix.

**Prerequisites:** Block 8.1.

**Repos / Files:** `runtime-cxtg`: `tests/run_all.sh` (new), QEMU launch wrapper script.

**Implementation notes:**
- Each test program prints PASSED or FAILED on last line; exits 0 or 1.
- `run_all.sh`: for each `-cpu` flag combination, run all applicable tests, collect matrix.
- Tests tagged by minimum extension requirement (e.g. block4_3 requires `zcx=on`).

**Tests:**
- All block tests pass on full config (all flags enabled).
- Block 1.x tests pass with only `zcx=on`.
- No test regresses when additional `-cpu` extensions are enabled.
- `make test` exits 0 for full config.

**Definition of Done:** `make test` exits 0. No regressions across any tested config combination.

**Merge action:** Merge `feat/8.2` → `cxtg-dev` → `cxtg`. Tag `cxtg-v1.0`.

---

## 6. Testing Infrastructure Notes

All bare-metal tests follow this pattern (run as ELF payloads via `qemu-system-riscv64 -M virt -bios <test.elf> -nographic -serial stdio -no-reboot`, not via the Ubuntu boot target):

```
1. M-mode startup: set mtvec to a known trap handler that records mcause.
2. Configure CX: csrw mstateen0 (C=1); csrw scxstp (Direct mode); csrw scxxs as needed.
3. Exercise the feature; check results against expected values.
4. For expected traps: verify mcause == 2 (illegal instruction) in trap handler.
5. Print PASSED / FAILED; call sbi_shutdown() or spin.
```

Standard value ranges to cover in every CSR test:
- Zero (`0x00000000`)
- One (`0x00000001`)
- Walking ones: `0x1, 0x2, 0x4 ... 0x80000000`
- All ones: `0xFFFFFFFF` (RV32) / `0xFFFFFFFFFFFFFFFF` (RV64)
- Domain-specific boundaries: CXID=0 (builtin), CXID=1 (first CX), CXID=63 (last valid), CXID=64 (out-of-range); IDX=0, IDX=1, IDX=num_contexts-1, IDX=num_contexts (out-of-range)

S-mode tests (Phase 3+): minimal OpenSBI + small supervisor payload. Linux not required until Phase 5+ runtime DT query.

---

## 7. Block Dependency Summary

```
0.1
 └─ 1.1 ──────────────────────────────┐
     ├─ 1.2 → 1.3 → 5.1 → 5.2 → 5.4  │
     ├─ 2.1                            │
     │   ├─ 2.2 → 2.3 ────→ 4.1 ──────┤
     │   └─ 3.1 → 3.2       │          │
     │       │     └─ 3.3◄──┤          │
     │       └─ 6.1 → 6.2◄──┘ (4.2)   │
     │           └─ 6.3 (Indirect)      │
     │           └─ 6.4 (Sscsrind)      │
     └─ 5.3 → 5.4 → 5.5 (low priority) │
                                        │
 2.2 → 7.1 → 7.2                       │
 4.1 → 4.2 → 4.3                       │
                                        ▼
 All → 8.1 → 8.2 (v1.0)
```

---

*Sources: `LOCALFILES/CXTG MEETING/src/*.adoc` (isa-unpriv, isa-priv, isa-state, api, cxsel-format), `include/utils.h`, `include/ci.h`, `src/ci.c`, `zoo/*/`.*

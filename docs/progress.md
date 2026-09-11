# CXTG QEMU Implementation — Progress

See [`CXTG_QEMU_Action_Plan.md`](CXTG_QEMU_Action_Plan.md) for full block specs.
See [`ADR_log.md`](ADR_log.md) for known bugs, deferred implementation decisions, and process notes | [`CHANGELOG.md`](CHANGELOG.md) for the change log (previously referred to as release notes).

---

## Phase 0 — Foundation

- [x] implementation plan written — `docs/superpowers/plans/2026-05-19-phase0-block0.1-integrate-audit-build.md`

- [x] **0.1** — Integrate Artur's branch, audit, build, and boot
  - [x] `feat/cx` cherry-picked into `cxtg-dev`; `feat/cx` deleted
  - [x] `docs/phase0_audit.md` committed (DT schema, CSR inventory, cxsetsel status)
  - [x] `docs/flowchart_direct_mode.svg` committed
  - [x] Clean build (QEMU 10.2.90); Ubuntu boot skipped (not required for bare-metal ELF tests)

---

## Phase 1 — Unpriv CSR and Instruction Scaffolding

- [x] implementation plan written — `docs/superpowers/specs/2026-05-22-phase1-csr-instruction-scaffolding-design.md`

- [x] **test-infra** — Bare-metal test harness: Makefile, tests/common/, README.md
- [x] **1.1** — CSR stubs: cxsel, cxsidx, cxsdata
- [x] **1.2** — cxsetsel instruction stub *(scope may change based on 0.1 audit)*
- [ ] **1.3** — Runtime wrappers (cx_select, cx_get_sel, cx_valid)

**Phase 1 milestone:** tag `v0.phase1`

---

## Phase 2 — Direct Mode Semantics

- [ ] **2.1** — scxstp CSR stub
  - [ ] spec/plan written
- [ ] **2.2** — scxstp.mode=0 (Disabled) enforcement
  - [ ] spec/plan written
- [ ] **2.3** — Direct mode: cxsel=0 builtin / invalid trap / unregistered trap
  - [ ] spec/plan written

**Phase 2 milestone:** merge `cxtg-dev` → `cxtg`, tag `v0.phase2`

---

## Phase 3 — Context Status (scxxs0-3)

- [ ] **3.1** — scxxs0-3 CSR stubs
  - [ ] spec/plan written
  - [ ] `docs/flowchart_xs_states.svg` committed
- [ ] **3.2** — scxxs Off-state enforcement
  - [ ] spec/plan written
  - [ ] `docs/flowchart_os_context_switch.svg` committed
- [ ] **3.3** — scxxs dirty tracking
  - [ ] spec/plan written

**Phase 3 milestone:** merge `cxtg-dev` → `cxtg`, tag `v0.phase3`

---

## Phase 4 — CX State Access: First CX and cxsidx/cxsdata

- [ ] **4.1** — Hardcode first CX (mulacc) in QEMU
  - [ ] spec/plan written
- [ ] **4.2** — cxsidx + cxsdata wired to CX state
  - [ ] spec/plan written
- [ ] **4.3** — cxdiscard and scxdiscard
  - [ ] spec/plan written

**Phase 4 milestone:** merge `cxtg-dev` → `cxtg`, tag `v0.phase4`

---

## Phase 5 — Runtime, API, and Device Tree

- [ ] **5.1** — New Zcx runtime API stubs
  - [ ] spec/plan written
- [ ] **5.2** — End-to-end smoke test program
  - [ ] spec/plan written
- [ ] **5.3** — Device tree CX registration
  - [ ] spec/plan written
- [ ] **5.4** — Runtime cx_open DT query (UUID → CXID)
  - [ ] spec/plan written
  - [ ] `docs/flowchart_cx_open.svg` committed
- [ ] **5.5** — YAML-driven DT and CX auto-registration *(LOW PRIORITY / NICE-TO-HAVE)*
  - [ ] spec/plan written

**Phase 5 milestone:** merge `cxtg-dev` → `cxtg`, tag `v0.phase5`

---

## Phase 6 — ZcxMulti: Per-context Status and Indirect Mode

- [ ] **6.1** — scxNxs0/1 CSR stubs (N=1–63)
  - [ ] spec/plan written
- [ ] **6.2** — Per-context dirty tracking via scxNxs
  - [ ] spec/plan written
- [ ] **6.3** — Indirect mode (scxstp.mode=2)
  - [ ] spec/plan written
  - [ ] `docs/flowchart_indirect_mode.svg` committed
- [ ] **6.4** — Sscsrind for scxNxs
  - [ ] spec/plan written

**Phase 6 milestone:** merge `cxtg-dev` → `cxtg`, tag `v0.phase6`

---

## Phase 7 — Stateen Integration

- [ ] **7.1** — mstateen0.C bit
  - [ ] spec/plan written
- [ ] **7.2** — sstateen0.C and hstateen0.C
  - [ ] spec/plan written

**Phase 7 milestone:** merge `cxtg-dev` → `cxtg`, tag `v0.phase7`

---

## Phase 8 — Build Matrix and Full Integration

- [ ] **8.1** — Feature flag build matrix
  - [ ] spec/plan written
- [ ] **8.2** — Full integration test suite
  - [ ] spec/plan written

**Phase 8 milestone:** merge `cxtg-dev` → `cxtg`, tag `v1.0`

---

See [`CHANGELOG.md`](CHANGELOG.md) for the change log (previously referred to as release notes).

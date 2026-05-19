# CXTG TODO — Deferred Decisions and Open Questions

---

## Build Flag Architecture (Phase 0 decision — 2026-05-19)

**Only two new runtime extension flags are needed: `ext_zcx` and `ext_zcxmulti`.**

No Kconfig compile-time CONFIG_* flags are required. All behavior gating is done at runtime via predicate functions and handler checks against already-present QEMU extension flags. This is the standard QEMU RISC-V extension pattern.

**Runtime flags to add** (follow Artur's `ext_zicx` pattern):
- `ext_zcx` — enables the base Zcx extension; user sets `-cpu rv64,zcx=on`
- `ext_zcxmulti` — enables ZcxMulti; requires both `zcx=on` AND `sscsrind=on` per spec; predicate enforces both

**Behavior conditioning at runtime** (no new flags, check existing QEMU state):
- S-mode features (scxstp, scxxs): predicate checks `riscv_has_ext(env, RVS)`
- Smstateen gating: check `riscv_cpu_cfg(env)->ext_smstateen`
- Sscsrind for scxNxs: check `riscv_cpu_cfg(env)->ext_sscsrind`
- Hypervisor (hstateen): check `riscv_has_ext(env, RVH)`

**Pattern:**
```c
static RISCVException scxstp_pred(CPURISCVState *env, int csrno)
{
    if (!riscv_cpu_cfg(env)->ext_zcx || !riscv_has_ext(env, RVS)) {
        return RISCV_EXCP_ILLEGAL_INST;
    }
    return RISCV_EXCP_NONE;
}
```

**Block 8.1 build matrix** becomes a test matrix of `-cpu` flag combinations against a single binary — no recompilation needed.

**Action plan CONFIG_* flags** (Section 4) map to phases of implementation work, not compile-time symbols. Treat them as milestone labels, not `#ifdef` guards.

---

## CSR Address Stubs (Phase 0)

| Symbol | File | Current Value | Pending |
|--------|------|---------------|---------|
| `CSR_CXSEL` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x800` | Spec draft says `0xCC0`; confirm with spec owners |
| `CSR_CXSIDX` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x801` | Pending spec finalisation |
| `CSR_CXSDATA` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x802` | Pending spec finalisation |
| Extension name | `qemu-cxtg/target/riscv/cpu.c` etc. | `zicx` | Confirm correct ISA name (`zcx` vs `zicx`) with spec owners |

---

## cxsetsel Instruction Encoding (Block 1.2 prerequisite)

- Encoding: SYSTEM major opcode (0x73), funct3 = available slot — **TBD**
- Define `MATCH_CXSETSEL` / `MASK_CXSETSEL` in `cpu_bits.h` once encoding confirmed

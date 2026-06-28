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

**Block 8.1 build matrix** is a test matrix of `-cpu` flag combinations against a single binary — no recompilation needed.

---

## CSR Address Stubs (Phase 0)

| Symbol | File | Current Value | Pending |
|--------|------|---------------|---------|
| `CSR_CXSEL` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x800` | Spec draft says `0xCC0`; confirm with spec owners |
| `CSR_CXSIDX` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x801` | Pending spec finalisation |
| `CSR_CXSDATA` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x802` | Pending spec finalisation |
| Extension name | `qemu-cxtg/target/riscv/cpu.c` etc. | `zicx` | Rename to `zcx` — Phase 1 task |

---

## scxstp.mode=2 (Indirect) when ZcxMulti absent (Block 6.3)

Writing mode=2 to scxstp when `ext_zcxmulti` is not enabled: **behavior TBD**.

Current implementation: treated as illegal instruction (trap on use).

Awaiting TG clarification: should mode=2 be WARL-clamped on write, raise illegal instruction on write, or be silently stored but trap on first CX instruction?

---

## cxsetsel Instruction Encoding (Block 1.2 prerequisite)

- Encoding: SYSTEM major opcode (0x73), funct3 = available slot — **TBD**
- Define `MATCH_CXSETSEL` / `MASK_CXSETSEL` in `cpu_bits.h` once encoding confirmed

---

## fix trace-events warnings

- qemu-system-riscv64:trace-events.txt:3: warning: trace event 'cxsetsel_csr_read' does not exist
- qemu-system-riscv64:trace-events.txt:4: warning: trace event 'cxsetsel_csr_write' does not exist
- qemu-system-riscv64:trace-events.txt:5: warning: trace event 'cxidx_csr_read' does not exist
- qemu-system-riscv64:trace-events.txt:6: warning: trace event 'cxidx_csr_write' does not exist
- qemu-system-riscv64:trace-events.txt:7: warning: trace event 'cxdata_csr_read' does not exist
- qemu-system-riscv64:trace-events.txt:8: warning: trace event 'cxdata_csr_write' does not exist

---

## fix disassembly names and addresses (disas/riscv.c)

---

## fix trace-events warnings

---
## decide on how we are dealing with ~0 in this implementation
- spec identifies ~0 as an invalid selector, but states ~0 need not be a legal value.
- spec requires cxsel be able to hold all valid selectors and it seems to imply it must hold at least one invalid selector with "Prior to writing cxsel, implementations may convert an invalid
value into some other invalid value that cxsel is capable of holding." May implementations clamp invalid values valid values? That would seem to defeat this line in the spec: "The all 1s value may be used by software to aid in debugging uninitialized variables."
- review the TG message chain on this topic, and add an open question to the bottom of Composable-Extensions.md
- logical implementation may be one of the following:
  - all invalid selectors are illegal except ~0, and all illegal values get clamped to ~0. easy debugging (bne ~0). technically maximizes the number of available selector values, but i'm not sure if that would be useful. 
  - [TENATIVELY USING THIS MODEL] all valid selectors have MSB = 0. invalid selectors are clamped to ~0. easiest debugging (blt x0).
  - same as above, but invalid selectors are ORed with MSB, flipping the sign bit.
  - all invalid selectors are clamped to some negative value from a list of sentinels that could each convey some sort of metadata (e.g. offline, absent, busy, etc.). more granular debugging, more complicated logic
  - some sort of customized clamping of certain values to certain other values, either valid or not. unclear if this is even allowed in the spec, but if so, it is probably out of scope for this project.
  - all invalid selectors are legal, no clamping. limits debugging: you can only know a selector is invalid by attempting to execute a custom instruction 

---

## revisit Composable-Extensions.md (literal requirements) and its source, the current draft spec

- read all this todo before starting
- compare Composable-Extensions.md to the spec doc  ../composable-custom-extensions/build/composable-custom-extensions.html (if it is easier to read, look at the source files in  ../composable-custom-extensions/src/ but referencing the built doc is probably necessary for ordering, section numbers, and other meta info)
- going through the spec in order, ensure the Composable-Extenstions.md concisely states *all* the spec requirements *without omitting any detail relevant to implementation*.
- ensure that there are no assumptions. this doc must derive *all* info *directly* from the spec source.
- when that is complete, using context from current work, go through each of the open questions at the bottom to see if they are still open, citing any resolution or tenative working assumptions. don't remove anything -- if anything ever was an open question, it likely needs to be clarified in the spec, so even if it is resolved it needs to stay on record. Using superpowers/brainstorming, add as many open questions as can be identified.
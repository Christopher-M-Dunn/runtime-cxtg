# CXTG TODO — Deferred Implementation Decisions

**Note:** This document is for QEMU and runtime implementation TODOs regarding *how* we are implementing the requirements. If the question is *what* the spec requires — including non-normative sections, which this project treats as requirements — check `Composable-Extensions.md`. If still unclear there, add an item to `Composable-Extensions.md`'s Discussion section, listing any decisions as project assumptions.

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

## CSR Addresses, instruction encodings, and extension names

- This item, and therefore this TODO, is permanent and not to be removed until end of project: Final addresses and extension name or names will need to be assigned by RISCV org

- CXSETSEL insn current encoding: SYSTEM major opcode (0x73), funct3 = 100, funct7=0

- CXDISCARD - TBD, likely SYSTEM 100,1 in this implementation

| Symbol | File | Current Value | Pending |
|--------|------|---------------|---------|
| `CSR_CXSEL` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0xCA0` | Final address pending RISC-V org assignment |
| `CSR_CXSIDX` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x018` | Final address pending RISC-V org assignment |
| `CSR_CXSDATA` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x019` | Final address pending RISC-V org assignment |
| `MATCH_CXSETSEL` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x00004073` (SYSTEM, funct3=4, funct7=0) | Final encoding pending RISC-V org assignment |

---

## scxstp.mode=2 (Indirect) when ZcxMulti absent (Block 6.3)

**RESOLVED (2026-07-20)** — see `Composable-Extensions.md` Discussion, "`scxstp.mode = 2` when `Zcxmulti` is absent": `scxstp` is WARL, so writing mode=2 with `ext_zcxmulti` disabled is legalized on write, not deferred to a trap on use.

**Project implementation:** clamp mode 2 to 1 (Direct), with all other bits = 0. Any write with a reserved mode (3–15) is ignored.

---

## decide on how we are dealing with ~0 in this implementation
- spec identifies ~0 as an invalid selector, but states ~0 need not be a legal value.
- spec requires cxsel be able to hold all valid selectors and it seems to imply it must hold at least one invalid selector with "Prior to writing cxsel, implementations may convert an invalid
value into some other invalid value that cxsel is capable of holding." May implementations clamp invalid values to valid values? Does that defeat this line in the spec?: "The all 1s value may be used by software to aid in debugging uninitialized variables." No. uninitialized values can be detected but spec gives no guaranteed way of checking if a selector is valid just given its value. I guess this is okay-- I believe runtime will have mechanisms for doing this.
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

**DONE (2026-07-13)** — full spec-to-doc comparison completed; missing sections added (composability criteria, external specifications/discovery, architecture models, verbatim library and context-switch examples); open questions reviewed, softened, and expanded; project-irrelevant spec content trimmed.

- read all this todo before starting
- compare Composable-Extensions.md to the spec doc  ../composable-custom-extensions/build/composable-custom-extensions.html (if it is easier to read, look at the source files in  ../composable-custom-extensions/src/ but referencing the built doc is probably necessary for ordering, section numbers, and other meta info)
- going through the spec in order, ensure the Composable-Extenstions.md concisely states *all* the spec requirements *without omitting ANY detail needed for implementation*.
- ensure that there are no assumptions. this doc must derive *all* info *directly* from the spec source.
- when that is complete, using context from current work, go through each of the open questions at the bottom to see if they are still open, citing any resolution or tenative working assumptions. don't remove anything -- if anything ever was an open question, it likely needs to be clarified in the spec, so even if it is resolved it needs to stay on record. Using superpowers/brainstorming, add as many open questions as can be identified.

---

## sync CXTG_QEMU_Action_Plan.md with recent todo.md / Composable-Extensions.md edits

`todo.md` and `Composable-Extensions.md` picked up a number of edits: `Composable-Extensions.md` has been rewritten after reanalyzing the spec source. It now includes more non-normative info that this project will consider as requirements when possible. `Composable-Extensions.md`'s `## Open Questions` heading was renamed to `## Discussion`, several Discussion items were resolved, reworded, and added. `todo.md`'s own title and scope note were tightened to explicitly separate *how* (todo.md) from *what* (Composable-Extensions.md).

The stale `scxstp.mode=2` line in `CXTG_QEMU_Action_Plan.md`'s Block 6.3 notes was already caught and fixed as a one-off (2026-07-25), but the rest of the Action Plan has not been reviewed end-to-end against these changes.

- read through `Composable-Extensions.md`. The whole document was updated. Keep it in context for things that may need to be changed or added in the Action Plan.
- read through `CXTG_QEMU_Action_Plan.md` block by block against the current `todo.md` and `Composable-Extensions.md`
- look for any other block notes, implementation notes, or definitions of done that cite behavior, wording, or section names that have since changed (the renamed `## Discussion` heading and any other resolved Discussion items are the likely culprits)
- update the `Supporting Documents` table and any other doc-scope description in the Action Plan if it still blurs the how/what split
- use superpowers brainstorming to ask clarifying questions
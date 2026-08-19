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
  - **[DECIDED MODEL — 2026-08-14, awaiting integration]** all invalid selectors are illegal except `~0`; every illegal write clamps to `~0`. Maximizes the number of available selector values, since no bit pattern is reserved just to mark validity. Spec-compliant today — WARL already permits clamping to any invalid value — and matches the `~0` half of the WARL simplification proposal below, so it has no dependency on that proposal.
  - all valid selectors have MSB = 0; invalid selectors clamped to `~0`. Restricts the valid-selector space beyond what spec requires — not adopted, superseded by the model above.
  - same as above, but invalid selectors are ORed with MSB, flipping the sign bit.
  - all invalid selectors are clamped to some negative value from a list of sentinels that could each convey some sort of metadata (e.g. offline, absent, busy, etc.). more granular debugging, more complicated logic
  - some sort of customized clamping of certain values to certain other values, either valid or not. unclear if this is even allowed in the spec, but if so, it is probably out of scope for this project.
  - all invalid selectors are legal, no clamping. limits debugging: you can only know a selector is invalid by attempting to execute a custom instruction 
- caveat on debugging via `cxsel` readback: spec guarantees `~0` is always invalid and that *some* invalid value must be legal/storable, but not that `~0` specifically is that value — so comparing `cxsel`'s readback against `~0` (e.g. `bne`) is reliable only because it's *this project's own* WARL choice, not a portable spec guarantee. Any test relying on it gets a `// not portable, project-specific WARL choice` comment, removable if the proposal below is adopted (which would make `~0` legal and required everywhere).

**Proposed WARL simplification from the 2026-08-14 TG meeting — strong agreement from attendees present, but Darius was not there and has previously advocated for the current spec wording on this exact point. Do not implement until either it is confirmed with Darius that he is on-board or the TG records a vote.**
 
Direct and Indirect mode, both:
- `~0` (reads XLEN-wide all-ones, regardless of how many bits `cxsel` actually implements) stays as reserved invalid selector, however now it is *always legal*.
- `~0` becomes *required* to be implemented, on the same footing as the built-in selector `0`.
- `~0` becomes the *only* selector that is both *legal* **and** *invalid*.
- *All* invalid values *must* read as `~0` via WARL — a hard tightening of the current spec's "may be converted to some other invalid value `cxsel` is capable of holding." The project's decided model above already matches this; if adopted TG-wide, nothing changes here, and the readback-portability caveat above no longer applies.

Indirect mode only:
- *All* table indices become valid *and* legal selectors regardless of entry contents — including `V = 0`, stale, and uninitialized entries.* This would separate "is `cxsel` a legal selector value" from "does the indicated entry currently authorize dispatch," and may resolve the Indirect mode `cxsel` bounds Discussion item outright for in-range indices.
- Table size may become implementation-variable instead of the spec's fixed 1024 entries — This needs further discussion to sus out any unforeseen ramifications; smaller tables attractive, larger tables a nice-to-have.

Direct mode only (briefly discussed, may need more consideration):
- All selectors not corresponding to real hardware become invalid and illegal.
- Open question: what determines "corresponds to real hardware" — a simple CXID range bound, CXID alone, CXID+context-index, or something that must also account for a hot-swappable PCI-like CXU slot, wherein the slot itself is present but the CXU is disabled/removed/powered down? A simple range bound is the only method guaranteed to be inexpensive, and all rest could be done inexpensively if carefully designed. Leaning toward a simple range bound, or some other simple check against runtime-immutable config

---

## revisit Composable-Extensions.md (literal requirements) and its source, the current draft spec

**DONE (2026-07-13)** — full spec-to-doc comparison completed; missing sections added (composability criteria, external specifications/discovery, architecture models, verbatim library and context-switch examples); open questions reviewed, softened, and expanded; project-irrelevant spec content trimmed.

- read all this todo before starting
- compare Composable-Extensions.md to the spec doc  ../composable-custom-extensions/build/composable-custom-extensions.html (if it is easier to read, look at the source files in  ../composable-custom-extensions/src/ but referencing the built doc is probably necessary for ordering, section numbers, and other meta info)
- going through the spec in order, ensure the Composable-Extenstions.md concisely states *all* the spec requirements *without omitting ANY detail needed for implementation*.
- ensure that there are no assumptions. this doc must derive *all* info *directly* from the spec source.
- when that is complete, using context from current work, go through each of the open questions at the bottom to see if they are still open, citing any resolution or tenative working assumptions. don't remove anything -- if anything ever was an open question, it likely needs to be clarified in the spec, so even if it is resolved it needs to stay on record. Using superpowers/brainstorming, add as many open questions as can be identified.

---

## incorporate Requirements.md into Composable-Extensions.md's `## Discussion`

**NOTE (2026-08-19)** — changed choice of `cxsidx` WARL implementation model. See `Composable-Extensions.md`.
**DONE (2026-08-18)** — `Requirements.md` cited into 10 existing Discussion items and folded into 5 new items appended at the bottom (RV32/RV64 parity, machine-mode-only/M+U-mode-only support, state context size bound, 255-CXs-vs-64-status-slots, performance-monitor/debug-trace excludes). Two outcomes below diverge from what this item originally sketched, left here for the record rather than edited: the `cxsidx` WARL clamp project assumption changed mid-work to Option 1 (`CX[cxsel] STATE_NUM_WORDS-1`, a per-CX bound), with code structured to also support Option 2 (`MAX()` across all CXs), rather than the flat 131071/65535 clamp the "state contexts from zero to at least 512 KiB" bullet below describes; and the "Hart↔context cardinality" item's stance flipped from favoring the flexible (many-to-many) option to starting from a rigid identical-topology-per-hart requirement. Also surfaced along the way: `CXTG_QEMU_Action_Plan.md`'s §4 "Note on S-mode" claim that M-mode-only requires a different machine/CPU definition is stale — `-cpu rv64,s=false,h=false,zcx=on` boots the same `virt` machine M-mode-only and passes `block1_1_csr_stubs.elf`; recorded in the machine-mode-only Discussion item, Action Plan note not yet corrected.

`docs/Requirements.md` (offline copy of the TG's requirements page, draft 2026-08-13) is not yet reflected anywhere in `Composable-Extensions.md`. It records what the TG *wants to achieve*, not what the spec literally states, so none of it belongs in the requirement sections — it goes in `## Discussion` only, cited as TG intent, siloed the same way basis-spec material is.

- read `docs/Requirements.md` in full; note the three-way tagging — unmarked = proposed for inclusion, `[discuss]` = undecided, `[exclude]` = proposed for exclusion
- new Discussion items go at the bottom of the section, not grouped near related topics
- for each existing Discussion item, check whether `Requirements.md` constrains it; cite it in that item rather than opening a duplicate
- `[exclude]` items that close off options currently under consideration — dynamic state context resizing, dynamic context count, performance monitors, debug/trace — record as bounded so they are not reopened
- `[discuss]` items that overlap existing open questions: more than one state context per hart; whether context count may differ from hart count; more than one selected CX at a time (cf. the tabled multiple-`cxsel` item)
  - the context-count-vs-hart-count overlap already has a start: a "Hart↔context cardinality" Discussion item was added out of band on 2026-08-14 (merging TG meeting results), taking a project stance favoring the flexible option. Build on/verify that item rather than opening a duplicate.
  - that same 2026-08-14 pass also touched the Indirect mode `cxsel` bounds, `V` bit, `cx_status`, and `scx_invalidate` items with meeting-derived content — skim the current Discussion section before assuming any of the items below are still in their original state
- unmarked (settled) requirements with no current coverage — add one Discussion item each:
  - RV32 and RV64 supported without preference for either; note that `scxxs`/`scxNxs` register pairing differs by XLEN, so RV32 exercises logic RV64 never does
  - machine-mode-only and machine+user-only systems must be supported (all present phases assume S-mode)
  - state contexts from zero to at least 512 KiB — bounds the existing `cxsidx` out-of-bounds item with a concrete figure, though `cxsidx` indexes XLEN-sized words rather than bytes: 512 KiB is 131072 words on RV32, 65536 on RV64. True legal bounds may be implementation-specific, and the TG is still undecided — the two proposals so far are that the legal WARL upper bound is either `CX[cxsel] STATE_NUM_WORDS-1`, or `MAX({CX1 STATE_NUM_WORDS-1}, {CX2 STATE_NUM_WORDS-1}, ..., {CXN STATE_NUM_WORDS-1})`. Under the latter — or any other implementation admitting legal-but-invalid values — a `cxsdata` read or write with an invalid `cxsidx` is currently spec'd UNDEFINED. Project assumption: clamp `cxsidx` values above 131071 (RV32) / 65535 (RV64) to those values, simply to have defined WARL behavior to test for.
- merge into the existing `cx_status` CSR / `get_status` opcode Discussion item rather than opening a new one: `Requirements.md` lists collecting accumulated errors arising from CX selection as a settled requirement, yet no such mechanism exists in the current draft — the basis spec's `cx_status` accrued-error CSR was dropped (see `basis_spec_comparison.md`). That Discussion item currently reads `cx_status` as intentionally superseded; the requirement suggests the need it served has not gone away. Worth raising with the TG.
- verify before recording as a conflict: `Requirements.md` states a system may have at least 255 CXs, while `scxxs0`–`scxxs3` hold 2-bit status for at most 64 CX IDs, including #0, the built-in selector — on both RV32 and RV64 (on RV64 only the even-numbered registers are present, so both XLENs yield 64 slots). Establish whether these are the same quantity, or whether a system can have 255 addressable CXs but state management enabled on only 63 of them.
- terminology: `Requirements.md` states "All CXs are stateful, but some CXs have zero-sized state" and explicitly excludes the "some CXs are stateless" framing; the `scxxs` state machine Discussion item currently uses "Stateless CXs".

---

## sync CXTG_QEMU_Action_Plan.md with recent todo.md / Composable-Extensions.md edits

`todo.md` and `Composable-Extensions.md` picked up a number of edits: `Composable-Extensions.md` has been rewritten after reanalyzing the spec source. It now includes more non-normative info that this project will consider as requirements when possible. `Composable-Extensions.md`'s `## Open Questions` heading was renamed to `## Discussion`, several Discussion items were resolved, reworded, and added. `todo.md`'s own title and scope note were tightened to explicitly separate *how* (todo.md) from *what* (Composable-Extensions.md).

The stale `scxstp.mode=2` line in `CXTG_QEMU_Action_Plan.md`'s Block 6.3 notes was already caught and fixed as a one-off (2026-07-25), but the rest of the Action Plan has not been reviewed end-to-end against these changes.

- read through `Composable-Extensions.md`. The whole document was updated. Keep it in context for things that may need to be changed or added in the Action Plan.
- read through `CXTG_QEMU_Action_Plan.md` block by block against the current `todo.md` and `Composable-Extensions.md`
- look for any other block notes, implementation notes, or definitions of done that cite behavior, wording, or section names that have since changed (the renamed `## Discussion` heading and any other resolved Discussion items are the likely culprits)
- update the `Supporting Documents` table and any other doc-scope description in the Action Plan if it still blurs the how/what split
- read `docs/Requirements.md` and assess its effect on block scope and phase ordering. It is TG intent writ large, which is what the Action Plan seeks to capture at this time. The system in place deliberately tolerates minor changes to the requirements, as the actual execution plans are only built one suphase at a time and only when needed. Prioritize settled items where possible, defer `[discuss]` items where possible, and there are no blocks aimed at a target the TG has already `[excluded]`:
  - Block 8.1's matrix varies runtime extension flags (`zcx`/`zcxmulti`/`sscsrind`/`smstateen`) only. Settled requirements call for RV32 *and* RV64 without preference, and for M-only and M+U-only systems — none of which any block currently varies. Decide whether 8.1 expands or a new block is added.
  - Block 4.2's `cxsidx` bounds work has no concrete state size; the settled requirement is zero to at least 512 KiB. Project assumption is a WARL clamp of out-of-bounds values to 131071 (RV32) / 65535 (RV64); `cxsdata` read/write with an invalid `cxsidx` is UNDEFINED
  - the `[exclude]` items (dynamic state context resizing, dynamic context count, performance monitors, debug/trace) confirm scope the Action Plan can stay out of — check no block has drifted toward them
  - the `[discuss]` items cluster on multi-instance/multi-context, which is Phase 6 territory; weigh against Phase 6 already sitting on the priv ISA the TG has no consensus on
  - add `docs/Requirements.md` and `docs/CHARTER.md` to the `Supporting Documents` table, with scope notes distinguishing TG intent from literal spec requirements
- also weigh the TG's WBS and ratification plans (`wbs.md`/`wbs.csv` and the Ratification Plan Milestone Review pages, https://riscv.atlassian.net/wiki/spaces/CCEX/; offline copies are kept outside the repo): the spec ratifies as three separate packages (Unpriv ISA / Priv ISA / Non-ISA), each carrying its own `freeze-isa-sim dep=poc-qemu` checklist item. The unpriv package is the one that can actually move (graded A; `spec-isa-unpriv progress=10%`), while `spec-isa-priv` has no recorded progress. Consider whether the Action Plan should define an "unpriv-package-complete" milestone, and note that `poc-qemu-sampleext` is the only PoC deliverable with no spec dependency at all — Phase 4.1 is safe to build against no matter how the spec moves.
- use superpowers brainstorming to ask clarifying questions

---

## sync README.md

Ensure `README.md` is current, and gives useful and relevant info, especially regarding orientation around the structure of the repo, all documentation and images with their use and scope, how to approach the wiki, (explain its function, how pages are generated, index.md, best viewed in obsidian with the root vault view being the root repo (runtime-cxtg/, not wiki/, so that links to source files work)). **make sure to stay concise**

---

## documentation prep for Action Plan update

- read all this todo, then use superpowers brainstorming before starting
- do this work on `todo/doc-rework` cut from cxtg-dev
- create `qemu-cxtg/Implementation_Choices.md` (aka "Choices"). All non-normative implementation choices will be documented here in a numbered list, as well as any change that requires an update to the Action Plan. Make a note at the top that if a choice changes meaningfully, it should be moved to a new number at the bottom and the entry at the old number should change to "Deprecated. See #X. [strike-through of old text]", where X is the new item number. The new entry should start with a reference to the entry it supersedes.  Explain about how the document is divided by a divider where everything below the divider is not yet incorporated into `docs/CXTG_QEMU_Action_Plan.md`. Put a divider just below the introductory notes.  
- `docs/todo.md`
  - Go through each item that is not marked as done and determine if it actually is already done (if so, mark it as done).
  - Go one at a time extracting any project assumptions or decisions (anything that could affect the Action Plan), and create new entries in Choices. determine if the item can be removed from `todo.md`. Unless another todo item is referencing it, it's not clear what other circumstance would prevent it from being removed.
- `qemu-cxtg/Composable-Extensions.md` Go through the Discussion section to improve conciseness: identify superfluous or duplicate info, and move references out to footnotes. Everything remaining should be concisely identifying gaps the spec doesn't cover, resolution, implementation (e.g. Implementation: I29) Move all implementation decisions to Choices and replace the text with a reference. read the whole document first, then apply small edits one at a time so they can be reviewed. Add a References section. For references format, use anchor links to LetterNumber e.g. #R2, where the letter represents a particular source type or document, and the number is the reference index. so if the reference is `#R2`, the reference is to the 2nd item under `Requirements.md` group in the References section; `#M1` would be the first item in the `Meetings` group (include the meeting date in the item); `I29` would be entry #29 in`Implementation_Choices.md`
- create the wiki for Choices
- reassess item `todo.md:130` for stale info based on the changes made since it was created (9250385738fae0e6934f75c65ed9215a865be062). Rewrite it as necessary. it will need to know about Choices to know what to change, but should also add entries as additional decisions are gleaned during the rewrite.
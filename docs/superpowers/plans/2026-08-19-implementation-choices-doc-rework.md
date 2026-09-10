# Implementation Choices Doc & Documentation Prep Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create `qemu-cxtg/Implementation_Choices.md` as the single source of truth for project-specific implementation decisions, sweep `docs/todo.md` to extract decided items into it (archiving cleared items to a new `docs/todo_done_did.md`), trim `qemu-cxtg/Composable-Extensions.md`'s Discussion section down to gap/status/pointer form with a new References section, wire up the wiki, and rewrite the now-stale `todo.md:130` Action-Plan-sync item.

**Architecture:** Pure documentation restructuring across two repos (`runtime-cxtg`, `qemu-cxtg`) on a shared feature branch. No code changes, no tests in the pytest sense — "tests" here are `grep`/`wc -l` checks and human review of `git diff` per task. Work proceeds in strict order because Choices entries are numbered sequentially, append-only, across the whole task — later tasks depend on the entry count left by earlier ones.

**Tech Stack:** Markdown, git, `wc`/`grep` for verification. Wiki pages follow `wiki/schema.md` Format D.

**Spec:** [`docs/superpowers/specs/2026-08-18-implementation-choices-doc-design.md`](../specs/2026-08-18-implementation-choices-doc-design.md) — the plan argues from the spec; read both before executing any task.

## Global Constraints

- Entries in `Implementation_Choices.md` are plain lines, never markdown numbered-list syntax: `<N> - <text>` (plain) or `<N> * <text>` (has a corresponding Discussion item). State the decision only — no rationale, no "why," no source citation.
- Choices numbering is sequential and append-only across the **entire task**, not per-file or per-task. Before adding an entry, check the current highest number in `Implementation_Choices.md` and use the next one. The numbers `29`/`30` used as illustrations in the spec are illustrative only — do not force those specific numbers.
- `Composable-Extensions.md` cites Choices by number (`Implementation: I<N>`); Choices entries never cite back except the trailing `*`.
- `docs/todo_done_did.md` entries are verbatim moves, never summarized or corrected — it's a historical record, not a live reference. If a later decision supersedes something recorded there, the correction lives in Choices, not in the archive.
- No changes to `docs/CXTG_QEMU_Action_Plan.md` itself. No scan of the QEMU source tree or `spec.md` for additional Choices candidates — only `todo.md` and `Composable-Extensions.md`'s Discussion section feed Choices in this pass.
- Reference-group taxonomy for `Composable-Extensions.md`'s new References section: **R** = `Requirements.md`, **M** = dated TG meetings, **B** = basis spec / `basis_spec_comparison.md`, **D** = design decks. No 5th group this pass.
- Per CLAUDE.md: never stage or commit without an explicit "ready to commit?" confirmation from the user, regardless of task/phase boundaries.
- All work happens on branch `todo/doc-rework`, cut from `cxtg-dev` in **both** `runtime-cxtg` and `qemu-cxtg`.

---

### Task 1: Cut the shared branch in both repos

**Files:** none (git operations only)

**Interfaces:** N/A

- [ ] **Step 1: Check both repos are clean**

Run: `git -C /home/cmd/Github/runtime-cxtg status --short && git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg status --short`
Expected: only the already-known untracked/modified files from this session (the spec file, `docs/todo.md`); no unexpected dirty state. If either repo has unrelated uncommitted work, stop and ask the user before branching.

- [ ] **Step 2: Cut `todo/doc-rework` in `runtime-cxtg`**

Run: `git -C /home/cmd/Github/runtime-cxtg checkout -b todo/doc-rework cxtg-dev`
Expected: `Switched to a new branch 'todo/doc-rework'`. If `cxtg-dev` isn't the current branch's ancestor or there are conflicts, resolve before continuing (do not force).

- [ ] **Step 3: Cut `todo/doc-rework` in `qemu-cxtg`**

Run: `git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg checkout -b todo/doc-rework cxtg-dev`
Expected: `Switched to a new branch 'todo/doc-rework'`.

- [ ] **Step 4: Verify**

Run: `git -C /home/cmd/Github/runtime-cxtg branch --show-current && git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg branch --show-current`
Expected: `todo/doc-rework` printed twice.

No commit this step — nothing has changed yet.

---

### Task 2: `Implementation_Choices.md` skeleton + permanent CSR/encoding entries

**Files:**
- Create: `qemu-cxtg/Implementation_Choices.md`

**Interfaces:**
- Produces: the numbered entry list later tasks append to. After this task, the highest entry number is **4**.

- [ ] **Step 1: Write the skeleton + first 4 entries**

Create `qemu-cxtg/Implementation_Choices.md`:

```markdown
# Implementation Choices

Numbered, append-only log of project-specific implementation decisions that go beyond what the spec (as distilled into this document's requirement sections) and `docs/Requirements.md` state. Test for inclusion: starting from those two sources, what additional information would someone need to replicate this project's behavior exactly? That's what belongs here.

Entries are plain lines: `<N> - <text>` for a plain entry, `<N> * <text>` when the entry has a corresponding item in `Composable-Extensions.md`'s Discussion section (the `*` is the only cross-reference this document carries in that direction — `Composable-Extensions.md` cites back by number, e.g. `Implementation: I12`). State the decision only: no rationale, no source citation.

If a choice changes meaningfully, the entry at its old number becomes:

> **Deprecated. See #X.** ~~<original text>~~

and a new entry is appended at the bottom of the list starting with a reference to the entry it supersedes, where `X` is the new entry's own number.

Everything below the divider is not yet incorporated into `docs/CXTG_QEMU_Action_Plan.md`. As entries get incorporated there in future work, they move above the divider.

---

1 - Runtime extension flags: `ext_zcx`, `ext_zcxmulti` (requires `zcx=on` AND `sscsrind=on`). No compile-time `CONFIG_*` flags; gating via predicate functions checking `riscv_has_ext()`/`riscv_cpu_cfg()` against existing QEMU flags — S-mode features (`scxstp`, `scxxs`) check `RVS`; `Smstateen` gating checks `ext_smstateen`; `scxNxs` checks `ext_sscsrind`; hypervisor (`hstateen`) checks `RVH`.
2 - CSR addresses (`qemu-cxtg/target/riscv/cpu_bits.h`): `CSR_CXSEL = 0xCA0`, `CSR_CXSIDX = 0x018`, `CSR_CXSDATA = 0x019`.
3 - `cxsetsel` opcode: `MATCH_CXSETSEL = 0x00004073` (SYSTEM major opcode, funct3=4, funct7=0).
4 - `cxdiscard` opcode: TBD, likely SYSTEM funct3=4, funct7=1 in this implementation.
```

- [ ] **Step 2: Verify format**

Run: `grep -n '^[0-9]' qemu-cxtg/Implementation_Choices.md`
Expected: four lines, `1 - ...` through `4 - ...`, no markdown `1.`/`2.` list syntax.

- [ ] **Step 3: Commit**

```bash
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg add Implementation_Choices.md
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg commit -m "docs: create Implementation_Choices.md with CSR/encoding placeholders"
```

(Ask the user "ready to commit?" before running — see Global Constraints.)

---

### Task 3: `docs/todo.md` sweep — Pass A (staleness check)

**Files:**
- Modify: `docs/todo.md`

**Interfaces:** none (no Choices entries created this task — extraction is Task 4)

Read every item in `docs/todo.md` not already marked `DONE`/`RESOLVED`. For each, check the current state of the project (via `Composable-Extensions.md`, `CHANGELOG.md`, code where relevant) to see if it's actually already resolved but not flagged.

Known findings from this session's reading — verify each, then act:

- [ ] **Step 1: "Build Flag Architecture (Phase 0 decision — 2026-05-19)"** — check `CHANGELOG.md`'s Phase 0/1 entries: `ext_zcx` is implemented (renamed from `ext_zicx` in the `feat/1.1` block). This item describes settled, implemented behavior. It has no `DONE`/`RESOLVED` marker in its heading — add one: change the heading to `## Build Flag Architecture (Phase 0 decision — 2026-05-19) — DONE`. Do not extract or archive yet (that's Task 4).

- [ ] **Step 2: "sync README.md"** — check whether `README.md` has been updated for wiki orientation/structure since this item was written. If not done, leave unmarked. (Expected: still open — no evidence of this work in `progress.md`.)

- [ ] **Step 3: "decide on how we are dealing with ~0 in this implementation"** — check whether the "review the TG message chain on this topic, and add an open question to the bottom of Composable-Extensions.md" action bullet has been done. Search `Composable-Extensions.md`'s Discussion section for a dedicated `~0`/all-invalid-selector item: `grep -n "~0" qemu-cxtg/Composable-Extensions.md`. If no dedicated item exists (the topic is currently only touched inside the "Valid/invalid selector" senses item and a few others), this action bullet is still outstanding — leave it unmarked, but note in Task 4 that only the **DECIDED MODEL** sub-bullet is extractable; the message-chain-review action bullet stays as a genuine open item.

- [ ] **Step 4: Verify no other unmarked item is secretly done**

Skim the remaining unmarked items ("scxstp.mode=2" is already marked RESOLVED; "revisit Composable-Extensions.md" and "incorporate Requirements.md" are already marked DONE; "sync CXTG_QEMU_Action_Plan.md" and "documentation prep for Action Plan update" are active/in-progress, not stale). No further staleness expected.

- [ ] **Step 5: Commit**

```bash
git -C /home/cmd/Github/runtime-cxtg add docs/todo.md
git -C /home/cmd/Github/runtime-cxtg commit -m "docs: mark Build Flag Architecture item done in todo.md"
```

---

### Task 4: `docs/todo.md` sweep — Pass B (extraction, split, archive) + create `todo_done_did.md`

**Files:**
- Create: `docs/todo_done_did.md`
- Modify: `docs/todo.md`
- Modify: `qemu-cxtg/Implementation_Choices.md`

**Interfaces:**
- Consumes: `Implementation_Choices.md` entries 1-4 (Task 2). Next available number is **5**.
- Produces: `todo_done_did.md` skeleton + archived entries; more Choices entries. Track the running "next number" as you go — write it down after each addition.

Process each `todo.md` item below in order. "Extract" means: write a new Choices entry (concrete form, no rationale) at the next available number, appended to the numbered list in `Implementation_Choices.md`. "Archive" means: move the item's text verbatim (or the decided fragment, for splits) to `todo_done_did.md`, with a trailing `→ Choices #N` line, then remove it from `todo.md`.

- [ ] **Step 1: Create `docs/todo_done_did.md` skeleton**

```markdown
# CXTG TODO — Done / Did

Append-only archive of `docs/todo.md` items (or decided fragments split off from compound items) once cleared. Entries are moved here verbatim, not summarized or corrected — this is a record of what the item said when it was cleared, not a live reference. For current project decisions, see `qemu-cxtg/Implementation_Choices.md`.

---
```

- [ ] **Step 2: "Build Flag Architecture (Phase 0 decision — 2026-05-19)"** (now marked DONE from Task 3)

Extract to Choices: this decision is already captured as entry 1 (Task 2) — no new entry needed, it would duplicate. Archive the full item text verbatim to `todo_done_did.md` with `→ Choices #1`. Remove from `todo.md`.

- [ ] **Step 3: "scxstp.mode=2 (Indirect) when ZcxMulti absent (Block 6.3)"** (already RESOLVED)

Extract to Choices at the next number (expected **5**):
`5 - scxstp WARL behaviour: writing scxstp.mode=2 (Indirect) when ext_zcxmulti is disabled clamps to mode=1 (Direct), all other bits zeroed; writes with reserved modes 3-15 are ignored; writes with mode = 0 or 1 zero all other bits.`

Mark this entry with `*` (it has a corresponding Discussion item, "scxstp.mode = 2 when Zcxmulti is absent," to be trimmed in Task 5-7) — write it as `5 * ...` not `5 - ...`.

Archive the full item text verbatim to `todo_done_did.md` with `→ Choices #5`. Remove from `todo.md`.

- [ ] **Step 4: "decide on how we are dealing with ~0 in this implementation"** (compound — split)

Decided part (the `[DECIDED MODEL — 2026-08-14, awaiting integration]` bullet only) → extract to Choices at the next number (expected **6**):
`6 - cxsel WARL behaviour: all invalid selector values are illegal except ~0; every write of an illegal value clamps cxsel to ~0.`

No `*` — there's no dedicated Discussion item for this yet (confirmed in Task 3, Step 3).

Archive just the decided bullet's text to `todo_done_did.md` with `→ Choices #6`.

The rest of the item — the alternative-models list (historical brainstorm, not adopted), the readback-portability caveat, the "review the TG message chain... add an open question" action bullet (still outstanding per Task 3), and the entire "Proposed WARL simplification from the 2026-08-14 TG meeting" block (explicitly blocked on Darius/TG vote) — stays in `todo.md`. Reword the item heading/intro to reflect that the model itself is decided (pointing to Choices #6) and only the message-chain-review action and the proposed simplification remain open:

Replace the item's opening paragraph and the `[DECIDED MODEL...]` bullet with:

```markdown
## decide on how we are dealing with ~0 in this implementation

**Model decided** — see `Implementation_Choices.md` #6. Remaining open items below.

- review the TG message chain on this topic, and add an open question to the bottom of Composable-Extensions.md
```

Keep the "caveat on debugging via `cxsel` readback" bullet and the full "Proposed WARL simplification..." block unchanged below that.

- [ ] **Step 5: "revisit Composable-Extensions.md (literal requirements)..."** (already DONE)

Pure archival, no Choices content (describes completed work, not a decision). Archive verbatim to `todo_done_did.md` with no `→ Choices` pointer (none applies). Remove from `todo.md`.

- [ ] **Step 6: "incorporate Requirements.md into Composable-Extensions.md's `## Discussion`"** (already DONE)

Pure archival. Archive verbatim to `todo_done_did.md`, no Choices pointer. Remove from `todo.md`. (Its DONE note's claim that the `cxsidx` assumption "changed mid-work to Option 1" is now itself superseded by the flat-clamp decision — per Global Constraints, this is **not** corrected in the archive; the correction lives in the Choices entry created in Task 6/7 when the corresponding Discussion item is trimmed.)

- [ ] **Step 7: "sync CXTG_QEMU_Action_Plan.md..."** — **do not touch in this task.** Handled separately in Task 9.

- [ ] **Step 8: "sync README.md"** — still open (Task 3, Step 2). Leave untouched in `todo.md`.

- [ ] **Step 9: "documentation prep for Action Plan update"** — this is the item driving the current work. **Do not touch in this task.** Handled in Task 9, Step 3, once all other phases are complete.

- [ ] **Step 10: Verify `todo.md` no longer contains the archived items**

Run: `grep -n "^## " docs/todo.md`
Expected headings remaining: `Build Flag Architecture` (gone — archived), so expected list is: `CSR Addresses, instruction encodings, and extension names`; `decide on how we are dealing with ~0 in this implementation`; `sync CXTG_QEMU_Action_Plan.md with recent todo.md / Composable-Extensions.md edits`; `sync README.md`; `documentation prep for Action Plan update`.

- [ ] **Step 11: Verify Choices numbering**

Run: `grep -n '^[0-9]' qemu-cxtg/Implementation_Choices.md | tail -5`
Expected: entries 1 through 6 present, entry 5 marked with `*`, entry 6 plain.

- [ ] **Step 12: Commit**

```bash
git -C /home/cmd/Github/runtime-cxtg add docs/todo.md docs/todo_done_did.md
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg add Implementation_Choices.md
git -C /home/cmd/Github/runtime-cxtg commit -m "docs: sweep todo.md — extract decisions to Choices, archive cleared items"
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg commit -m "docs: add Choices entries from todo.md sweep"
```

---

### Task 5: `Composable-Extensions.md` Discussion trim — items 1-12

**Files:**
- Modify: `qemu-cxtg/Composable-Extensions.md`
- Modify: `qemu-cxtg/Implementation_Choices.md`

**Interfaces:**
- Consumes: Choices entries 1-6 (Tasks 2, 4). Next available number is **7**.
- Produces: more Choices entries; trimmed Discussion prose for the first 12 items (in document order, per the current file).

For each item below: rewrite to *gap → resolution status → implementation pointer (if any)*, per spec §5. Classification from this session's full read of the Discussion section:

- [ ] **Step 1: "§cxsel — URO vs. WARL"** — no extractable decision (interpretive resolution, no value/behavior beyond what's already in the requirement sections). Trim the explanatory clause about the hardware-enforced URO range down to one sentence; keep status line.

- [ ] **Step 2: "§cxsetsel — illegal instruction conditions"** — extract "illegal in both cases" (Disabled mode, `sstateen0.C=0`) to Choices #7:
`7 * cxsetsel raises illegal instruction when scxstp.mode = Disabled or sstateen0.C = 0.`
Replace the item's "Project assumption: illegal in both cases" sentence with `Implementation: I7`.

- [ ] **Step 3: "§cxsel — 'built-in custom extension' used before it is defined"** — no extraction (editorial note only). Leave concise as-is.

- [ ] **Step 4: "§cxsetsel — Does 'CX instructions treated as illegal' include cxsetsel?"** — extract the resolution to Choices #8:
`8 * cxsetsel is not subject to the "CX instructions treated as illegal when cxsel is invalid" rule — it remains executable regardless of cxsel's validity, since it's the only way to restore a valid value.`
Replace resolution prose with `Implementation: I8`.

- [ ] **Step 5: "§cxsidx repeated text"** — no extraction (editorial artifact note). Leave as-is.

- [ ] **Step 6: "CX CSR interrupt atomicity"** — extract to Choices #9:
`9 * cxsdata read-modify-write and the cxsidx auto-increment are treated as atomic as a unit (standard base-ISA CSR instruction atomicity); no restart semantics are implemented.`
Replace resolution prose with `Implementation: I9`.

- [ ] **Step 7: "`cxsidx` out-of-bounds behavior"** — extract the chosen option to Choices #10:
`10 * cxsdata access with cxsidx >= state_size is undefined/unenforced (option c) — no trap, no clamp.`
Replace the "status: Resolved..." paragraph with `Implementation: I10`. Keep the (a)/(b)/(c) option list as context for why this was a choice among alternatives — this is the kind of superfluous detail the todo item's conciseness instruction targets, so trim the (a)/(b)/(c) list down to a one-line "options considered: trap, clamp, or leave undefined" if it reads as filler; use judgment on whether the alternatives add value.

- [ ] **Step 8: "`cxsdata` auto-increment"** — no extraction (tabled, no decision made). Leave as-is.

- [ ] **Step 9: "HARTs with heterogeneous CX support"** — no extraction (tabled, out-of-scope note; overlaps item 38's "Number of contexts per hart" which carries the real assumption — don't duplicate). Trim the two long code examples if they read as filler beyond illustrating the two candidate approaches — use judgment; the examples are illustrative, not implementation-critical, so they're a reasonable conciseness cut, but confirm nothing else in the doc references them before deleting.

- [ ] **Step 10: "`cxsetsel` atomicity scope"** — extract to Choices #11:
`11 * cxsetsel's "atomic" swap is instruction-scoped only — not interruptible mid-execution on the executing hart, no cross-hart memory visibility implied.`
Replace resolution prose with `Implementation: I11`.

- [ ] **Step 11: "`cxsdata` when `cxsel = 0`"** — extract to Choices #12:
`12 * cxsdata access when cxsel = 0 is left undefined per spec, with no additional ISA-level restriction added — conforming software never observes this since cx_save/cx_restore return 0 for the builtin selection.`
Replace resolution prose with `Implementation: I12`.

- [ ] **Step 12: Verify**

Run: `grep -n '^[0-9]' qemu-cxtg/Implementation_Choices.md | tail -8`
Expected: entries 7 through 12 present, all marked `*`.

- [ ] **Step 13: Commit**

```bash
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg add Composable-Extensions.md Implementation_Choices.md
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg commit -m "docs: trim Composable-Extensions.md Discussion items 1-12, extract to Choices"
```

---

### Task 6: `Composable-Extensions.md` Discussion trim — items 13-26

**Files:**
- Modify: `qemu-cxtg/Composable-Extensions.md`
- Modify: `qemu-cxtg/Implementation_Choices.md`

**Interfaces:**
- Consumes: Choices entries 1-12 (Tasks 2, 4, 5). Next available number is **13**. No References section yet — citations converted this task get placeholder anchors resolved once Task 7 builds the References section (see Step-by-step note below).

**Note on citations before the References section exists:** items in this range cite `Requirements.md` and the 2026-08-14 TG meeting. Since the References section is built in Task 7 (after all Discussion items are scanned for citations), do the citation-anchor conversion in Task 7, not here — for this task, leave citation prose as-is except where it's bundled with a decision worth extracting to Choices (extract the decision now; leave the citation prose in place for Task 7 to convert).

- [ ] **Step 1: "Indirect mode `cxsel` bounds"** — no firm Choices extraction (explicitly "leaning toward," deferred to Phase 6). Leave the TG-discussion citation (2026-08-14 meeting) in place for Task 7. No change this task beyond confirming nothing here is extractable yet.

- [ ] **Step 2: "`scxxs` state machine"** — extract the adopted model to Choices #13:
`13 * scxxs adopts the full four-state model (Off/Initial/Clean/Dirty, priv spec §3.1.6.7 Table 12): Off traps on CX instructions; Initial/Clean→Dirty on state-modifying CX instructions; Dirty→Clean on context save; any non-Off→Initial on cxdiscard/scxdiscard; explicit scxxs write drives Off/enable transitions. Zero-sized-state CXs use only Off/Initial. Without Zcxmulti, scxxs is writable (like FS/VS); with Zcxmulti, scxxs is read-only summary (like XS) and scxNxs holds per-context status.`
Replace the resolution paragraph with `Implementation: I13`.

- [ ] **Step 3: "`scxNxs` as future indirect CSRs"** — extract to Choices #14:
`14 * scxNxs registers are implemented as indirect CSRs via Sscsrind starting in Phase 6, treating Sscsrind as a current requirement despite the spec's future-tense wording.`
Replace resolution prose with `Implementation: I14`.

- [ ] **Step 4: "`cxdiscard` vs. `scxdiscard`"** — extract only the firm part to Choices #15:
`15 * cxdiscard is implemented as a non-custom ISA opcode.`
Leave the rest (scxdiscard's delivery mechanism — still deferred among several options) as open prose. Add `Implementation: I15` inline next to the cxdiscard-opcode sentence, keep the rest of the "Tentative project assumptions" paragraph for the still-undecided scxdiscard delivery mechanism.

- [ ] **Step 5: "Smstateen default behavior for CX CSR access"** — extract to Choices #16:
`16 * CX CSR accesses raise illegal instruction when mstateen0.C = 0 (standard Smstateen behavior, no CX-specific deviation).`
Replace resolution prose with `Implementation: I16`.

- [ ] **Step 6: "`scxstp.mode = 2` when `Zcxmulti` is absent"** — this decision is already Choices #5 (Task 4, Step 3). Do not create a duplicate entry. Replace the item's "Project implementation: clamp mode 2 to 1..." paragraph with `Implementation: I5`.

- [ ] **Step 7: "CX thread-scoped selectors and OS context switch"** — extract to Choices #17:
`17 * cxsel is saved/restored as part of thread context via the kernel task_struct (analogous to fcsr). ucontext_t support (cx_open_ucontext) is deferred.`
Replace "status: Resolved in part..." prose with `Implementation: I17`.

- [ ] **Step 8: "Exclusive vs. shared CX instances on an OS supporting only one model"** — no extraction (unresolved, no decision). Leave as-is.

- [ ] **Step 9: "§isa-priv — privileged CSR presence condition"** — extract to Choices #18:
`18 * scxstp and scxxs0-scxxs3 are conditioned on Zcx + S-mode; scxNxs registers are conditioned on Zcxmulti + S-mode (narrower reading of the spec's single conditioning sentence).`
Replace resolution prose with `Implementation: I18`. **Do not** touch the near-identical inline note in the "Privileged CSRs" requirements section (outside Discussion) — that's out of this task's scope per Global Constraints (Discussion section only).

- [ ] **Step 10: "Single `cxsel` and the four custom opcode spaces"** — no Choices extraction (tabled by TG, no project decision). Leave the citation to `Requirements.md` for Task 7 to convert to `#R` anchors.

- [ ] **Step 11: "State management as a separate extension"** — no extraction (open, awaiting spec clarification). Leave as-is.

- [ ] **Step 12: "Replacement for the R (ready/busy) flag"** — no extraction (observation, not a firm decision — no CX exists yet to test against). Leave as-is.

- [ ] **Step 13: "`cx_status` CSR or `get_status` opcode"** — no Choices extraction (open). Leave the `Requirements.md` citation for Task 7.

- [ ] **Step 14: "CX initialization procedure and data leakage"** — extract the tentative procedure to Choices #19:
`19 * CX initialization: on initial cx_open, the S-mode runtime executes scxdiscard (if supported) or writes 0 to cxsdata at every cxsidx; XS remains Off until the init loop completes, then updates to Initial before cx_open returns a selector. CX-specific additional initialization happens after that.`
Replace the "For now, reading it as..." sentence with `Implementation: I19`. Leave the `Requirements.md` citation for Task 7.

- [ ] **Step 15: Verify**

Run: `grep -n '^[0-9]' qemu-cxtg/Implementation_Choices.md | tail -7`
Expected: entries 13 through 19 present, all marked `*`.

- [ ] **Step 16: Commit**

```bash
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg add Composable-Extensions.md Implementation_Choices.md
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg commit -m "docs: trim Composable-Extensions.md Discussion items 13-26, extract to Choices"
```

---

### Task 7: `Composable-Extensions.md` Discussion trim — items 27-44 + References section

**Files:**
- Modify: `qemu-cxtg/Composable-Extensions.md`
- Modify: `qemu-cxtg/Implementation_Choices.md`

**Interfaces:**
- Consumes: Choices entries 1-19 (Tasks 2, 4, 5, 6). Next available number is **20**.
- Produces: final Choices entries from Discussion; the new `## References` section; all citation anchors resolved throughout the whole Discussion section (this task also goes back and fixes the citations left pending in Task 6, Step 1 and Steps 10/13/14).

- [ ] **Step 1: "`cxsel-format.adoc` — typed `cxsel` layout"** — no extraction (open, awaiting clarification). Leave as-is.

- [ ] **Step 2: "§criteria — PC in the composable-state list"** — extract to Choices #20:
`20 * PC is treated as not writable composable state (interim reading pending spec clarification of the criteria-vs-rationale wording discrepancy).`
Replace "Project assumption until clarified..." with `Implementation: I20`.

- [ ] **Step 3: "CX CC — callee-saved `cxsel` vs. zero-on-entry"** — no extraction (open, awaiting clarification). Leave as-is.

- [ ] **Step 4: "Discovery path split across devicetree / hwprobe / prctl"** — extract to Choices #21:
`21 * UUID→CXID lookup for user space uses prctl(PR_RISCV_CX_QUERY), not hwprobe (hwprobe carries no UUIDs; per-CXID keys start at CX1).`
Add `Implementation: I21` after the relevant sentence; keep the rest of the analysis. Leave the `Requirements.md` citation for the References pass (Step 12 below).

- [ ] **Step 5: "Which unprivileged architecture model (Appendix C)"** — no Choices extraction (spec explicitly undecided; project's alignment is an observation, not a commitment). Leave the `Requirements.md` citation for Step 12.

- [ ] **Step 6: "§criteria — chapter still evolving"** — no extraction. Leave as-is.

- [ ] **Step 7: "§api Other TODOs"** — no extraction. Leave as-is.

- [ ] **Step 8: "'Valid'/'invalid' selector — several distinct senses"** — no Choices extraction (interpretive framework, not a value/behavior decision). This item is long relative to its content — trim for conciseness per the todo item's general instruction, but preserve the open question at the end about disabled-but-not-invalidated selectors.

- [ ] **Step 9: "§cx_select repeated text"** — no extraction (editorial artifact). Leave as-is.

- [ ] **Step 10: "Indirect mode table entry — intended use of the `V` bit"** — extract the firm part to Choices #22:
`22 * V's use beyond "clear traps, set does not" is non-normative and left to software (eviction, lazy context-switching, virtualization schemes). V is treated as analogous to the basis spec's ~cxe bit.`
Replace "Project assumption: yes, V's use..." with `Implementation: I22`. Leave the basis-spec citation (§2.2.1) and the 2026-08-14 meeting citation in place for Step 12 (References pass) — do not extract the proposed `V`→`XE`/`cxe` rename, it's a proposal, not adopted.

- [ ] **Step 11: "`scx_invalidate(idx)` — possible explicit signaling instruction"** — no extraction (project idea, not TG-raised, no decision). Leave as-is.

- [ ] **Step 12: "Number of contexts per hart"** — extract to Choices #23:
`23 * Project model requires all harts to see an identical CX topology (same dedicated number and size of contexts per hart) as the starting point; other topologies deferred.`
Replace "Project model starts with..." with `Implementation: I23`. Leave the `Requirements.md` citation for the References pass.

- [ ] **Step 13: "A third `scxstp` mode — 'Bare' (2026-08-14 TG meeting)"** — no Choices extraction (still open, no firm decision on encoding). Leave the meeting and `Requirements.md` citations for the References pass.

- [ ] **Step 14: "RV32/RV64 parity"** — no separate Choices extraction (restates a settled `Requirements.md` item + test-matrix implication, not a project-invented decision). Leave the `Requirements.md` citation for the References pass.

- [ ] **Step 15: "Machine-mode-only and machine+user-mode-only systems"** — extract the verified finding to Choices #24:
`24 * M-mode-only and M+U-mode-only systems use the same virt machine/CPU type as S-mode configs; pass s=false,h=false (h implies s) via -cpu; no separate machine/CPU definition needed. (Verified 2026-08-18: -cpu rv64,s=false,h=false,zcx=on boots virt and passes block1_1_csr_stubs.elf.)`
Add `Implementation: I24` after the verification sentence; keep the rest. Leave the `Requirements.md` citation for the References pass.

- [ ] **Step 16: "State context size bound (512 KiB)"** — extract the corrected (flat-clamp) decision to Choices #25:
`25 * cxsidx WARL behaviour: cxsidx & ((512 * KiB / sizeof(target_ulong)) - 1) — flat clamp to 131071 (RV32) / 65535 (RV64), same bound for every CX.`
Replace the "Project assumption: Option 1..." sentence with `Implementation: I25`, and correct the surrounding prose so the item no longer presents Option 1/Option 2 as the live project stance — reword to note the TG still hasn't settled the *true* upper bound question generally, but this project's own implementation uses the flat clamp. Leave the `Requirements.md` citation for the References pass.

- [ ] **Step 17: "255 CXs vs. 64 `scxxs`/`scxNxs` status slots"** — no Choices extraction (open, no decision — "worth raising with the TG"). Leave the `Requirements.md` citation for the References pass.

- [ ] **Step 18: "Performance monitors and debug/trace — excluded"** — no separate Choices extraction (restates a `Requirements.md` exclusion, scope-tracking not an independent decision). Leave the `Requirements.md` citation for the References pass.

- [ ] **Step 19: Build the References section**

Scan the whole Discussion section (now trimmed) for every remaining inline citation to `Requirements.md`, a dated TG meeting, the basis spec / `basis_spec_comparison.md`, or a design deck. For each, replace the inline citation with a `#R<N>`/`#M<N>`/`#B<N>`/`#D<N>` anchor (numbered in the order encountered scanning top to bottom) and add the corresponding line to a new `## References` section at the end of the document (after the existing `## Discussion` section, before EOF):

```markdown
---

## References

**Requirements.md**
- #R1 — <first Requirements.md citation encountered, with enough context to identify which line/bullet it refers to>
- #R2 — ...
(continue for every Requirements.md citation found across the whole Discussion section, including ones from Tasks 5-6 that were left as prose)

**Meetings**
- #M1 — 2026-07-31 TG meeting, scxstp Indirect/table mode
- #M2 — 2026-08-14 TG meeting, ~0 WARL model + indirect mode topics (covers all 2026-08-14 citations — do not split into multiple meeting entries for the same date)

**Basis spec**
- #B1 — grayresearch/CX §2.2.1, cxe bit (Indirect mode V bit item)

**Design decks**
- #D1 — Darius & Jan, 2025-06-13, Context Management pre-draft (only if actually cited somewhere in Discussion — verify with `grep -n "2025-06-13" qemu-cxtg/Composable-Extensions.md` before adding; if not cited, omit this group entirely rather than adding an empty one)
```

This step touches every Discussion item that still has bare citation prose (from Task 6 Step 1, and Task 7 Steps 4, 5, 10, 12, 13, 14, 15, 16, 17, 18) — go back through each and swap the prose citation for the anchor.

- [ ] **Step 20: Verify**

Run: `grep -n '^[0-9]' qemu-cxtg/Implementation_Choices.md | tail -6`
Expected: entries 20 through 25 present, all marked `*`.

Run: `grep -n '#R[0-9]\|#M[0-9]\|#B[0-9]\|#D[0-9]' qemu-cxtg/Composable-Extensions.md | wc -l`
Expected: nonzero, matching the count of citations moved to References.

Run: `grep -n "^## References" qemu-cxtg/Composable-Extensions.md`
Expected: one match.

- [ ] **Step 21: Commit**

```bash
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg add Composable-Extensions.md Implementation_Choices.md
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg commit -m "docs: trim remaining Discussion items, add References section"
```

---

### Task 8: Wiki updates

**Files:**
- Create: `wiki/runtime-cxtg/qemu-cxtg/Implementation_Choices.md.md`
- Create: `wiki/runtime-cxtg/docs/todo_done_did.md.md`
- Modify: `wiki/runtime-cxtg/qemu-cxtg/Composable-Extensions.md.md` (if it exists — check first)
- Modify: `wiki/index.md`

**Interfaces:** none (no Choices entries this task)

- [ ] **Step 1: Check whether `Composable-Extensions.md` already has a wiki page**

Run: `find /home/cmd/Github/runtime-cxtg/wiki -iname "*Composable-Extensions*"`
If found, read it and update to reflect the trimmed Discussion section and new References section (Format D — self-explanatory doc, per `wiki/schema.md`). If not found, create it (Format D).

- [ ] **Step 2: Create the `Implementation_Choices.md` wiki page**

Follow `wiki/schema.md` Format D (self-explanatory file), depth 3 from `wiki/` (`wiki/runtime-cxtg/qemu-cxtg/Implementation_Choices.md.md`), so `index.md` path is `../../index.md`:

```markdown
*← [Main Index](../../index.md)* | *↗ [view source](../../../qemu-cxtg/Implementation_Choices.md)*

# qemu-cxtg/Implementation_Choices.md

Numbered, append-only log of project-specific implementation decisions that go beyond the spec and `docs/Requirements.md` — CSR/encoding placeholder values, WARL clamp behaviors, mode-legalization rules, state-machine mappings, and similar. Entries with a corresponding item in `Composable-Extensions.md`'s Discussion section are marked with a trailing `*`. Everything below the file's divider is not yet incorporated into `docs/CXTG_QEMU_Action_Plan.md`.
```

- [ ] **Step 3: Create the `todo_done_did.md` wiki page**

Depth 2 from `wiki/` (`wiki/runtime-cxtg/docs/todo_done_did.md.md`), `index.md` path `../../index.md`:

```markdown
*← [Main Index](../../index.md)* | *↗ [view source](../../../docs/todo_done_did.md)*

# docs/todo_done_did.md

Append-only archive of `docs/todo.md` items (or decided fragments split off from compound items) once cleared, moved here verbatim with a pointer to the corresponding `Implementation_Choices.md` entry where applicable. A historical record, not a live reference — see `Implementation_Choices.md` for current project decisions.
```

- [ ] **Step 4: Add both new pages to `wiki/index.md`**

Follow the existing table row format: `| [path/to/file](runtime-cxtg/path/to/file.md) | One-line description |`. Add rows for `qemu-cxtg/Implementation_Choices.md` and `docs/todo_done_did.md` in the appropriate alphabetical/structural position matching the existing table's ordering convention (read the current table first to match it).

- [ ] **Step 5: Verify**

Run: `wc -l /home/cmd/Github/runtime-cxtg/wiki/runtime-cxtg/qemu-cxtg/Implementation_Choices.md.md /home/cmd/Github/runtime-cxtg/wiki/runtime-cxtg/docs/todo_done_did.md.md`
Expected: both under 125 lines (Format D doesn't have the A→C conversion threshold, but confirm they're reasonably sized self-explanatory pages, not accidentally huge).

- [ ] **Step 6: Commit**

```bash
git -C /home/cmd/Github/runtime-cxtg add wiki/
git -C /home/cmd/Github/runtime-cxtg commit -m "docs: add wiki pages for Implementation_Choices.md and todo_done_did.md"
```

---

### Task 9: Rewrite `todo.md:130` (Action Plan sync item) and close out `todo.md:157`

**Files:**
- Modify: `docs/todo.md`
- Modify: `qemu-cxtg/Implementation_Choices.md` (only if the rewrite surfaces new decisions)
- Modify: `docs/todo_done_did.md`

**Interfaces:**
- Consumes: final Choices entry count from Task 7 (25, unless Task 8 added none — confirm before starting).

- [ ] **Step 1: Re-read `todo.md:130` ("sync CXTG_QEMU_Action_Plan.md...") against the current state**

By now `todo.md` and `Composable-Extensions.md` have changed substantially: `Composable-Extensions.md`'s Discussion section is trimmed with a References section; `Implementation_Choices.md` exists with 25 entries; `todo_done_did.md` exists. Read the item's full current text (it references the pre-trim state: "`## Open Questions` heading was renamed to `## Discussion`," resolved/reworded/added items, etc. — all still historically accurate framing, but now incomplete since it doesn't mention Choices).

- [ ] **Step 2: Rewrite the item in place**, maintaining its original intent (assess Action Plan staleness against current docs, don't execute the sync itself). Add:
- A note that `Implementation_Choices.md` now exists and the Action Plan sync work should treat it as a source (the "Supporting Documents" table addition this item already calls for should include `Implementation_Choices.md` alongside `Requirements.md`/`CHARTER.md`).
- A note that `docs/todo_done_did.md` now exists as a historical archive, distinct from the live `todo.md`.
- Keep all the existing sub-bullets (Block 8.1 matrix, Block 4.2 cxsidx bounds — **update this one**: it currently says "Project assumption is a WARL clamp of out-of-bounds values to 131071 (RV32) / 65535 (RV64)" — this is now `Implementation_Choices.md` #25, cite it directly instead of restating the assumption inline; the `[exclude]` scope-boundary check; the `[discuss]` items; the WBS/ratification weighing) — these remain accurate future-work instructions, just update citations to point at Choices where the item currently restates something now captured there.

- [ ] **Step 3: If the rewrite surfaces any new decision not already in Choices**, add it as a new entry (next number, 26+) following the same extraction rules as Tasks 4-7. (Expected: none — this is primarily an assessment/pointer-update pass, not new decision-making — but don't force it if something genuinely surfaces.)

- [ ] **Step 4: Mark `todo.md:157` ("documentation prep for Action Plan update") done and archive it**

This is the item that has driven this entire plan. Once Steps 1-3 are complete, mark its heading `## documentation prep for Action Plan update — DONE (2026-08-19)`, then move it verbatim to `docs/todo_done_did.md` (no `→ Choices` pointer — it's a meta/process item, not a single decision) and remove it from `todo.md`.

- [ ] **Step 5: Verify**

Run: `grep -n "^## " docs/todo.md`
Expected remaining headings: `CSR Addresses, instruction encodings, and extension names`; `decide on how we are dealing with ~0 in this implementation`; `sync CXTG_QEMU_Action_Plan.md with recent todo.md / Composable-Extensions.md edits` (rewritten); `sync README.md`.

- [ ] **Step 6: Commit**

```bash
git -C /home/cmd/Github/runtime-cxtg add docs/todo.md docs/todo_done_did.md
git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg add Implementation_Choices.md
git -C /home/cmd/Github/runtime-cxtg commit -m "docs: rewrite Action Plan sync item, archive documentation-prep todo"
```

---

### Task 10: Final review across both repos

**Files:** none (review only)

**Interfaces:** none

- [ ] **Step 1: Full diff review**

Run: `git -C /home/cmd/Github/runtime-cxtg diff cxtg-dev...todo/doc-rework --stat` and `git -C /home/cmd/Github/runtime-cxtg/qemu-cxtg diff cxtg-dev...todo/doc-rework --stat`
Read through both full diffs (not just stat) and confirm every file matches the spec's intent: `Implementation_Choices.md` entries are all plain-line format, no rationale prose; `todo.md` retains only genuinely open items; `todo_done_did.md` has verbatim archived entries; `Composable-Extensions.md`'s Discussion section is gap→status→pointer shaped throughout with a working References section; wiki pages exist and follow Format D.

- [ ] **Step 2: Cross-check against the spec**

Re-read `docs/superpowers/specs/2026-08-18-implementation-choices-doc-design.md` section by section and confirm each section's requirement has a corresponding change in the diff. Flag any gap to the user rather than silently filling it.

- [ ] **Step 3: Report to the user**

Summarize what changed (entry count in Choices, items archived, items still open in `todo.md`, References section groups populated) and ask "ready to commit?" / ready to merge `todo/doc-rework` → `cxtg-dev` in both repos per the branch workflow. Do not merge without explicit confirmation.

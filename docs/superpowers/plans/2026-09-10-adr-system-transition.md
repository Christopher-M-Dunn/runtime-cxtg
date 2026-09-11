# ADR System Transition Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Stand up the ADR (Architecture Decision Record) system (`docs/ADR_log.md` + `docs/adr/NNN.md`) and migrate every item currently spread across `docs/todo.md`, `docs/bugs.md`, and `qemu-cxtg/Implementation_Choices.md` into it. `docs/todo.md` becomes a one-line stub; `docs/bugs.md` and `qemu-cxtg/Implementation_Choices.md` are deleted outright (both are fully absorbed — `bugs.md` is already empty, `Implementation_Choices.md`'s 4 entries move to ADR-001/002/003).

**Architecture:** One global sequential ID across three categories (Decision / Bug / Note), tracked in four tables in `docs/ADR_log.md` (Decisions, Bugs, Notes, Archive), with one mandatory detail page per entry at `docs/adr/NNN.md`. No entry is ever deleted — only its status changes; the periodic archival sweep that moves terminal entries into Archive isn't triggered by this plan (it runs on a `feat/<block>` merge to `cxtg-dev`, which doesn't happen here).

**Tech Stack:** Markdown only, no QEMU code changes. Deleting `qemu-cxtg/Implementation_Choices.md` requires a commit inside the `qemu-cxtg` submodule plus a submodule-pointer bump in `runtime-cxtg`.

**Spec:** `docs/superpowers/specs/2026-09-10-adr-system-design.md` (this plan implements its §10 execution order), alongside `docs/superpowers/specs/2026-09-10-adr-system-statemachine.csv` (status-transition matrix) and `docs/flowchart_adr_life_cycle.drawio.svg` (lifecycle diagram).

## Global Constraints

- **Re-read before every edit, re-read after every write.** Standing rule from this repo's `CLAUDE.md` ("Session Behavior") — the user edits files directly, out of band. This applies to every step below even where not repeated; if a step's quoted `old_string` no longer matches the live file, find the equivalent current content and adjust — the edit's *intent* is authoritative, not the literal string captured when this plan was written.
- **Never commit without asking.** Both `CLAUDE.md` files require an explicit "Ready to commit?" before any `git add`/`git commit` — this overrides the writing-plans skill's default per-task commit step. Only the phase-checkpoint tasks (last task in each phase) stage/commit, and only after the user says yes.
- **Wiki update-on-write, with the `docs/adr/**` exemption.** Every file this plan touches outside `docs/adr/**` gets its wiki page updated in the same task. `docs/adr/**` (including `_schema.md` and every `NNN.md`) is exempt — Phase 1 records that exemption before any numbered `docs/adr/NNN.md` file is written.
- **Citation format:** an ADR is cited as a markdown link to its detail page, `[ADR-029](029.md)` from inside `docs/adr/*.md`, `[ADR-029](adr/029.md)` from `docs/ADR_log.md`. Every `ADR_log.md` row links the **ID** column (`| [ADR-029](adr/029.md) | ... |`); the **Title** column stays plain text. `Parent(s):`/`Children:` fields are links the same way. This is enforced in every file-content block below — read it, don't re-derive it.
- **ID assignment:** next ID = `ls docs/adr | sort -n | tail -1` + 1 (numeric sort treats `_schema.md` as `0`, so it's never picked up). Filenames are the zero-padded 3-digit ID.
- **Branch:** stay on the already-checked-out `todo/doc-rework` branch in both repos — no new branch. The normal rule is doc-only changes commit straight to `cxtg-dev`; this whole doc-rework effort is already the standing exception to that, predating this plan — it merges to `cxtg-dev` later, outside this plan's scope.
- **Submodule pointer:** any commit landing inside `qemu-cxtg` gets its gitlink bumped in the same `runtime-cxtg` commit that includes the rest of that phase's changes.
- **No scope creep:** don't touch `qemu-cxtg/Composable-Extensions.md`'s Discussion section (deferred to ADR-013), don't scan the QEMU source tree/`spec.md`/Action Plan for more entry candidates, don't execute the Action-Plan sync itself (ADR-009 documents it's owed; this plan doesn't do it).

---

## Phase 1 — Skeleton, schema, wiki exemption

### Task 1.1: Create `docs/ADR_log.md` + its wiki page

**Files:** Create `docs/ADR_log.md`, `wiki/runtime-cxtg/docs/ADR_log.md.md`; modify `wiki/index.md`.

- [ ] **Step 1: Write `docs/ADR_log.md`**

```markdown
# ADR Log

Single index of every Decision, Bug, and Note for this project — one global,
sequential ID across all three categories. Nothing is deleted; an entry
only changes status. Every entry has a detail page at `docs/adr/NNN.md`
(format and status vocabulary: `docs/adr/_schema.md`). See
`docs/flowchart_adr_life_cycle.drawio.svg` for the full status lifecycle
these tables track.

If this log's Status column and an entry's own Transitions table ever
disagree, this index is wrong — fix it to match the entry's page.

## Archival sweep

Reaching a terminal status does not by itself move an entry into Archive —
it stays visible in its live category table until an explicit sweep, run
immediately after a `feat/<block>` branch merges back into `cxtg-dev` (the
sweep is therefore done on `cxtg-dev` directly, consistent with docs-only
commits). On each sweep, review every terminal-status entry not yet
archived and propose moving it, in this order:

1. Title starts with `KEEP` → never propose archiving it.
2. Has an un-archived child → propose archiving only once every child is archived; otherwise keep, regardless of category.
3. Bug, `closed`, title starts with `FIXED` → propose archiving. A bug closed any other way (e.g. "unable to reproduce") only leaves by hand.
4. Decision, `incorporated` → propose archiving by default.
5. Note, `retired`, from any phase before the most recent milestone git tag → propose archiving.

If a proposed archival is declined, prefix that entry's title with `KEEP`
(in both this log's row and the entry's own `docs/adr/NNN.md` H1) so
future sweeps stop proposing it.

## Decisions

| ID | Status | Phase(s) | Title |
|---|---|---|---|

## Bugs

| ID | Status | Phase(s) | Title |
|---|---|---|---|

## Notes

| ID | Status | Phase(s) | Title |
|---|---|---|---|

## Archive

| ID | Status | Phase(s) | Title |
|---|---|---|---|
```

- [ ] **Step 2: Write the wiki page** (Format D, depth 2 like `wiki/runtime-cxtg/docs/todo.md.md`)

```markdown
---
format: D
---

*← [Main Index](../../index.md)* | *↗ [view source](../../../docs/ADR_log.md)*

# docs/ADR_log.md

Single index of every Decision, Bug, and Note for the project — one global
sequential ID across all three categories, in four tables (Decisions,
Bugs, Notes, Archive). Each row links to its detail page at
`docs/adr/NNN.md` (`docs/adr/**` is exempt from per-file wiki pages — see
`wiki/schema.md`).
```

Save as `wiki/runtime-cxtg/docs/ADR_log.md.md`.

- [ ] **Step 3: Add the `wiki/index.md` row** — between the `docs/20260810_...` and `docs/basis_spec_comparison.md` rows (digits sort before letters):

```
| [docs/ADR_log.md](runtime-cxtg/docs/ADR_log.md.md) | Single index of every Decision, Bug, and Note — replaces `todo.md`/`bugs.md`/`Implementation_Choices.md` |
```

### Task 1.2: Wiki exemption for `docs/adr/**`

**Files:** Modify `wiki/schema.md`, `CLAUDE.md`, `qemu-cxtg/CLAUDE.md`.

- [ ] **Step 1:** In `wiki/schema.md`, end of the "Content rules" → "Never" list (just before `## wiki/index.md entry format`), add:

```
**Exempt:**
- `docs/adr/**` — exempt from wiki pages entirely, per `docs/superpowers/specs/2026-09-10-adr-system-design.md` §7.2.
```

- [ ] **Step 2:** In `CLAUDE.md`'s `### wiki/` subsection, after "This rule applies to all files in both `runtime-cxtg/` and `qemu-cxtg/`.", append: "Exception: `docs/adr/**` is exempt from wiki pages (see `wiki/schema.md`)."

- [ ] **Step 3:** In `qemu-cxtg/CLAUDE.md`'s `## Wiki` section, at the end of its final "New files:" sentence, append: "Exception: `docs/adr/**` in `runtime-cxtg` is exempt from wiki pages (see `runtime-cxtg/wiki/schema.md`)."

### Task 1.3: Create `docs/adr/_schema.md`

Now safe to create a `docs/adr/**` file — Task 1.2 already put the exemption on record.

- [ ] **Step 1: Write `docs/adr/_schema.md`**

```markdown
# ADR Entry Schema

Reference template and rules for `docs/adr/NNN.md` entries and their
`docs/ADR_log.md` index rows. Not itself a numbered entry — its filename
starts with `_` specifically so `ls docs/adr | sort -n | tail -1` (used to
assign the next ID) never picks it up as the highest-numbered file.

## Categories and status progression

| Category | Status progression |
|---|---|
| **Decision** | `deferred` → `pending` → `in progress` (only if the ADR was created mid-phase) → `incorporated`, or `superseded` from any point |
| **Bug** | `open` → `closed` |
| **Note** | `active` → `retired` |

`superseded` is a Decision-only status — Bugs and Notes never carry it. A
Note's displacement is recorded as prose (in the body, or a Transitions
row's Notes column, citing the other entry), not as a status value; a
formal `supersede` transition is reserved for architectural,
Action-Plan-level Decisions.

## ID assignment and citation format

- **Next ID** = highest existing filename in `docs/adr/` + 1: `ls docs/adr | sort -n | tail -1`.
- **Citation:** always a link to the detail page, `[ADR-029](029.md)` from within `docs/adr/`, `[ADR-029](adr/029.md)` from `docs/ADR_log.md`. Never a bare `ADR-029` string.
- **`ADR_log.md` row format:** `| [ADR-029](adr/029.md) | <status> | <phase(s)> | <Title, plain text> |` — the ID column carries the link; the Title column does not.

## Entry template (`docs/adr/NNN.md`)

```
# ADR-NNN: <Title>

**Phase(s):** <phase/block id(s), `all`, or `?`>
**Parent(s):** [ADR-XXX](XXX.md), ... (omit if none)
**Children:** [ADR-YYY](YYY.md), ... (omit if none)

<Body. No length constraint and no category-based shape requirement — a
Decision can carry as much rationale, considered-and-rejected options, or
scratch reasoning as a Bug's repro steps or a Note's guidance text need.
Only the ADR_log.md index line has to stay a one-sentence summary; this
page is where the actual substance lives. An entry with several dates
that matter to its narrative can and should tell that story here in
prose — the Transitions table below is a status-change ledger, not the
only place a date is allowed to appear.>

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| YYYY-MM-DD | <status> | ... |
```

- No separate "current status" field — it's already in the `ADR_log.md`
  row and this page's own Transitions table.
- The Transitions table's first row is the entry's creation, at whatever
  status it started in. For an entry migrated from content with a
  real-world history, that real date goes in the creation row's Notes
  column (or the body, if there's more than one date worth telling) —
  never invented as if the ADR itself existed then.
- When an entry spawns a child, edit both: the child's `Parent(s)` line,
  and (if useful) the parent's `Children` line.
```

---

### Task 1.4 (Checkpoint): Review and commit Phase 1

- [ ] **Step 1:** Show the user the diff/new files for Phase 1; confirm `git status --porcelain` in both repos matches expectations.
- [ ] **Step 2:** Ask "Ready to commit?" — wait for yes.
- [ ] **Step 3:** Commit in `qemu-cxtg`:

```bash
git -C qemu-cxtg add CLAUDE.md
git -C qemu-cxtg commit -m "$(cat <<'EOF'
docs: exempt docs/adr/** from the wiki update-on-write rule

Part of the ADR system rollout (runtime-cxtg docs/superpowers/specs/2026-09-10-adr-system-design.md).

Co-Authored-By: Claude Sonnet 5 <noreply@anthropic.com>
EOF
)"
```

- [ ] **Step 4:** Commit in `runtime-cxtg`, bumping the submodule pointer:

```bash
git add docs/ADR_log.md docs/adr/_schema.md wiki/runtime-cxtg/docs/ADR_log.md.md wiki/index.md wiki/schema.md CLAUDE.md qemu-cxtg
git commit -m "$(cat <<'EOF'
docs: create the ADR log skeleton and schema, exempt it from wiki updates

First phase of the ADR system rollout per docs/superpowers/specs/2026-09-10-adr-system-design.md.

Co-Authored-By: Claude Sonnet 5 <noreply@anthropic.com>
EOF
)"
```

- [ ] **Step 5:** `git status --porcelain && git -C qemu-cxtg status --porcelain` — both clean (aside from `linux-cxtg/`).

---

## Phase 2 — Migrate `qemu-cxtg/Implementation_Choices.md`

Each entry merges an `Implementation_Choices.md` line with the `docs/todo.md` section stating the same fact (spec §5's overlap-watch rule).

### Task 2.1: ADR-001 — Build flag architecture

**Files:** Create `docs/adr/001.md`; modify `docs/ADR_log.md`, `docs/todo.md`, `qemu-cxtg/Implementation_Choices.md`.

- [ ] **Step 1: Write `docs/adr/001.md`**

```markdown
# ADR-001: Build flag architecture — `ext_zcx` / `ext_zcxmulti`

**Phase(s):** all

Only two new runtime extension flags are needed: `ext_zcx` and `ext_zcxmulti`.

No Kconfig compile-time `CONFIG_*` flags are required. All behavior gating is done at runtime via predicate functions and handler checks against already-present QEMU extension flags. This is the standard QEMU RISC-V extension pattern.

**Runtime flags:**
- `ext_zcx` — enables the base Zcx extension; user sets `-cpu rv64,zcx=on`. Already registered (`target/riscv/cpu.c`: `ISA_EXT_DATA_ENTRY(zcx, ...)`, `MULTI_EXT_CFG_BOOL("zcx", ext_zcx, false)`; `target/riscv/cpu_cfg_fields.h.inc`: `BOOL_FIELD(ext_zcx)`).
- `ext_zcxmulti` — enables ZcxMulti; requires both `zcx=on` AND `sscsrind=on` per spec; predicate enforces both. Not yet added to the code — planned for Phase 6 (ZcxMulti: Per-context Status and Indirect Mode).

**Behavior conditioning at runtime** (no new flags — check existing QEMU state):
- S-mode features (`scxstp`, `scxxs`): predicate checks `riscv_has_ext(env, RVS)`
- `Smstateen` gating: check `riscv_cpu_cfg(env)->ext_smstateen`
- `Sscsrind` for `scxNxs`: check `riscv_cpu_cfg(env)->ext_sscsrind`
- Hypervisor (`hstateen`): check `riscv_has_ext(env, RVH)`

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

**Open question:** does the `riscv_has_ext(env, RVS)` check behave correctly for test configurations that disable S-mode entirely (e.g. `-cpu rv64,s=false,h=false,zcx=on`, used elsewhere in the project to exercise M-mode-only per [ADR-010](010.md))? Not yet verified.

Block 8.1's build matrix is a test matrix of `-cpu` flag combinations against a single binary — no recompilation needed.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | incorporated | Migrated from `docs/todo.md`'s "Build Flag Architecture" section and `qemu-cxtg/Implementation_Choices.md` entry 1 (merged — both stated this decision). Originally decided 2026-05-19 (Phase 0); `ext_zcx` already implemented, `ext_zcxmulti` still pending Phase 6. |
```

- [ ] **Step 2:** Add to `docs/ADR_log.md`'s `## Decisions`:

```
| [ADR-001](adr/001.md) | incorporated | all | Build flag architecture — `ext_zcx` / `ext_zcxmulti` |
```

- [ ] **Step 3:** Remove `docs/todo.md`'s `## Build Flag Architecture (Phase 0 decision — 2026-05-19)` section (heading through its trailing `---` divider) — fully captured above.

- [ ] **Step 4:** Remove `qemu-cxtg/Implementation_Choices.md`'s entry `1 - Runtime extension flags: ...`.

- [ ] **Step 5: Verify** — `grep -rn "Build Flag Architecture" docs/ qemu-cxtg/*.md` → no output.

### Task 2.2: ADR-002 + ADR-003 — CSR addresses/encodings, and its implementer note; delete `Implementation_Choices.md`

**Files:** Create `docs/adr/002.md`, `docs/adr/003.md`; modify `docs/ADR_log.md`, `docs/todo.md`; delete `qemu-cxtg/Implementation_Choices.md`.

- [ ] **Step 1: Write `docs/adr/002.md`**

```markdown
# ADR-002: CSR addresses, instruction encodings, and extension names

**Phase(s):** all
**Children:** [ADR-003](003.md)

Final CSR addresses, instruction encodings, and extension name(s) are not yet assigned — they will be assigned by the RISC-V org. This entry, and therefore its `deferred` status, is permanent and not to be removed until the project ends: the values below are the project's current working stubs, subject to change on final assignment.

**Current values:**

| Symbol | File | Current Value | Pending |
|--------|------|---------------|---------|
| `CSR_CXSEL` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0xCA0` | Final address pending RISC-V org assignment |
| `CSR_CXSIDX` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x018` | Final address pending RISC-V org assignment |
| `CSR_CXSDATA` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x019` | Final address pending RISC-V org assignment |
| `MATCH_CXSETSEL` | `qemu-cxtg/target/riscv/cpu_bits.h` | `0x00004073` (SYSTEM, funct3=4, funct7=0) | Final encoding pending RISC-V org assignment |

`cxdiscard` opcode: TBD, likely SYSTEM major opcode, funct3=4, funct7=1 in this implementation — not yet assigned even a working stub value.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | deferred | Migrated from `docs/todo.md`'s "CSR Addresses, instruction encodings, and extension names" section and `qemu-cxtg/Implementation_Choices.md` entries 2–4 (merged). Working values already implemented as stubs since Phase 1 Block 1.1; `cxdiscard` not yet assigned even a stub. Spawned child ADR-003 (implementer comment directive). |
```

- [ ] **Step 2: Write `docs/adr/003.md`**

```markdown
# ADR-003: Mark provisional CSR/encoding values wherever used in code

**Phase(s):** all
**Parent(s):** [ADR-002](002.md)

Spawned from ADR-002 (CSR addresses/encodings pending RISC-V org assignment). Implementer note: wherever `CSR_CXSEL`, `CSR_CXSIDX`, `CSR_CXSDATA`, `MATCH_CXSETSEL`, or the future `cxdiscard` encoding is used in code, add a comment marking it provisional pending final assignment (e.g. `/* provisional value, pending RISC-V org CSR/encoding assignment — see ADR-002 */`), so a future rename/renumber pass can find every call site by searching for that comment.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | active | Spawned from ADR-002 during the ADR-system migration. |
```

- [ ] **Step 3:** Add to `docs/ADR_log.md`'s `## Decisions` (after ADR-001):

```
| [ADR-002](adr/002.md) | deferred | all | CSR addresses, instruction encodings, and extension names |
```

and to `## Notes` (first row):

```
| [ADR-003](adr/003.md) | active | all | Mark provisional CSR/encoding values wherever used in code |
```

- [ ] **Step 4:** Remove `docs/todo.md`'s `## CSR Addresses, instruction encodings, and extension names` section (heading through its trailing `---`) — fully captured above.

- [ ] **Step 5:** `rm qemu-cxtg/Implementation_Choices.md` (only entries 2–4 remain after Task 2.1; the whole file is now superseded).

- [ ] **Step 6: Verify** — `grep -rln "Implementation_Choices" . --include="*.md" | grep -v superpowers/specs` → no output (remaining mentions are historical, inside the two superseded 2026-08-18/2026-08-19 spec/plan files).

### Task 2.3 (Checkpoint): Review and commit Phase 2

- [ ] **Step 1:** Show the user `docs/adr/001–003.md`, the `ADR_log.md` diff, the `docs/todo.md` diff, and confirm `Implementation_Choices.md` is deleted.
- [ ] **Step 2:** Ask "Ready to commit?" — wait for yes.
- [ ] **Step 3:** Commit in `qemu-cxtg`:

```bash
git -C qemu-cxtg add -u Implementation_Choices.md
git -C qemu-cxtg commit -m "$(cat <<'EOF'
docs: remove Implementation_Choices.md, superseded by ADR_log.md

Its 4 entries are now ADR-001/002/003 in runtime-cxtg's docs/ADR_log.md.

Co-Authored-By: Claude Sonnet 5 <noreply@anthropic.com>
EOF
)"
```

- [ ] **Step 4:** Commit in `runtime-cxtg`, bumping the submodule pointer:

```bash
git add docs/adr/001.md docs/adr/002.md docs/adr/003.md docs/ADR_log.md docs/todo.md qemu-cxtg
git commit -m "$(cat <<'EOF'
docs: migrate Implementation_Choices.md into ADR-001/002/003

Phase 2 of the ADR system rollout.

Co-Authored-By: Claude Sonnet 5 <noreply@anthropic.com>
EOF
)"
```

- [ ] **Step 5:** `git status --porcelain && git -C qemu-cxtg status --porcelain`.

---

## Phase 3 — Migrate `docs/todo.md`'s remaining items

### Task 3.1: ADR-004 — `scxstp.mode=2` clamp

- [ ] **Step 1: Write `docs/adr/004.md`**

```markdown
# ADR-004: `scxstp.mode=2` (Indirect) when ZcxMulti is absent

**Phase(s):** 6.3

`scxstp` is WARL, so writing `mode=2` (Indirect) with `ext_zcxmulti` disabled is legalized on write, not deferred to a trap on use. See `Composable-Extensions.md` Discussion, "`scxstp.mode = 2` when `Zcxmulti` is absent" for the spec-compliance reasoning.

**Project implementation:** clamp `mode=2` to `mode=1` (Direct), with all other bits = 0. Any write with a reserved mode (3–15) is ignored (WARL: illegal write has no effect).

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | pending | Migrated from `docs/todo.md`'s "scxstp.mode=2 (Indirect) when ZcxMulti absent (Block 6.3)" section. Resolved 2026-07-20; not yet implemented — booked to Block 6.3, not yet started. |
```

- [ ] **Step 2:** Add to `docs/ADR_log.md`'s `## Decisions` (after ADR-002): `| [ADR-004](adr/004.md) | pending | 6.3 | \`scxstp.mode=2\` (Indirect) when ZcxMulti is absent |`

- [ ] **Step 3:** Remove `docs/todo.md`'s `## scxstp.mode=2 (Indirect) when ZcxMulti absent (Block 6.3)` section — fully captured above.

### Task 3.2: ADR-005 + ADR-006 — `~0` decided model and its spawned Note

- [ ] **Step 1: Write `docs/adr/005.md`**

```markdown
# ADR-005: `cxsel` invalid-selector handling — the `~0` clamp model

**Phase(s):** 2.3
**Children:** [ADR-006](006.md)

The spec identifies `~0` (all-ones) as an invalid selector but does not require `~0` itself to be a legal value — only that `cxsel` be able to hold all valid selectors and at least one invalid one. Prior to writing `cxsel`, implementations may convert an invalid value into some other invalid value `cxsel` is capable of holding. The all-ones value may be used by software to aid debugging of uninitialized variables — but the spec gives no guaranteed way to check whether a selector is valid just from its value; the project assumes runtime tooling will have its own mechanisms for that.

**Decided model (2026-08-14):** all invalid selectors are illegal except `~0`; every illegal write clamps to `~0`. This maximizes the number of available selector values, since no bit pattern is reserved just to mark validity. It is spec-compliant today (WARL already permits clamping to any invalid value) and matches the `~0` half of the WARL-simplification proposal ([ADR-007](007.md)), so it has no dependency on that proposal being adopted.

**Options considered and rejected:**
- All valid selectors have MSB = 0; invalid selectors clamped to `~0`. Restricts the valid-selector space beyond what the spec requires — not adopted, superseded by the decided model above.
- Same as above, but invalid selectors are ORed with MSB, flipping the sign bit.
- All invalid selectors clamped to one of several negative sentinel values, each conveying its own metadata (e.g. offline, absent, busy). More granular debugging, more complicated logic.
- Customized clamping of certain values to certain other values, valid or not. Unclear if the spec even allows this; likely out of scope for this project if so.
- All invalid selectors legal, no clamping. Limits debugging: a selector's invalidity can only be discovered by attempting to execute a custom instruction against it.

**Caveat on debugging via `cxsel` readback:** the spec guarantees `~0` is always invalid and that *some* invalid value must be legal/storable, but not that `~0` specifically is that value — so comparing `cxsel`'s readback against `~0` (e.g. `bne`) is reliable only because it's *this project's own* WARL choice, not a portable spec guarantee. Any test relying on it gets a `// not portable, project-specific WARL choice` comment, removable if [ADR-007](007.md)'s proposal is adopted (which would make `~0` legal and required everywhere).

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | pending | Migrated from `docs/todo.md`'s "decide on how we are dealing with ~0 in this implementation" section. Decided model dated 2026-08-14; not yet implemented — no invalid-selector clamp exists yet in `qemu-cxtg/target/riscv/cx.c`. Booked to Block 2.3, not yet started. Spawned child ADR-006 (TG-chain review and open-question action) and cross-references ADR-007 (competing WARL-simplification proposal from the same TG meeting). |
```

- [ ] **Step 2: Write `docs/adr/006.md`**

```markdown
# ADR-006: Review TG message chain on `~0` and add an open question to `Composable-Extensions.md`

**Phase(s):** all
**Parent(s):** [ADR-005](005.md)

Spawned from ADR-005 (the `~0` invalid-selector decision). Still outstanding:
- Review the TG message chain on the `~0` / invalid-selector topic.
- Add an open question to the bottom of `Composable-Extensions.md`'s Discussion section capturing it, per that document's existing convention for project assumptions vs. open spec questions.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | active | Migrated from `docs/todo.md`'s "decide on how we are dealing with ~0 in this implementation" section — spun out as its own Note, distinct from the clamp decision itself (ADR-005). |
```

- [ ] **Step 3:** Add to `docs/ADR_log.md`: `## Decisions` (after ADR-004): `| [ADR-005](adr/005.md) | pending | 2.3 | \`cxsel\` invalid-selector handling — the \`~0\` clamp model |`. `## Notes` (after ADR-003): `| [ADR-006](adr/006.md) | active | all | Review TG message chain on \`~0\` and add an open question to \`Composable-Extensions.md\` |`

(`docs/todo.md`'s source section is removed in Task 3.3, once ADR-007 — the rest of the same section — is also written, so it's removed as one atomic edit.)

### Task 3.3: ADR-007 — WARL simplification proposal; remove the `~0` section

- [ ] **Step 1: Write `docs/adr/007.md`**

```markdown
# ADR-007: Proposed WARL simplification for `~0` and table validity (2026-08-14 TG meeting)

**Phase(s):** ?

**Do not implement until either Darius confirms he is on board, or the TG records a vote.** Strong agreement from attendees present at the 2026-08-14 TG meeting, but Darius was not there and has previously advocated for the current spec wording on this exact point. See [ADR-005](005.md) for the currently-decided (and currently spec-compliant) `~0` clamp model this proposal would supersede if adopted.

**Direct and Indirect mode, both:**
- `~0` (reads XLEN-wide all-ones, regardless of how many bits `cxsel` actually implements) stays as the reserved invalid selector, but becomes *always legal*.
- `~0` becomes *required* to be implemented, on the same footing as the built-in selector `0`.
- `~0` becomes the *only* selector that is both *legal* **and** *invalid*.
- *All* invalid values *must* read as `~0` via WARL — a hard tightening of the current spec's "may be converted to some other invalid value `cxsel` is capable of holding." [ADR-005](005.md)'s model already matches this; if adopted TG-wide, nothing changes there, and its readback-portability caveat no longer applies.

**Indirect mode only:**
- *All* table indices become valid *and* legal selectors regardless of entry contents — including `V = 0`, stale, and uninitialized entries. This would separate "is `cxsel` a legal selector value" from "does the indicated entry currently authorize dispatch," and may resolve the Indirect-mode `cxsel`-bounds Discussion item outright for in-range indices.
- Table size may become implementation-variable instead of the spec's fixed 1024 entries. Needs further discussion to work out unforeseen ramifications; smaller tables are attractive, larger tables a nice-to-have.

**Direct mode only** (briefly discussed, may need more consideration):
- All selectors not corresponding to real hardware become invalid and illegal.
- Open question: what determines "corresponds to real hardware" — a simple CXID range bound, CXID alone, CXID+context-index, or something that must also account for a hot-swappable PCI-like CXU slot, wherein the slot itself is present but the CXU is disabled/removed/powered down? A simple range bound is the only method guaranteed to be inexpensive, and everything else could be done inexpensively if carefully designed. Leaning toward a simple range bound, or some other simple check against runtime-immutable config.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | deferred | Migrated from `docs/todo.md`'s "decide on how we are dealing with ~0 in this implementation" section, "Proposed WARL simplification..." sub-part. Proposed 2026-08-14; blocked on external TG/Darius confirmation, not booked to any phase. |
```

- [ ] **Step 2:** Add to `docs/ADR_log.md`'s `## Decisions` (after ADR-005): `| [ADR-007](adr/007.md) | deferred | ? | Proposed WARL simplification for \`~0\` and table validity |`

- [ ] **Step 3:** Remove `docs/todo.md`'s entire `## decide on how we are dealing with ~0 in this implementation` section (heading through trailing `---`) — now fully migrated across ADR-005/006/007.

- [ ] **Step 4: Verify** — `grep -n "DECIDED MODEL\|WARL simplification" docs/todo.md` → no output.

### Task 3.4: ADR-008 — revisit `Composable-Extensions.md` (DONE item)

- [ ] **Step 1: Write `docs/adr/008.md`**

```markdown
# ADR-008: Revisit `Composable-Extensions.md` against the current draft spec

**Phase(s):** all

Full spec-to-doc comparison: compare `Composable-Extensions.md` to the spec source (`../composable-custom-extensions/build/composable-custom-extensions.html`, or `../composable-custom-extensions/src/` if easier to read, though the built doc is needed for ordering/section numbers/other meta info), going through the spec in order to ensure `Composable-Extensions.md` concisely states *all* the spec's requirements without omitting any implementation-relevant detail, and derives all of that info directly from the spec source with no unstated assumptions. Then review each existing open question at the bottom to check whether it's still open, citing any resolution or tentative working assumption — nothing gets removed even once resolved, since anything that was ever an open question likely needs eventual spec clarification.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | retired | Migrated from `docs/todo.md`'s "revisit Composable-Extensions.md (literal requirements) and its source, the current draft spec" section, marked done 2026-07-13. Historical record only. |
```

- [ ] **Step 2:** Add to `docs/ADR_log.md`'s `## Notes` (after ADR-006): `| [ADR-008](adr/008.md) | retired | all | Revisit \`Composable-Extensions.md\` against the current draft spec |`

- [ ] **Step 3:** Remove `docs/todo.md`'s `## revisit Composable-Extensions.md (literal requirements) and its source, the current draft spec` section.

### Task 3.5: ADR-009 + ADR-010 — sync Action Plan; incorporate Requirements.md (cites ADR-009)

- [ ] **Step 1: Write `docs/adr/009.md`**

```markdown
# ADR-009: Sync `CXTG_QEMU_Action_Plan.md` with recent `todo.md` / `Composable-Extensions.md` edits

**Phase(s):** all

`todo.md` and `Composable-Extensions.md` picked up a number of edits: `Composable-Extensions.md` has been rewritten after reanalyzing the spec source, and now includes more non-normative info this project treats as requirements where possible; its `## Open Questions` heading was renamed to `## Discussion`, and several Discussion items were resolved, reworded, or added. `todo.md`'s own title and scope note were tightened to explicitly separate *how* (`todo.md`, now `ADR_log.md`) from *what* (`Composable-Extensions.md`).

The stale `scxstp.mode=2` line in `CXTG_QEMU_Action_Plan.md`'s Block 6.3 notes was already caught and fixed as a one-off (2026-07-25), but the rest of the Action Plan has not been reviewed end-to-end against these changes. ADR-010 additionally flagged that its `§4` "Note on S-mode" claim (that M-mode-only requires a different machine/CPU definition) is stale — `-cpu rv64,s=false,h=false,zcx=on` boots the same `virt` machine M-mode-only and passes `block1_1_csr_stubs.elf` — and that claim is still not corrected.

**Still to do:**
- Read through `Composable-Extensions.md` in full — the whole document was updated. Keep it in context for anything that may need to change or be added in the Action Plan.
- Read through `CXTG_QEMU_Action_Plan.md` block by block against the current `ADR_log.md` and `Composable-Extensions.md`.
- Look for any block notes, implementation notes, or definitions of done that cite behavior, wording, or section names that have since changed (the renamed `## Discussion` heading and any other resolved Discussion items are the likely culprits).
- Update the `Supporting Documents` table and any other doc-scope description in the Action Plan if it still blurs the how/what split.
- Read `docs/Requirements.md` and assess its effect on block scope and phase ordering. It is TG intent writ large, which is what the Action Plan seeks to capture at this time. The system in place deliberately tolerates minor changes to the requirements, since actual execution plans are only built one subphase at a time and only when needed. Prioritize settled items where possible, defer `[discuss]` items where possible, and confirm no block is aimed at a target the TG has already `[excluded]`:
  - Block 8.1's matrix varies runtime extension flags (`zcx`/`zcxmulti`/`sscsrind`/`smstateen`) only. Settled requirements call for RV32 *and* RV64 without preference, and for M-only and M+U-only systems — none of which any block currently varies. Decide whether 8.1 expands or a new block is added.
  - Block 4.2's `cxsidx` bounds work has no concrete state size; the settled requirement is zero to at least 512 KiB. Project assumption is a WARL clamp of out-of-bounds values to 131071 (RV32) / 65535 (RV64); `cxsdata` read/write with an invalid `cxsidx` is UNDEFINED.
  - The `[exclude]` items (dynamic state context resizing, dynamic context count, performance monitors, debug/trace) confirm scope the Action Plan can stay out of — check no block has drifted toward them.
  - The `[discuss]` items cluster on multi-instance/multi-context, which is Phase 6 territory — weigh against Phase 6 already sitting on the priv ISA the TG has no consensus on.
  - Add `docs/Requirements.md` and `docs/CHARTER.md` to the `Supporting Documents` table, with scope notes distinguishing TG intent from literal spec requirements.
- Also weigh the TG's WBS and ratification plans (`wbs.md`/`wbs.csv` and the Ratification Plan Milestone Review pages, https://riscv.atlassian.net/wiki/spaces/CCEX/; offline copies kept outside this repo): the spec ratifies as three separate packages (Unpriv ISA / Priv ISA / Non-ISA), each carrying its own `freeze-isa-sim dep=poc-qemu` checklist item. The unpriv package is the one that can actually move (graded A; `spec-isa-unpriv progress=10%`), while `spec-isa-priv` has no recorded progress. Consider whether the Action Plan should define an "unpriv-package-complete" milestone, and note that `poc-qemu-sampleext` is the only PoC deliverable with no spec dependency at all — Phase 4.1 is safe to build against no matter how the spec moves.
- Use `superpowers:brainstorming` to ask clarifying questions before making structural changes to the Action Plan.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | active | Migrated from `docs/todo.md`'s "sync CXTG_QEMU_Action_Plan.md with recent todo.md / Composable-Extensions.md edits" section. Still fully outstanding; executing this sync is out of scope for the ADR-system migration itself. |
```

- [ ] **Step 2: Write `docs/adr/010.md`**

```markdown
# ADR-010: Incorporate `Requirements.md` into `Composable-Extensions.md`'s `## Discussion`

**Phase(s):** all

`docs/Requirements.md` (offline copy of the TG's requirements page, draft 2026-08-13) records what the TG *wants to achieve*, not what the spec literally states, so none of it belongs in `Composable-Extensions.md`'s requirement sections — it goes in `## Discussion` only, cited as TG intent, siloed the same way basis-spec material is.

**Work done:** read `Requirements.md` in full, noting its three-way tagging (unmarked = proposed for inclusion, `[discuss]` = undecided, `[exclude]` = proposed for exclusion). New Discussion items were appended at the bottom of the section rather than grouped near related topics. Existing Discussion items were checked against `Requirements.md` and cited where it constrained them. `[exclude]` items that close off options under consideration (dynamic state context resizing, dynamic context count, performance monitors, debug/trace) were recorded as bounded so they wouldn't be reopened. `[discuss]` items overlapping existing open questions (more than one state context per hart; context count vs. hart count; more than one selected CX at a time) were merged into their existing Discussion items rather than duplicated. Unmarked (settled) requirements with no prior coverage got one new Discussion item each: RV32/RV64 parity, machine-mode-only/M+U-mode-only support, state-context size bound (0 to at least 512 KiB), the 255-CXs-vs-64-status-slots question, and the `cx_status`/accrued-error requirement merged into the existing `cx_status` Discussion item. Terminology was aligned to `Requirements.md`'s "all CXs are stateful, but some CXs have zero-sized state" framing.

**Outcomes that diverged from the original plan** (kept for the record rather than edited): the `cxsidx` WARL clamp project assumption changed mid-work to Option 1 (`CX[cxsel] STATE_NUM_WORDS-1`, a per-CX bound), with code structured to also support Option 2 (`MAX()` across all CXs), rather than the flat 131071/65535 clamp originally sketched; and the "Hart↔context cardinality" item's stance flipped from favoring the flexible (many-to-many) option to starting from a rigid identical-topology-per-hart requirement.

**Still owed, tracked separately:** `CXTG_QEMU_Action_Plan.md`'s §4 "Note on S-mode" claim — that M-mode-only requires a different machine/CPU definition — is stale (`-cpu rv64,s=false,h=false,zcx=on` boots the same `virt` machine M-mode-only and passes `block1_1_csr_stubs.elf`); this correction is folded into [ADR-009](009.md)'s Action-Plan sync scope rather than tracked here.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | retired | Migrated from `docs/todo.md`'s "incorporate Requirements.md into Composable-Extensions.md's ## Discussion" section, marked done 2026-08-18, with a 2026-08-19 note about the `cxsidx` WARL model change. Historical record only; the still-owed Action Plan correction is tracked under ADR-009. |
```

- [ ] **Step 3:** Add to `docs/ADR_log.md`'s `## Notes` (after ADR-008):

```
| [ADR-009](adr/009.md) | active | all | Sync `CXTG_QEMU_Action_Plan.md` with recent `todo.md` / `Composable-Extensions.md` edits |
| [ADR-010](adr/010.md) | retired | all | Incorporate `Requirements.md` into `Composable-Extensions.md`'s `## Discussion` |
```

- [ ] **Step 4:** Remove `docs/todo.md`'s `## sync CXTG_QEMU_Action_Plan.md with recent todo.md / Composable-Extensions.md edits` and `## incorporate Requirements.md into Composable-Extensions.md's ## Discussion` sections (re-read the live file to find each — they aren't necessarily adjacent).

- [ ] **Step 5: Verify** — `grep -n "^## sync CXTG_QEMU_Action_Plan\|^## incorporate Requirements" docs/todo.md` → no output.

### Task 3.6: ADR-011 — sync README.md

- [ ] **Step 1: Write `docs/adr/011.md`**

```markdown
# ADR-011: Sync `README.md`

**Phase(s):** all

Ensure `README.md` is current and gives useful, relevant information — especially orientation around the repo's structure, all documentation and images with their use and scope, and how to approach the wiki (its function, how pages are generated, `index.md`, best viewed in Obsidian with the root vault view set to the root repo (`runtime-cxtg/`, not `wiki/`, so links to source files work). Stay concise.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | active | Migrated from `docs/todo.md`'s "sync README.md" section — fully specified process item, entered directly at `active`. |
```

- [ ] **Step 2:** Add to `docs/ADR_log.md`'s `## Notes` (after ADR-010): `| [ADR-011](adr/011.md) | active | all | Sync \`README.md\` |`

- [ ] **Step 3:** Remove `docs/todo.md`'s `## sync README.md` section.

### Task 3.7: Confirm `docs/bugs.md` needs no migration

- [ ] **Step 1:** `wc -l docs/bugs.md` → expect `0` (per spec §5, this is a no-op check; deletion happens in Phase 4 alongside `docs/todo.md`'s stub).

### Task 3.8 (Checkpoint): Review and commit Phase 3

- [ ] **Step 1:** Show the user `docs/adr/004–011.md`, the `ADR_log.md` diff, and the `docs/todo.md` diff (should now contain only its header and the "documentation prep..."/"wiki lint" sections).
- [ ] **Step 2:** Ask "Ready to commit?" — wait for yes.
- [ ] **Step 3:** Commit in `runtime-cxtg` (no `qemu-cxtg` changes this phase):

```bash
git add docs/adr/004.md docs/adr/005.md docs/adr/006.md docs/adr/007.md docs/adr/008.md docs/adr/009.md docs/adr/010.md docs/adr/011.md docs/ADR_log.md docs/todo.md
git commit -m "$(cat <<'EOF'
docs: migrate docs/todo.md's remaining Decisions and Notes into ADR_log.md

Phase 3 of the ADR system rollout: ADR-004 through ADR-011.

Co-Authored-By: Claude Sonnet 5 <noreply@anthropic.com>
EOF
)"
```

- [ ] **Step 4:** `git status --porcelain`.

---

## Phase 4 — Bootstrapping Notes; stub `docs/todo.md`; delete `docs/bugs.md`

### Task 4.1: ADR-012 + ADR-013 — "documentation prep" parent and its spawned Discussion-rework child

- [ ] **Step 1: Write `docs/adr/012.md`**

```markdown
# ADR-012: Documentation prep for Action Plan update — pivot to the ADR system

**Phase(s):** all
**Children:** [ADR-013](013.md)

This item originally called for creating `qemu-cxtg/Implementation_Choices.md` (a flat numbered "Choices" log) and manually migrating `todo.md`'s deferred decisions into it. After brainstorming (`docs/superpowers/specs/2026-09-10-adr-system-design.md`), that plan was superseded before completion by the ADR (Architecture Decision Record) system: a single index `docs/ADR_log.md`, one global sequential number across three categories — Decisions, Bugs, and Notes — each with its own status lifecycle, replacing `todo.md`, `bugs.md`, and `qemu-cxtg/Implementation_Choices.md` outright.

This entry is the migration's origin point, executed via `docs/superpowers/plans/2026-09-10-adr-system-transition.md`. Everything from `todo.md`, `bugs.md`, and `Implementation_Choices.md` is now migrated into `ADR_log.md`/`docs/adr/*.md`; `todo.md` is reduced to a stub, `bugs.md` and `Implementation_Choices.md` are deleted. The one piece of this item's original scope still genuinely outstanding — reworking `Composable-Extensions.md`'s Discussion section for conciseness — is spawned off as its own child, ADR-013, rather than carried out as part of this migration.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | retired | Originally added to `docs/todo.md` at commit `9250385738fae0e6934f75c65ed9215a865be062` as "documentation prep for Action Plan update"; superseded in place on 2026-09-09 to record the ADR pivot. Retires now that the ADR system is stood up and populated, but stays in the live Notes table until its child, ADR-013, is also archived, per the archival-sweep child rule. |
```

- [ ] **Step 2: Write `docs/adr/013.md`**

```markdown
# ADR-013: Rework `Composable-Extensions.md`'s Discussion section for conciseness and ADR references

**Phase(s):** all
**Parent(s):** [ADR-012](012.md)

Go through `Composable-Extensions.md`'s Discussion section to improve conciseness: identify superfluous or duplicate info, and move references out to footnotes. Everything remaining should concisely identify the gap the spec doesn't cover, its resolution, and its implementation (e.g. `Implementation: ADR-029`). Move all implementation decisions into `ADR_log.md` and replace the inline text with a reference. Read the whole document first, then apply small edits one at a time so they can be reviewed.

Add a References section. For the reference format, use anchor links of the form Letter+Number, e.g. `#R2`, where the letter identifies a source-type/document group and the number is that item's index within it: `#R2` is the 2nd item under the `Requirements.md` group; `#M1` is the 1st item under the `Meetings` group (include the meeting date in the item); `ADR-029` is entry #29 in `ADR_log.md` (not part of the Letter+Number scheme — it's a direct citation into the ADR system).

This is deliberately not given its own spec/plan — it is carried out later by a separate agent working directly from this Note, once picked up.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | active | Spawned from ADR-012 during the ADR-system migration — the one piece of that item's original scope not completed as part of standing up `ADR_log.md` itself. |
```

- [ ] **Step 3:** Add to `docs/ADR_log.md`'s `## Notes` (after ADR-011):

```
| [ADR-012](adr/012.md) | retired | all | Documentation prep for Action Plan update — pivot to the ADR system |
| [ADR-013](adr/013.md) | active | all | Rework `Composable-Extensions.md`'s Discussion section for conciseness and ADR references |
```

### Task 4.2: ADR-014 — wiki lint

- [ ] **Step 1: Write `docs/adr/014.md`**

```markdown
# ADR-014: Wiki lint procedure

**Phase(s):** all

Make a procedure to check the wiki for consistency against its source files and `wiki/schema.md`'s rules — this was planned but never actually built; nothing under this name exists anywhere in the repo yet.

**Open question:** should this future lint procedure also validate `ADR_log.md`/`docs/adr/`'s own internal consistency (ID sequence has no gaps or duplicates, every log row has a matching `docs/adr/NNN.md` file and vice versa, a row's Status matches its file's own Transitions table, Parent/Children citations are reciprocal) — not just the rest of the wiki?

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | active | Migrated from `docs/todo.md`'s "wiki lint" section — fully specified process item, entered directly at `active`. |
```

- [ ] **Step 2:** Add to `docs/ADR_log.md`'s `## Notes` (after ADR-013): `| [ADR-014](adr/014.md) | active | all | Wiki lint procedure |`

### Task 4.3: ADR-015 — phase/block-level gate-check flowchart gap

- [ ] **Step 1: Write `docs/adr/015.md`**

```markdown
# ADR-015: Phase/block-level gate-check flow is undocumented beyond one sentence

**Phase(s):** all

`flowchart_adr_life_cycle.drawio.svg` diagrams entry *statuses and transitions* only. How a phase or block start actually checks `ADR_log.md` for open `deferred`/`pending`/`?`-phase(s) entries affecting it — the "gate check" `docs/README_docs.md` already references — is currently one prose sentence in the root `CLAUDE.md`'s "Starting a New Phase" step 2, and `qemu-cxtg/CLAUDE.md`'s per-block workflow has no gate-check step at all.

A dedicated procedure (ideally its own flowchart, matching the project's existing `flowchart_*` diagrams) is still needed. It is possible that the ideal solution keeps both: the flowchart for humans, and an efficient prose directive for Claude.

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | active | Created during the ADR-system migration to track a scope gap surfaced while writing its design spec — not itself part of the migration's scope to resolve. |
```

- [ ] **Step 2:** Add to `docs/ADR_log.md`'s `## Notes` (after ADR-014): `| [ADR-015](adr/015.md) | active | all | Phase/block-level gate-check flow is undocumented beyond one sentence |`

- [ ] **Step 3: Verify** — `ls docs/adr | sort -n` → `_schema.md` then `001.md`...`015.md`, no gaps.

### Task 4.4: Stub `docs/todo.md`; delete `docs/bugs.md`; update both `CLAUDE.md` files

**Files:** Modify `docs/todo.md`, `wiki/runtime-cxtg/docs/todo.md.md`, `CLAUDE.md`, `qemu-cxtg/CLAUDE.md`; delete `docs/bugs.md`, `wiki/runtime-cxtg/docs/bugs.md.md`; modify `wiki/index.md`.

- [ ] **Step 1:** Replace `docs/todo.md`'s entire content (at this point it should hold only its header and the fully-migrated "documentation prep..."/"wiki lint" sections) with:

```markdown
# CXTG TODO — superseded

All deferred implementation decisions, bugs, and process notes have moved
to [`docs/ADR_log.md`](ADR_log.md). This file is kept only so old links
and habitual lookups don't silently 404.
```

- [ ] **Step 2:** Update `wiki/runtime-cxtg/docs/todo.md.md`'s body to: "Superseded stub — all deferred decisions, bugs, and process notes moved to `docs/ADR_log.md`. Kept only so old links and habitual lookups don't silently 404."

- [ ] **Step 3:** `rm docs/bugs.md wiki/runtime-cxtg/docs/bugs.md.md` — it's empty and fully absorbed by `ADR_log.md`'s Bugs table; unlike `todo.md`, no stub is needed since it carried no content to begin with.

- [ ] **Step 4:** Remove `docs/bugs.md`'s row from `wiki/index.md`.

- [ ] **Step 5:** In `CLAUDE.md`, update the two `docs/todo.md` references:
  - "Current Work" section: "...and what is deferred in `docs/todo.md`" → "...and what is deferred, open, or noted in `docs/ADR_log.md`."
  - "Starting a New Phase" step 2: "Read `docs/todo.md` — resolve or confirm any open items not explicitly labelled for a later phase." → "Read `docs/ADR_log.md` — resolve or confirm any open `deferred`/`pending`/`?`-phase(s) entries not explicitly labelled for a later phase."

- [ ] **Step 6:** In `CLAUDE.md`'s "Documentation Files", replace the `### docs/bugs.md` subsection with:

```markdown
### `docs/ADR_log.md`

Single index of every Decision, Bug, and Note for the project — one global sequential ID across all three categories, each with its own status lifecycle. Every entry has a detail page at `docs/adr/NNN.md`; the format and status vocabulary for both are in `docs/adr/_schema.md`.

- **Read:** at the start of any session, to see what is deferred/pending/open; before starting a new phase or block, to check for any `deferred`/`pending`/`?`-phase(s) entries affecting it or a parent phase
- **Update:** add an entry for any new decision, bug, or note as it's identified; transition an entry's status (and its `docs/adr/NNN.md` Transitions table) as work on it progresses; run the archival sweep immediately after a `feat/<block>` branch merges into `cxtg-dev`

---
```

- [ ] **Step 7:** In `qemu-cxtg/CLAUDE.md`'s "Implementation status" line, replace "See `runtime-cxtg/docs/todo.md` for deferred implementation decisions." with "See `runtime-cxtg/docs/ADR_log.md` for deferred implementation decisions, bugs, and implementer notes."

- [ ] **Step 8: Verify** — `grep -rn "docs/todo\.md\|docs/bugs\.md" CLAUDE.md qemu-cxtg/CLAUDE.md docs/README_docs.md` → only `docs/todo.md`'s own self-reference should remain; fix anything else that turns up.

### Task 4.5 (Checkpoint): Final consistency check and commit

- [ ] **Step 1: Cross-check** — every `docs/adr/*.md` file has a matching `ADR_log.md` row and vice versa, and every `Parent(s)`/`Children` citation is reciprocal:

```bash
ls docs/adr | grep -v '^_schema.md$' | sed 's/\.md$//' | sort > /tmp/adr_files.txt
grep -oE 'ADR-[0-9]{3}' docs/ADR_log.md | sed 's/ADR-//' | sort -u > /tmp/adr_log_ids.txt
diff /tmp/adr_files.txt /tmp/adr_log_ids.txt   # expect no output
grep -rn "Parent(s):\|Children:" docs/adr/*.md  # expect ADR-005↔006, ADR-002↔003, ADR-012↔013 pairs
```

- [ ] **Step 2:** Show the user `docs/adr/012–015.md`, the final `docs/ADR_log.md` (15 rows across Decisions/Notes; Bugs and Archive still empty), the stubbed `docs/todo.md`, confirmation `docs/bugs.md` is gone, and both `CLAUDE.md` diffs.
- [ ] **Step 3:** Ask "Ready to commit?" — wait for yes.
- [ ] **Step 4:** Commit in `runtime-cxtg`:

```bash
git add docs/adr/012.md docs/adr/013.md docs/adr/014.md docs/adr/015.md docs/ADR_log.md docs/todo.md docs/bugs.md wiki/runtime-cxtg/docs/todo.md.md wiki/runtime-cxtg/docs/bugs.md.md wiki/index.md CLAUDE.md qemu-cxtg/CLAUDE.md
git commit -m "$(cat <<'EOF'
docs: complete the ADR system rollout — bootstrapping Notes, stub/remove legacy files

Phase 4 (final): ADR-012 through ADR-015. docs/todo.md is reduced to a
one-line stub pointing at docs/ADR_log.md; docs/bugs.md is deleted
(already empty, fully absorbed by ADR_log.md's Bugs table). Both
CLAUDE.md files' references are updated to match.

Co-Authored-By: Claude Sonnet 5 <noreply@anthropic.com>
EOF
)"
```

- [ ] **Step 5:** `git status --porcelain && git -C qemu-cxtg status --porcelain` — both clean (aside from `linux-cxtg/`).

---

## Self-Review Notes

- **Spec coverage:** §1/§2/§2.1 → Task 1.1; §3/§4/§7.1/§7.2 → Tasks 1.2/1.3; §5 migration of `Implementation_Choices.md` → Phase 2; §5 migration of `todo.md` → Phase 3; §6 → Task 4.4; §9 bootstrapping Notes → Phase 4. §8's scope boundaries are enforced by Global Constraints and by ADR-009/013's bodies stating that work is deferred, not done here.
- **Deviation from the spec, per direct user feedback on the first draft:** the spec's §6 only defines `docs/todo.md`'s stub fate; `docs/bugs.md` is deleted outright instead (it carries no content, so no stub is needed) rather than also stubbed.
- **New entry not in the original spec:** ADR-003, a child Note spawned from ADR-002, directing implementers to comment every call site of a provisional CSR/encoding value — added per direct feedback, shifting every subsequent ID up by one from the previous draft.
- **ID/status choices cross-checked against actual code state, not just `todo.md`'s prose:** ADR-001 `incorporated` despite `ext_zcxmulti` not yet existing (the *decision* is settled and partly acted on; Phase 6 completes applying it); ADR-004/ADR-005 `pending` rather than `incorporated` (both decided, neither yet coded per `qemu-cxtg/target/riscv/cx.c` and `progress.md`'s unchecked Blocks 2.3/6.3); ADR-002/ADR-007 `deferred` (external blockers — RISC-V org assignment, TG/Darius confirmation — neither booked to a phase).
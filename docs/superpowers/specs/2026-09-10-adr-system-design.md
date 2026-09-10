# ADR System — Design

**Source:** `docs/todo.md`, "documentation prep for Action Plan update" (originally added at commit `9250385738fae0e6934f75c65ed9215a865be062`; superseded and rewritten in place on 2026-09-09 to record this pivot).

**Supersedes:** `docs/superpowers/specs/2026-08-18-implementation-choices-doc-design.md` and its plan `docs/superpowers/plans/2026-08-19-implementation-choices-doc-rework.md`. That design (a single flat numbered `Implementation_Choices.md`) is abandoned before implementation completed — only its skeleton exists (`qemu-cxtg/Implementation_Choices.md` at commit `b181e10a0a`, 4 placeholder entries). Nothing from those two files carries forward except the entries already written, which this design's migration step ports over.

**Classification:** Architectural — introduces a new persistent document system (`docs/ADR_log.md` + `docs/adr/NNN.md`) with its own numbering, status, and citation conventions, replacing four existing files (`docs/todo.md` in full, `docs/bugs.md`, `docs/todo_archive.md`, `qemu-cxtg/Implementation_Choices.md`) and carving out an exception to `CLAUDE.md`'s wiki-update rule.

---

## 1. Goal

Give the project one place to record every implementation decision, bug, and implementer or process note — each with an explicit lifecycle — instead of the current spread across `todo.md` prose, `bugs.md`, and the abandoned `Implementation_Choices.md` skeleton. The system is an ADR (Architecture Decision Record) log: entries are never deleted, only transitioned between statuses, and every transition is dated.

**Everything in scope migrates — there is no "not ADR-shaped" carve-out.** A substantive design decision becomes a Decision; a behavioral defect becomes a Bug; anything else — including a plain process/procedural to-do like "sync README.md" — becomes a Note (§5 spells out that lifecycle). Nothing is left behind in a legacy file once migration finishes.

Three categories, one shared numbering sequence:

| Category | Status progression |
|---|---|
| **Decision** | `deferred` → `pending` → `in progress` (only if ADR created mid-phase) → `incorporated`, or `superseded` from any point |
| **Bug** | `open` → `closed` |
| **Note** | `active` → `retired` |

`superseded` is a **Decision-only** status — Bugs and Notes never carry it. When a Note is effectively displaced by something else, that relationship is recorded as prose (in the body, or a Transitions row's Notes column, citing the other entry) rather than as a status value; a formal `supersede` transition is reserved for architectural, Action-Plan-level Decisions.

The full state-transition/spawn matrix (which moves are valid, which are common vs. rare, worked examples for each) is finalized in `docs/superpowers/specs/2026-09-10-adr-system-statemachine.csv` and diagrammed in `docs/flowchart_adr_life_cycle.drawio.svg` — both already exist and are not part of this design's scope; they're the authority this design implements storage for.

---

## 2. `docs/ADR_log.md`

New file, wiki Format D.

**Structure:** four tables, each carrying a **Phase(s)** column — the Action Plan phase/block(s) an entry affects (e.g. `1.2`), `all` for a project-wide item, or `?` if not yet determined. This is what lets a phase start actually check for open work affecting it (`docs/README_docs.md`'s existing "check for any `deferred`/`pending`/`?`-phase(s) ADRs" language).

```
# ADR Log

<intro: one global sequential ID across Decisions/Bugs/Notes; every entry
gets a detail page at docs/adr/NNN.md — see that page's format below.
An entry may move between tables if its category changes; it is never deleted.>

## Decisions

| ID | Status | Phase(s) | Title |
|---|---|---|---|
| ADR-003 | pending | 1.2 | ... |

## Bugs

| ID | Status | Phase(s) | Title |
|---|---|---|---|
| ADR-007 | open | 2.1 | ... |

## Notes

| ID | Status | Phase(s) | Title |
|---|---|---|---|
| ADR-011 | active | all | ... |

## Archive

| ID | Status | Phase(s) | Title |
|---|---|---|---|
| ADR-001 | incorporated | 0.1 | ... |
| ADR-002 | closed | 1.1 | ... |
```

- The **Title** column links to the entry's `docs/adr/NNN.md`.
- **Decisions/Bugs/Notes tables** hold every entry in that category except ones that have passed an archival sweep (§2.1) — that includes ordinary non-terminal entries *and* terminal ones still awaiting their sweep.
- **Archive** holds only entries that have passed a sweep, sorted by ID, merged across all three categories. No separate Category column: each terminal status value belongs to exactly one category (`incorporated`/`superseded` → Decision, `closed` → Bug, `retired` → Note), so Status alone identifies it.
- If the log's Status column and an entry's own Transitions table (§3) ever disagree, the index is wrong — fix the index to match.

### 2.1 Archival sweep

Reaching a terminal status does not by itself move an entry into Archive — it stays visible in its live category table until an explicit sweep runs, immediately after a `feat/<block>` branch merges back into `cxtg-dev`(the sweep is therefore to be done on `cxtg-dev` directly, consistent with docs-only commits). On each sweep, review every terminal-status entry not yet archived and propose moving it, in this order:

1. **Title starts with `KEEP`** → never propose archiving it.
2. **Has an un-archived child** → propose archiving only once every child is archived; otherwise keep, regardless of category.
3. **Bug, `closed`, title starts with `FIXED`** → propose archiving. A bug closed any other way (e.g. "unable to reproduce") is not proposed — it only leaves by hand.
4. **Decision, `incorporated`** → propose archiving by default.
5. **Note, `retired`, from any phase before the most recent milestone git tag** → propose archiving.

Net effect: entries with un-archived children never leave on their own; Notes default to lingering until a milestone is reached; non-`FIXED` closed Bugs only leave by hand; incorporated Decisions leave by default. If the user declines a proposed archival, prefix that entry's title with `KEEP` so future sweeps stop proposing it.

---

## 3. `docs/adr/NNN.md`

New directory, one file per entry, no exceptions — no length threshold, unlike the old Choices design's single-line-per-entry format. This is a deliberate simplification: every entry gets the same page structure, so there's no per-entry judgment call about whether it "deserves" a page, and it's what lets ID assignment work by directory listing (§4). Filenames are the zero-padded 3-digit ID: `029.md` for `ADR-029`.

**Structure:**

```
# ADR-NNN: <Title>

**Phase(s):** <phase/block id(s), `all`, or `?`>
**Parent(s):** ADR-XXX, ... (omit if none)
**Children:** ADR-YYY, ... (omit if none)

<Body. No length constraint and no category-based shape requirement — a
Decision can carry as much rationale, considered-and-rejected options, or
scratch reasoning as a Bug's repro steps or a Note's guidance text need.
Only the ADR_log.md index line has to stay a one-sentence summary; this
page is where the actual substance lives. An entry with several dates
that matter to its narrative (e.g. a TG meeting on one date, a project
decision on another) can and should tell that story here in prose — the
Transitions table below is a status-change ledger, not the only place a
date is allowed to appear.>

---

## Transitions

| Date | Status | Notes |
|---|---|---|
| 2026-09-10 | pending | ... |
```

- **No separate "current status" header field on the page itself** — it's already recorded in the `ADR_log.md` row and this page's own Transitions table.
- **The Transitions table's first row is the entry's creation**, at whatever status it started in (`deferred`/`pending` for a Decision, `open` for a Bug, `active` for a Note). For an entry migrated from existing content that already has a real-world history (e.g. a todo.md item marked "DECIDED — 2026-08-14"), that real date goes in the creation row's **Notes** column, or in the body per above if the history has more than one date worth telling — never invented as if the ADR itself existed back then.
- **Parent/Children** citations are how the "child ADR" spawn mechanism (statemachine.csv, `flowchart_adr_life_cycle.drawio.svg`'s Rules box) is actually recorded — when an entry spawns a child, both files get an edit: the child's `Parent(s)` line, and (if useful) the parent's `Children` line.

---

## 4. ID assignment and citation format

- **Next ID** = highest existing filename in `docs/adr/` + 1. Since every entry has a file (§3), this is a single `ls docs/adr | sort -n | tail -1` — no need to scan four `ADR_log.md` tables and risk missing one. The schema file (§7.1) is named so it doesn't start with a digit, so `sort -n` sorts it as the lowest value regardless of its exact name — it can never land at the tail end and throw off this command.
- **Citation format:** `ADR-029`, zero-padded to 3 digits. Used in `ADR_log.md`, in `docs/adr/*.md`'s Parent(s)/Children lines, and in any other doc that needs to point at an entry (e.g. a future `Composable-Extensions.md` Discussion item, or `CXTG_QEMU_Action_Plan.md`'s "Deviation — see ADR-NNN" notes per `docs/README_docs.md`).

---

## 5. Migration

Source material, in migration order:

1. **`docs/todo.md`'s items** — all of them, including process/procedural ones (see below), except the two meta items describing this migration itself (§9 covers those, last).
2. **`docs/bugs.md`** — currently empty; migration here is a no-op check, not empty content to port.
3. **`qemu-cxtg/Implementation_Choices.md`** — 4 existing entries (runtime extension flags, CSR addresses, `cxsetsel` opcode, `cxdiscard` opcode) written before this pivot. Port each into an `ADR_log.md`/`docs/adr/NNN.md` pair; the source file is then superseded and can be deleted (nothing else in the project cites it by its `I<N>` numbers yet, so this is a clean removal, not a deprecation chain).

**Classifying an item:**
- A substantive design choice or established fact (Build Flag Architecture, CSR addresses, the `~0` WARL decision, `scxstp.mode=2` resolution) → **Decision**.
- A plain process/procedural item (`sync README.md`, `sync CXTG_QEMU_Action_Plan.md with recent edits`) → **Note**. Its lifecycle is `deferred → note (active) → retired`: it starts `deferred` only if there's still an open question about *how* the work should be done; once the how is fully specified (as most of `todo.md`'s process items already are — they're written as concrete instructions, not open questions), it enters directly at `note active` (statemachine.csv's `NEW → note active` path), stays active until the work described is actually carried out, then transitions to `retired`. This mirrors how the system already treats an implementer hint that's acted on and no longer needed.
- No item is left uncategorized or left behind in a source file because it "isn't ADR-shaped" — every item is one of the two above.

**Procedure — one item at a time, reviewed as it happens** (matches the original todo item's own "go one at a time" instruction):

1. **Watch for overlap.** `todo.md` and `Implementation_Choices.md` already independently document some of the same facts (e.g. both currently state the runtime extension flags and CSR addresses) — migrate to one ADR entry, not two, when this happens.
2. **Write the entry:** assign the next ID (§4), create `docs/adr/NNN.md` with the classified category, Phase(s), and the item's content as the body, add the creation row to its Transitions table (carrying forward any real historical date into that row's Notes, per §3), add the `ADR_log.md` index row.
3. **Remove the source line.** Once an item is fully captured, remove it from `todo.md`/delete `Implementation_Choices.md`'s corresponding line, unless another `todo.md` item references it (per that file's own existing rule) — in which case leave a pointer rather than deleting outright, resolved once the referencing item itself migrates.

---

## 6. `docs/todo.md`'s fate

Once every item is migrated (§5, §9), `docs/todo.md` becomes a one-line stub pointing at `ADR_log.md`, rather than being deleted outright — this keeps old links and habitual lookups (`CLAUDE.md`'s "Starting a New Phase" step 2, `qemu-cxtg/CLAUDE.md`'s "Implementation status" pointer, anyone's muscle memory) from silently 404ing. Both `CLAUDE.md` files' references to `docs/todo.md` get updated to point at `ADR_log.md` instead once the stub is in place.

---

## 7. Wiki updates

`docs/ADR_log.md` gets a single Format D wiki page, added to `wiki/index.md`, same as any other doc file.

### 7.1 `docs/adr/_schema.md`

A schema/reference file living inside `docs/adr/` itself (not a numbered entry) — the entry template from §3, the category/status tables from §1, the ID-assignment and citation rules from §4, in one place next to the entries it governs. Named with a non-numeric leading character specifically so `ls docs/adr | sort -n | tail -1` (§4) never picks it up as the "highest" entry.

### 7.2 Wiki exception for `docs/adr/**`

`docs/adr/**` (including `_schema.md`) is exempt from wiki pages: the log+detail-page pair already is an index/sub-page structure in the shape of `wiki/schema.md`'s Format B, and an ADR page's Transitions table is exactly the dated-history content the wiki's "current state only" convention excludes. This needs one added sentence in `wiki/schema.md`'s content-rules section recording the exception (e.g. "`docs/adr/**` is exempt from wiki pages as per `docs/superpowers/specs/2026-09-10-adr-system-design.md` §7.2") — kept to that one sentence, since `wiki/schema.md` is read on every wiki update and the fuller rationale belongs here, not there. Both `CLAUDE.md` files' wiki-trigger paragraphs get the same one-line carve-out.

---

## 8. Explicit scope boundaries

- **`qemu-cxtg/Composable-Extensions.md`'s Discussion-section rework** (conciseness pass, new References section, `I<N>`-style citations) is **not** part of this design or its implementation plan — it will be carried out later by a separate agent, working directly from the Note ADR §9 spawns for it.
- **No scan of the QEMU source tree, `spec.md`, or the Action Plan's own hardcoded values** for additional entry candidates beyond §5's three sources — same boundary the original design drew, still holds. `ADR_log.md` is expected to keep growing from other sources over time; that's ordinary future use of the system, not part of standing it up.
- **No changes to `docs/CXTG_QEMU_Action_Plan.md` itself.** The migrated "sync CXTG_QEMU_Action_Plan.md" Note (§5) documents that the sync is still owed; actually executing it is out of scope here.
- **No phase/block-level gate-check flowchart.** `flowchart_adr_life_cycle.drawio.svg` diagrams entry *statuses and transitions* only; how a phase start actually checks the log for open `deferred`/`pending`/`?`-phase(s) entries affecting it is currently one sentence in `CLAUDE.md` and nothing in `qemu-cxtg/CLAUDE.md`. §9 creates a Note ADR to track this gap rather than leaving it as prose here.

---

## 9. Bootstrapping items and new Notes created during setup

Three Notes get created that aren't a plain 1:1 port of existing content:

- **"documentation prep for Action Plan update"** — most of this item's scope is complete once this plan lands. Its one sub-bullet that's still genuinely outstanding — the `Composable-Extensions.md` Discussion rework (§8) — spawns off first as its own **child Note**, `active`, `phase(s) = all`, carrying that remaining scope forward rephrased for the new system (`ADR-NNN` instead of `I<N>`, `ADR_log.md` instead of "Choices"). Only then does the parent item itself migrate, `retired`. Because it now has an active child, the archival sweep (§2.1) keeps the parent in the live Notes table regardless of its own `retired` status — it doesn't archive until that child does.
- **"wiki lint"** — migrates as its own Note, `active`, `phase(s) = all`. Its body should carry an open question: should this future lint procedure also validate `ADR_log.md`/`docs/adr/`'s own internal consistency (ID sequence, every log row has a matching file, etc.), not just the rest of the wiki?
- **Phase/block-level gate-check flowchart** (§8) — a new Note, `active`, `phase(s) = all`, documenting that `CLAUDE.md`'s "Starting a New Phase" step 2 and `qemu-cxtg/CLAUDE.md`'s per-block workflow need an actual gate-check procedure (ideally its own flowchart) that reads `ADR_log.md`'s Phase(s) column, rather than the current single prose sentence. It is possible that the ideal solution keeps both: the flowchart for humans, and an efficient prose directive for Claude.

---

## 10. Execution plan (phases, each ending in a review checkpoint)

1. **Phase 1** — Create `docs/ADR_log.md` skeleton (four empty tables + intro), the `docs/adr/` directory, and `docs/adr/_schema.md` (§7.1). Write the `wiki/schema.md` one-sentence addendum and update both `CLAUDE.md` files' wiki-trigger wording (§7.2) before any numbered `adr/NNN.md` file is written, so the exception is on record first.
2. **Phase 2** — Migrate `qemu-cxtg/Implementation_Choices.md`'s 4 entries (smallest, cleanest source — good first pass to validate the entry format). Delete the source file once ported.
3. **Phase 3** — Migrate `docs/todo.md`'s remaining items (Decisions and Notes both, per §5's classification), one at a time, watching for overlap with Phase 2's entries. Confirm `docs/bugs.md` is genuinely empty and needs no migration.
4. **Phase 4** — Create the three new Notes (§9), migrate the two bootstrapping items, then reduce `docs/todo.md` to its stub (§6); update both `CLAUDE.md` files' `docs/todo.md` references to point at `ADR_log.md`.

Per `CLAUDE.md`, staging/committing waits for an explicit "ready to commit?" regardless of phase boundaries. Continue on the existing `todo/doc-rework` branch (cut in both `runtime-cxtg` and `qemu-cxtg`, since Phase 2 touches `qemu-cxtg/Implementation_Choices.md`) — no new branch needed, this is a continuation of the same doc-rework effort already in progress there.
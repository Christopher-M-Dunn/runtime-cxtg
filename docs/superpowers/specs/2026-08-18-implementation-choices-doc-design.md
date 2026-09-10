# Implementation Choices Doc & Documentation Prep — Design

**Source:** `docs/todo.md`, "documentation prep for Action Plan update" (originally added at commit `9250385738fae0e6934f75c65ed9215a865be062`).

**Classification:** Architectural — introduces a new persistent document (`Implementation_Choices.md`) with its own numbering/deprecation convention and a cross-reference format (`I<N>`, `#R<N>`/`#M<N>`/`#B<N>`/`#D<N>`) that other docs depend on.

---

## 1. Goal

Separate three things that are currently tangled together across `docs/todo.md` and `qemu-cxtg/Composable-Extensions.md`:

1. **What the spec requires** — already lives in `Composable-Extensions.md`'s requirement sections (out of scope here, untouched).
2. **What the TG wants (intent, not yet settled spec)** — already lives in `docs/Requirements.md` (out of scope here, untouched).
3. **What this project decided, beyond (1) and (2), in order to have something buildable and testable today** — currently scattered across `todo.md` prose and `Composable-Extensions.md`'s Discussion section, mixed in with rationale, open questions, and meeting notes. This is the gap `Implementation_Choices.md` fills.

**Test for "is this a Choices entry":** starting from the spec (as distilled into `Composable-Extensions.md`'s requirement sections) and `Requirements.md`, what additional information would someone need to replicate this project's behavior exactly? That information — and only that — belongs in Choices.

---

## 2. `qemu-cxtg/Implementation_Choices.md`

New file, wiki Format D (self-explanatory doc).

**Structure:**

```
# Implementation Choices

<intro note explaining the convention: entries are numbered and append-only;
when a choice changes meaningfully, the old entry becomes
"**Deprecated. See #X.** ~~<original text>~~" and a new entry is appended
at the bottom starting with a reference back to what it supersedes>

---

<numbered list, entries appended here as they're identified>

1. ...
2. ...
```

Nothing currently lives above the divider — this task only populates the archive; moving entries "above the divider" (marking them incorporated into the Action Plan) is future work, not part of this task.

**Entry style — concrete, not prose:**

- State the decision only. No rationale, no "why," no source citation.
- Entries are plain lines, not markdown numbered-list syntax: `<N> - <text>` for a plain entry, `<N> * <text>` when the entry has a corresponding Discussion item in `Composable-Extensions.md` — the `*` is the only cross-reference Choices carries in that direction. (The relationship is asymmetric: `Composable-Extensions.md` cites Choices by number — `Implementation: I29` — but Choices entries don't cite back.)
- Scope includes anything needed to replicate current project behavior: CSR/encoding placeholder values (including the ones in `todo.md`'s permanent "CSR Addresses, instruction encodings, and extension names" section — the tracking item itself stays in `todo.md` since final values are still pending RISC-V org assignment, but today's placeholder values are Choices entries), WARL clamp behaviors, mode-legalization rules, state-machine mappings, etc.

Example entries:

```
12 - CSR addresses: `CSR_CXSEL = 0xCA0`, `CSR_CXSIDX = 0x018`, `CSR_CXSDATA = 0x019`
13 - cxsetsel opcode = 0x00004073 (SYSTEM, funct3=4, funct7=0)
29 * `cxsidx` WARL behaviour: `cxsidx & ((512 * KiB / sizeof(target_ulong)) - 1)`
30 * `scxstp` WARL behaviour: writing `scxstp.mode=2` (Indirect) when `ext_zcxmulti` is disabled clamps to mode=1 (Direct), all other bits zeroed; writes with reserved modes 3-15 are ignored; writes with .mode = (1 or 0) zeroes all other bits
```

---

## 3. `docs/todo.md` sweep

Two passes, in order:

**Pass A — staleness check:** read every item not already marked done/resolved; if it turns out to already be resolved but unflagged, mark it done in place (no extraction yet — this pass only fixes stale flags).

**Pass B — extraction, case by case, item by item (including sub-bullets of compound items):**

- **Pure settled implementation detail** (address, encoding, WARL clamp, mode mapping, state-machine adoption, etc.) → new Choices entry (concrete form only, per §2), then move the todo.md item verbatim to `docs/todo_done_did.md` with a trailing `→ Choices #N` pointer. Remove it from `todo.md`.
- **Bundles a decided part and a still-open action/question** → split: decided part becomes a Choices entry (and the decided fragment moves to `todo_done_did.md` with its pointer); the open part stays in `todo.md`, reworded to cover only what remains.
- **Pure open question / awaiting TG input** → left untouched in `todo.md`.
- **Referenced by another todo.md item** → not removed even if otherwise eligible, per the todo item's own instruction.

Compound items (`~0 in this implementation`, `incorporate Requirements.md into Discussion` — already marked done, so this one moves to `todo_done_did.md` in full once its content is confirmed non-actionable, `sync CXTG_QEMU_Action_Plan.md`) are walked sub-bullet by sub-bullet under these same rules rather than as a single unit.

The permanent "CSR Addresses, instruction encodings, and extension names" section stays in `todo.md` in full (it's explicitly flagged not-to-be-removed until end of project) — its current values additionally get Choices entries per §2's scope note, but the tracking item itself is untouched.

---

## 4. `docs/todo_done_did.md`

New file, wiki Format D. Append-only archive: cleared `todo.md` items (or split-off decided fragments) moved here verbatim, each with a `→ Choices #N` pointer where applicable. No summarization — this is a record of what the item said when it was cleared, not a rewritten changelog entry.

---

## 5. `qemu-cxtg/Composable-Extensions.md` — Discussion section trim

Read the whole Discussion section first (already done during brainstorming — 35 items), then edit item-by-item with targeted edits (not a whole-file rewrite), so the result is reviewable via `git diff` hunk-by-hunk.

**Per-item target shape:** gap the spec doesn't cover → resolution status → implementation pointer (if any). Specifically:

- A concrete decision embedded in an item's prose → extract to a new Choices entry (marking that Choices entry with `*`), replace the prose in-place with `Implementation: I<N>`.
- A citation to `Requirements.md`, a dated TG meeting, the basis spec / `basis_spec_comparison.md`, or a design deck → replace the inline citation with the matching anchor (`#R<N>`, `#M<N>`, `#B<N>`, `#D<N>`); move the full citation detail to the new References section (§6).
- Genuinely open questions, TG-awaiting-clarification notes, and editorial-artifact flags (duplicated spec text, etc.) stay as concise prose — nothing to extract.
- Duplicate or superfluous explanatory text gets cut outright (not moved anywhere) if it doesn't fit any of the above categories.

**Known correction to make here — `cxsidx` WARL bound reverts to the flat clamp:** the current "State context size bound (512 KiB)" Discussion item records the interim "Option 1, per-CX bound, behind a flag" assumption as live. That assumption is superseded as of this task: the actual project decision is the flat clamp, `cxsidx` values above 131071 (RV32) / 65535 (RV64) clamp to that value, same bound for every CX (Choices entry per §2's worked example). When this Discussion item is trimmed, its text must reflect the flat clamp, not Option 1, with an `Implementation: I<N>` pointer to the flat-clamp Choices entry. The older per-CX assumption isn't corrected anywhere retroactively — the "incorporate Requirements.md" todo.md item that recorded that pivot moves to `todo_done_did.md` verbatim as history (§3-4); Choices is what's authoritative for current behavior, not the archive.

---

## 6. `## References` section (new, end of `Composable-Extensions.md`)

Four groups, confirmed taxonomy:

```
## References

**Requirements.md**
- #R1 — ...
- #R2 — ...

**Meetings**
- #M1 — 2026-07-31 TG meeting
- #M2 — 2026-08-14 TG meeting

**Basis spec**
- #B1 — grayresearch/CX §2.2.1, cxe bit
- ...

**Design decks**
- #D1 — Darius & Jan, 2025-06-13, Context Management pre-draft
```

Anchor links use `#R2`-style fragment IDs; the letter identifies the group, the number is that group's entry index (1-based, in the order encountered while trimming Discussion).

---

## 7. Wiki updates

`Implementation_Choices.md` and `todo_done_did.md` each get a Format D page (per `wiki/schema.md`) under `wiki/runtime-cxtg/qemu-cxtg/` and `wiki/runtime-cxtg/docs/` respectively, added to `wiki/index.md`. Created immediately after each file is written, per the "wiki update is part of the write action" rule — not batched to the end.

`Composable-Extensions.md`'s existing wiki page gets updated to reflect the trimmed Discussion section and new References section.

---

## 8. Reassessing `todo.md`'s "sync CXTG_QEMU_Action_Plan.md" item

After §§2-7 are done, re-read that item's text against the now-current state of `todo.md` and `Composable-Extensions.md` (both changed substantially by this task) and rewrite as necessary, maintaining original intent. Pay attention to what the item originally asked for that Choices now resolves or makes moot, what's still accurate, and what if anything is newly true because Choices exists (e.g., the item's own note that "the Action Plan can cite Implementation Choices for X" once such a doc exists). If the rewrite surfaces additional project decisions not already captured, add those as new Choices entries too — this step isn't purely read-only. Actually executing that item's Action-Plan-sync work is explicitly out of scope.

---

## 9. Explicit scope boundaries

- No changes to `docs/CXTG_QEMU_Action_Plan.md` itself (todo.md item 130 gets revised, not executed).
- No scan of the QEMU source tree, `spec.md`, or the Action Plan's own hardcoded-address comments for additional Choices candidates — this pass draws only from `todo.md` and `Composable-Extensions.md`'s Discussion section, per the originating todo item's scope. (Choices is expected to keep growing from other sources later; that's future work.)
- `CHARTER.md` is not currently cited anywhere in Discussion, so no `#C` reference group is created this pass — if a future edit needs one, add it then.

---

## 10. Execution plan (phases, each ending in a review checkpoint)

1. **Phase 1** — Create `Implementation_Choices.md` skeleton (intro + divider) and populate it with the permanent CSR/encoding placeholder values from `todo.md`'s permanent section (§2/§3 scope note). No todo.md edits yet.
2. **Phase 2** — `todo.md` sweep: Pass A (staleness), then Pass B (extraction/split/archive), populating more Choices entries and creating `docs/todo_done_did.md`.
3. **Phase 3** — `Composable-Extensions.md` Discussion trim + new References section, cross-referencing Choices entries created in Phases 1-2 and adding any new ones the trim itself surfaces.
4. **Phase 4** — Wiki pages for `Implementation_Choices.md` and `todo_done_did.md`; update `Composable-Extensions.md`'s wiki page.
5. **Phase 5** — revise `todo.md`'s "sync CXTG_QEMU_Action_Plan.md" item and check to make sure it makes sense.

Each phase is its own reviewable diff; per CLAUDE.md, staging/committing waits for an explicit "ready to commit?" confirmation regardless of phase boundaries — these are documentation-only changes, so per the branch workflow they would normally go directly to `cxtg-dev`, but this todo is an explicit exception to that. Cut branch `todo/doc-rework` from `cxtg-dev` in **both** `runtime-cxtg` and `qemu-cxtg`, mirroring the standard cross-repo `feat/<block>` pattern — needed since `Composable-Extensions.md`/`Implementation_Choices.md` live in `qemu-cxtg` while `todo.md`/`todo_done_did.md`/wiki/this spec live in `runtime-cxtg`.

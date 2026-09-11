# CXTG TODO — Deferred Implementation Decisions

**Note:** This document is for QEMU and runtime implementation TODOs regarding *how* we are implementing the requirements. If the question is *what* the spec requires — including non-normative sections, which this project treats as requirements — check `Composable-Extensions.md`. If still unclear there, add an item to `Composable-Extensions.md`'s Discussion section, listing any decisions as project assumptions.

---

## documentation prep for Action Plan update

- read all this todo, then use superpowers brainstorming before starting
- do this work on `todo/doc-rework` cut from cxtg-dev

**NOTE (2026-09-09)** — superseded the `Implementation_Choices.md` design below after brainstorming. Settled instead on an ADR (Architecture Decision Record) system: a single index `docs/ADR_log.md` (not yet created), one global sequential number across three categories — Decisions (`deferred → pending → in progress → incorporated`, or `superseded` at any point), Bugs (`open`/`closed`), Notes (`active`/`retired`). This replaces `qemu-cxtg/Implementation_Choices.md`, this file's own deferred-decision items, `bugs.md`, and `todo_archive.md` outright — see `docs/README_docs.md` for the full file-layout writeup. Done so far: the state machine is finalized (`/media/sf_CX/statemachine.csv`) and diagrammed (`docs/flowchart_adr_life_cycle.drawio.svg`, editable in draw.io); `docs/README_docs.md` rewritten to describe the new layout. Still to do:
  - create `docs/ADR_log.md` and `docs/adr/NNN.md` per that design
  - migrate this file's other still-open items, `docs/bugs.md`, and `qemu-cxtg/Implementation_Choices.md` (already created at qemu-cxtg commit b181e10a0a with CSR/encoding placeholders — superseded by this pivot, port its content into ADR entries or discard it) into `ADR_log.md` entries
  - decide the fate of this file once migrated — delete outright, or leave a stub pointing to `ADR_log.md`
  - create the wiki page(s) for `ADR_log.md` and its per-entry pages
  - a separate flowchart for the phase/block-level gate-check flow is still needed — currently it's one prose sentence in the root `CLAUDE.md`'s "Starting a New Phase" step 2, and `qemu-cxtg/CLAUDE.md`'s per-block workflow has no gate-check step at all

Original plan (superseded, kept for context — do not action):
- ~~create `qemu-cxtg/Implementation_Choices.md` (aka "Choices"). All non-normative implementation choices will be documented here in a numbered list, as well as any change that requires an update to the Action Plan. Make a note at the top that if a choice changes meaningfully, it should be moved to a new number at the bottom and the entry at the old number should change to "Deprecated. See #X. [strike-through of old text]", where X is the new item number. The new entry should start with a reference to the entry it supersedes. Explain about how the document is divided by a divider where everything below the divider is not yet incorporated into `docs/CXTG_QEMU_Action_Plan.md`. Put a divider just below the introductory notes.~~
- `docs/todo.md`
  - Go through each item that is not marked as done and determine if it actually is already done (if so, mark it as done).
  - Go one at a time extracting any project assumptions or decisions (anything that could affect the Action Plan), and create new entries in `ADR_log.md`. determine if the item can be removed from `todo.md`. Unless another todo item is referencing it, it's not clear what other circumstance would prevent it from being removed.
- `qemu-cxtg/Composable-Extensions.md` Go through the Discussion section to improve conciseness: identify superfluous or duplicate info, and move references out to footnotes. Everything remaining should be concisely identifying gaps the spec doesn't cover, resolution, implementation (e.g. Implementation: ADR-029) Move all implementation decisions to `ADR_log.md` and replace the text with a reference. read the whole document first, then apply small edits one at a time so they can be reviewed. Add a References section. For references format, use anchor links to LetterNumber e.g. #R2, where the letter represents a particular source type or document, and the number is the reference index. so if the reference is `#R2`, the reference is to the 2nd item under `Requirements.md` group in the References section; `#M1` would be the first item in the `Meetings` group (include the meeting date in the item); `ADR-029` would be entry #29 in `ADR_log.md`
- reassess item `todo.md:130` for stale info based on the changes made since it was created (9250385738fae0e6934f75c65ed9215a865be062). Rewrite it as necessary. it will need to know about `ADR_log.md` to know what to change, but should also add entries as additional decisions are gleaned during the rewrite.

---

## wiki lint

- make a procedure to check wiki for consistency
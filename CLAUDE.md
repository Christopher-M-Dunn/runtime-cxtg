# CLAUDE.md

This file provides guidance to Claude Code when working in this repository.

## Methodology

Every word of code and every programatic structure must be 100% understood by the user before implementing. This includes not only how something is being done, but why it is being done that way. Always explore multiple ways of doing things. Draw inspiration from and comparisons to existing outside systems and known best practices for similar work.

## Repository Structure

`runtime-cxtg` is the umbrella project for the RISC-V Composable Extensions (Zcx/ZcxMulti) implementation.

| Path | Purpose |
| --- | --- |
| `qemu-cxtg/` | QEMU fork implementing the Zcx/ZcxMulti hardware model (git submodule) |
| `linux-cxtg/` | Kernel for S-mode testing — future work (git submodule) |
| `docs/` | Project documentation, action plan, progress tracking, and audit notes |
| `LOCALFILES/` | Local reference files; not committed; don't look here unless directed to. |

For QEMU-specific guidance (build system, code style, CSR patterns, extension registration), see `qemu-cxtg/CLAUDE.md`.

## Current Work

This project follows a phased implementation plan defined in `docs/CXTG_QEMU_Action_Plan.md`.

**At the start of any session:** read `docs/progress.md` to see which phase is active, which blocks are complete, history of every major acttion committed, and what is deferred in `docs/todo.md`.

## Branch Workflow

Both repos follow the same pattern. Feature branches are cut in both repos together at the start of each block and merged together when the block is done.

```
cxtg          ← stable; updated only at phase milestones, always tagged
cxtg-dev      ← integration branch; feat/* merges here
feat/<block>  ← all work for a block; cut from cxtg-dev in both repos simultaneously
```

For the per-block workflow inside `qemu-cxtg`, see `qemu-cxtg/CLAUDE.md`.

### Phase milestone (both repos together)

When a phase is complete:
1. `qemu-cxtg`: merge `cxtg-dev` → `cxtg`; tag (e.g. `cxtg-v0.phase1`).
2. In `runtime-cxtg/cxtg-dev`: commit the updated `qemu-cxtg` submodule pointer and any documentation updates.
3. `runtime-cxtg`: merge `cxtg-dev` → `cxtg`; apply the same tag.

## Starting a New Phase

1. Read `docs/CXTG_QEMU_Action_Plan.md` — the relevant phase section for block objectives, files, and definitions of done.
2. Read `docs/todo.md` — resolve or confirm any open items not explicitly labelled for a later phase.
3. Use `superpowers:brainstorming` to work through each block, nail down all specifics, and revisit open todos.
4. Use `superpowers:writing-plans` to produce the phase implementation plan before touching any code.
5. Mark `implementation plan written` in `docs/progress.md` for the phase.

## Documentation Files

### `docs/CXTG_QEMU_Action_Plan.md`

The master implementation plan. Every block contains: objective, prerequisites, repos/files, implementation notes, tests, definition of done, and merge action.

- **Read:** at the start of every phase and every block
- **Update:** when a block scope changes due to audit findings; when a todo item resolves a planning decision that affects block notes
- **Navigate:** each phase and block is a named heading — jump directly to the relevant section

---

### `docs/progress.md`

Live block-by-block checklist and release notes. The primary status document.

- **Read:** at the start of any session to orient yourself
- **Sections:** Phase checklists (top) one per phase; `## Release Notes` (bottom) filled backwards as milestones land

**Checklist updates:**
- Check off block items as they complete
- Mark `implementation plan written` at the top of a phase when the plan is done

**Release notes — when to write:** incrementally, as each block merges to `cxtg-dev`. Write the entries for a block immediately after its merge, while the diff is fresh. At phase milestone time, run `git diff --stat <prev-tag>..<new-tag>` in each repo and compare the file list against the notes and wiki entries to catch anything missed — do a full diff only for files that appear in the stat but not in the notes.

**Release notes — what to include, and how:**

Each `### v0.phaseN` entry must contain enough information to answer "what exactly changed and where?" via a text search alone. The format is:

1. **`qemu-cxtg` changes** — for each file touched by the block, write one bullet per discrete action in past tense. Each bullet must name the function, symbol, struct field, or constant that was touched — not just a description of intent. Examples of good vs. bad:
   - Good: `modified: target/riscv/cpu.c — registered zcx as ISA extension (ISA_EXT_DATA_ENTRY, MULTI_EXT_CFG_BOOL, default off)`
   - Bad: `modified: target/riscv/cpu.c — added extension support`
   New files use `created:`; modified files use `modified:`. Combine bullets that touch the same function for the same purpose.

2. **`runtime-cxtg` changes** — same format, for any docs or source files changed in this repo. Omit if nothing changed beyond the submodule pointer.

3. **New deferred items** — any new entries added to `docs/todo.md` during this phase, nested under `- created/modified: docs/todo.md`, one line each in the same format as the existing deferred items in `### v0.phase0`.

4. **Bugs** — any bugs opened or closed during the phase, one line each referencing the entry in `docs/bugs.md`.

---

### `docs/bugs.md`

Known bugs and their status.

- **Read:** before starting a block that touches affected code
- **Update:** add a bug when found; mark resolved when fixed

---

### `docs/spec.md` *(symlink → `qemu-cxtg/spec.md`)*

Artur's original Zicx implementation specification. CSR layout, behavior semantics, and QEMU integration patterns.

- **Read:** when implementing or reviewing CSR behavior for cxsel, cxsidx, cxsdata; when verifying privilege and reset semantics
- **Update:** `## Architecture` section only — when CSR addresses or semantics are confirmed by spec owners; all other sections are stable reference
- **Note:** extension name is `zcx` (not `zicx` as written); CSR addresses are stubs pending spec finalisation — see `docs/todo.md`

---

### `wiki/`

A navigable reference wiki for every file touched by the project. All pages live under `wiki/runtime-cxtg/`, mirroring the source tree. `wiki/index.md` is the master file list.

**Before writing any wiki page:** read the four format examples at the top of `wiki/index.md`. Each example links to a live page — open it and read it before writing anything. The format chosen depends on the nature of the file (small de novo, pre-existing with discrete changes, large de novo, or self-explanatory).

**Before committing any change to a tracked file:** update the corresponding wiki page(s). If the file is not yet in the wiki, add it to `wiki/index.md` and create its page at `wiki/runtime-cxtg/<path-to-file>.md`.

---

### `docs/Composable-Extensions.md` *(symlink → `qemu-cxtg/Composable-Extensions.md`)*

CX extension requirements: per-CSR behavior rules, cxsetsel semantics, open questions, and test cases.

- **Read:** when verifying an implementation matches spec requirements; when reviewing what open questions remain
- **Update:**
  - `## Open Questions` — remove items as resolved, add new ones as they surface
  - `## Tests` — add test cases as blocks are implemented
  - CSR requirement sections — update only if the spec changes a behavior rule

### `../composable-custom-extensions/src`

Task group's full spec documentation deliverable, early draft
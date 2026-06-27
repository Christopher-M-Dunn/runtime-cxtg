# Wiki Fix Design

**Branch:** `todo/fix_wiki`
**Date:** 2026-06-25

---

## Problem

The wiki has accumulated inconsistencies:
- No front matter indicating format type
- Titles are bare headings with no link to the source file
- No in-page navigation (no "Main Index" link, no back-to-top for Format C, no "Parent: x" link for subpages)
- Format A/C split criterion ("fits in one screen") is too vague
- Format D criterion ("self-evident") is ambiguous

---

## Schema Changes

### Format selection criteria

| File type | Format |
|---|---|
| All non-code files (docs, config, SVGs, symlinks, markdown) | **D** |
| Pre-existing upstream file modified by this project | **B** |
| New file written by this project | **A** to start — convert to **C** if wiki output > 125 raw lines |

**Checking Format B eligibility (qemu-cxtg submodule only):**
```bash
git -C qemu-cxtg show v10.2.90:<path-relative-to-qemu-cxtg-root> 2>/dev/null \
  && echo "upstream" || echo "new"
```
Tag `v10.2.90` is the QEMU base from the phase 0 audit. Verify with `git -C qemu-cxtg tag` if uncertain. Do NOT check against the project's own `cxtg` branch — that branch will accumulate project files over time and the check will break.

All files in `runtime-cxtg/` are de-novo; none are Format B.

**Format A/C reassessment rule:** When updating a Format A page, count the raw lines of the wiki file after edits (`wc -l <file>`). If > 125, convert to Format C by adding the `## Index` section with URL-encoded heading links, and back-to-top links at the end of each section. Format C pages never convert back to A.

---

### Front matter

Every **main page** (not sub-pages) starts with:
```yaml
---
format: A
---
```
Replace `A` with the correct format letter. Front matter is parsed silently by Obsidian (visible in the Properties panel, not in the rendered body).

Sub-pages (Format B only) get no front matter.

---

### Navigation links

Every **main page** gets this as the first visible element, right after any front matter and before the H1 title:
```html
<p align="right">[Main Index](relative/path/to/index.md)</p>
```

Every **sub-page** (Format B only) gets:
```html
<p align="right">[Main Index](relative/path/to/index.md) | Parent: [parent-basename](../parent-file.md)</p>
```
`parent-file.md` is the main page filename (e.g., `cpu.c.md`); `parent-basename` is the display label (e.g., `cpu.c`).

`wiki/index.md` is the only file that does not get a navigation link.

The relative path to `wiki/index.md` depends on directory depth from `wiki/`:

| Depth from `wiki/` | Example | Path |
|---|---|---|
| 1 | `wiki/runtime-cxtg/Makefile.md` | `../index.md` |
| 2 | `wiki/runtime-cxtg/docs/bugs.md.md` | `../../index.md` |
| 3 | `wiki/runtime-cxtg/qemu-cxtg/disas/riscv.c.md` | `../../../index.md` |
| 4 | `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu.c.md` | `../../../../index.md` |
| 5 | `wiki/runtime-cxtg/qemu-cxtg/target/riscv/insn_trans/…` | `../../../../../index.md` |

Sub-pages are one level deeper than their parent main page.

---

### Title format (all main pages)

Plain heading followed by a "source" link on the next line:
```markdown
# path/to/file

[source](relative/path/to/source/file)
```
The `relative/path` is computed from the wiki page's own location to the source file in the repo. Examples:
- `wiki/runtime-cxtg/tests/common/crt0.S.md` → `[source](../../../../tests/common/crt0.S)`
- `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu.c.md` → `[source](../../../../../../qemu-cxtg/target/riscv/cpu.c)`
- `wiki/runtime-cxtg/CLAUDE.md.md` → `[source](../../CLAUDE.md)`

Sub-pages get no source link.

---

### Format C — back to top

Each `###` section in the full text walkthrough ends with a blank line then a right-aligned back-to-top link, placed just above the `---` divider that opens the next section (or at end of file if last section):

```markdown
<p align="right">[↑ back to top](#Index)</p>
```

Obsidian resolves heading anchors by URL-encoded heading text, not slugs. `#Index` works as-is (single word). For headings with spaces use `%20`: e.g. `#Full%20text%20walkthrough`. Do not use `{#anchor}` tags — Obsidian does not support them and they corrupt the heading text.

---

## Implementation Passes

Execute in this order. Each pass is independent — complete one before starting the next.

### Pass 1 — Schema

Update `wiki/schema.md` with all changes described above.

### Pass 2 — Navigation links (main pages, bulk)

Prepend the `Main Index` navigation link to every main-page `.md` file under `wiki/runtime-cxtg/`. Sub-pages are handled in Pass 5 (they need a parent-specific link). Group by depth — see the depth table above for the correct relative path to `wiki/index.md` per depth level.

Verify a sample file looks correct before moving on.

### Pass 3 — Front matter

For each main page (not sub-pages):
1. Identify the source file
2. Determine format: D for non-code; B via `git show v10.2.90`; otherwise A (de-novo code files start as A — conversion to C happens after writing, based on output line count)
3. Prepend `---\nformat: X\n---\n\n` before the navigation link. If the format is **not B**, check for a same-named subfolder (e.g., `cpu.c/` next to `cpu.c.md`). If one exists, log it: `[wiki/path/file.md] — non-B page has stale sub-page folder; review sub-page content before deleting`. The subfolder is handled during the format-specific pass, after its content has been reviewed for anything worth incorporating into the main page.

Do not read the wiki page body — only prepend. Sub-pages are skipped entirely.

### Pass 4 — Format A pages

For each Format A page:
- Add the `[source](...)` link below the title
- Fix any non-conformances found
- Log each issue to `wiki/wiki_fix_log.md` as: `[wiki/path/to/file.md] — <issue>`

### Pass 5 — Format B pages

For each Format B main page:
- Add the `[source](...)` link below the title
- Fix any non-conformances on the main page

For each sub-page: prepend the `Main Index | Parent: x` navigation link (depth = parent depth + 1; parent link = `../parent-file.md`); fix any non-conformances; log issues.

### Pass 6 — Format C pages

For each Format C page:
- Add the `[source](...)` link below the title
- Add `<p align="right"><a href="#index">↑ back to top</a></p>` at the end of each `###` section
- Fix any non-conformances
- Log issues

### Pass 7 — Format D pages

For each Format D page:
- Add the `[source](...)` link below the title
- Fix any non-conformances
- Log issues

### Pass 8 — Log review and cleanup

1. Read `wiki/wiki_fix_log.md`
2. Identify recurring patterns (e.g., "missing Works with block", "history prose in descriptions")
3. Update `wiki/schema.md` to add rules that prevent each pattern
4. Update `qemu-cxtg/CLAUDE.md` and `CLAUDE.md` (root) wiki instruction sections if any changes make more sense as workflow reminders than schema rules
5. Delete `wiki/wiki_fix_log.md`

---

## Non-conformance Log Format

File: `wiki/wiki_fix_log.md` (temporary — deleted after Pass 8)

One entry per issue:
```
[wiki/runtime-cxtg/tests/common/crt0.S.md] — history prose in description ("was changed from X to Y")
[wiki/runtime-cxtg/qemu-cxtg/target/riscv/csr.c/cxsel_pred.md] — missing enclosing function signature in code snippet
```

---

## Files Touched

- `wiki/schema.md` — schema updates
- All `wiki/runtime-cxtg/**/*.md` files — up links, front matter, source links, back-to-top, conformance fixes
- `wiki/wiki_fix_log.md` — created during passes 4–7, deleted in pass 8
- `CLAUDE.md` (root) — possible wiki instruction tidying in pass 8
- `qemu-cxtg/CLAUDE.md` — possible wiki instruction tidying in pass 8

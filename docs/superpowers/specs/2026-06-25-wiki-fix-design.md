# Wiki Fix Design

**Branch:** `todo/fix_wiki`
**Date:** 2026-06-25

---

## Problem

The wiki has accumulated inconsistencies:
- No front matter indicating format type
- Titles are bare headings with no link to the source file
- No in-page navigation (no "up" link, no back-to-top for Format C)
- Format A/C split criterion ("fits in one screen") is too vague
- Format D criterion ("self-evident") is ambiguous

---

## Schema Changes

### Format selection criteria

| File type | Format |
|---|---|
| All non-code files (docs, config, SVGs, symlinks, markdown) | **D** |
| Pre-existing upstream file modified by this project | **B** |
| New file written by this project, source < 100 cloc lines | **A** |
| New file written by this project, source ≥ 100 cloc lines | **C** |

**Checking cloc:** `cloc <source-file>` — use the "code" column only (excluding comments and blank lines).

**Checking Format B eligibility (qemu-cxtg submodule only):**
```bash
git -C qemu-cxtg show v10.2.90:<path-relative-to-qemu-cxtg-root> 2>/dev/null \
  && echo "upstream" || echo "new"
```
Tag `v10.2.90` is the QEMU base from the phase 0 audit. Verify with `git -C qemu-cxtg tag` if uncertain. Do NOT check against the project's own `cxtg` branch — that branch will accumulate project files over time and the check will break.

All files in `runtime-cxtg/` are de-novo; none are Format B.

**Format A/C reassessment rule:** When updating a Format A page, re-run `cloc` on the source. If it has grown to ≥ 100 lines, convert the page to Format C. Conversion in the other direction (C → A) is unlikely but follows the same check.

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

### "up" navigation link

Every page — including sub-pages — gets this as the first visible element, right after any front matter and before the H1 title:
```html
<p align="right"><a href="../">up</a></p>
```
`wiki/index.md` is the only file that does not get an "up" link.

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

```html
<p align="right"><a href="#index">↑ back to top</a></p>
```

---

## Implementation Passes

Execute in this order. Each pass is independent — complete one before starting the next.

### Pass 1 — Schema

Update `wiki/schema.md` with all changes described above.

### Pass 2 — "up" link (bulk)

Prepend the "up" link to every `.md` file under `wiki/runtime-cxtg/`, excluding `wiki/index.md`. This can be done in one command:
```bash
find wiki/runtime-cxtg -name "*.md" | xargs sed -i '1s/^/<p align="right"><a href="..\/">up<\/a><\/p>\n\n/'
```
Verify a sample file looks correct before moving on.

### Pass 3 — Front matter

For each main page (not sub-pages):
1. Identify the source file
2. Determine format (D for non-code; B via `git show v10.2.90`; A/C via `cloc`)
3. Prepend `---\nformat: X\n---\n\n` before the "up" link. This is the format the page will be, but may not reflect the format it currently is, until after the format passes. If the format is **not B**, check for a same-named subfolder (e.g., `cpu.c/` next to `cpu.c.md`). If one exists, log it: `[wiki/path/file.md] — non-B page has stale sub-page folder (likely currently is a B page and needs to be converted); review sub-page content before deleting`. The subfolder is handled during the format-specific pass for that page, after its content has been reviewed for anything worth incorporating into the main page.

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
- For each sub-page: verify "up" link is present (added in pass 2); fix any non-conformances
- Log issues

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

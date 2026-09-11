*← [Main Index](index.md)*

# Wiki Format Schema

Authoritative format rules for all pages under `wiki/runtime-cxtg/`. Read this before writing or editing any wiki page.

---

## When to update the wiki

**After every file write** — update the corresponding wiki page before writing the next file. The wiki update is part of the write action, not a post-step. Do not batch wiki updates to commit time.

If the file has no wiki page yet: add it to `wiki/index.md` and create its page at `wiki/runtime-cxtg/<path-to-file>.md`.

---

## Choosing a format type

| File is… | Format |
|---|---|
| All non-code files (docs, config, SVGs, symlinks, markdown) | **D** |
| Pre-existing upstream file modified by this project | **B** — index to sub-pages |
| New file written by this project | **A** to start — convert to **C** if wiki output > 125 raw lines |

**A → C conversion:** After writing a new Format A page (or after updating one), run `wc -l <wiki-page.md>`. If > 125, convert to Format C: add a `## Index` section with URL-encoded heading links, and add `*↑ [back to top](#Index)*` at the end of each `###` section. Format C pages never convert back to A.

**Checking Format B eligibility (submodules only):** All files in `runtime-cxtg/` are de-novo; none are Format B. For submodule files (e.g., `qemu-cxtg/`), check against the upstream release tag — NOT the project's own `cxtg` branch, which will accumulate project files over time:
```bash
git -C qemu-cxtg show v10.2.90:<path-relative-to-submodule-root> 2>/dev/null \
  && echo "upstream" || echo "new"
```
Verify the tag with `git -C qemu-cxtg tag` if uncertain.

---

## Front matter

Every **main page** (not sub-pages) starts with:
```yaml
---
format: A
---
```
Replace `A` with the correct format letter. Obsidian parses this silently — visible in the Properties panel, not in the rendered body. Sub-pages (Format B only) get no front matter.

---

## Navigation line

Every page except `wiki/index.md` and `wiki/schema.md` gets a navigation line as the **first line** of the visible body (after front matter on main pages), followed by a blank line, before the H1 title. The line combines italic links separated by `|`:

```
*← [Main Index](path/to/index.md)* | *↑ [parent-basename](../parent-file.md)* | *↗ [view source](path/to/source)*
```

- **`← Main Index`** — always present
- **`↑ parent-basename`** — sub-pages only; omit on main pages
- **`↗ view source`** — always present (all pages except `wiki/index.md` and `wiki/schema.md`)

`wiki/schema.md` is special: it gets only `*← [Main Index](index.md)*` (no source link — it is a meta-file, not a source wiki page).

All paths are relative to the wiki file's location. The vault root is the repo root.

The relative path to `wiki/index.md` depends on directory depth from `wiki/`:

| Depth from `wiki/` | Example | index.md path |
|---|---|---|
| 1 | `wiki/runtime-cxtg/Makefile.md` | `../index.md` |
| 2 | `wiki/runtime-cxtg/docs/bugs.md.md` | `../../index.md` |
| 3 | `wiki/runtime-cxtg/qemu-cxtg/disas/riscv.c.md` | `../../../index.md` |
| 4 | `wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu.c.md` | `../../../../index.md` |
| 5 | `wiki/runtime-cxtg/qemu-cxtg/target/riscv/insn_trans/…` | `../../../../../index.md` |

Sub-pages are one level deeper than their parent main page.

---

## Format A — small de novo file (full text walkthrough)

Used for new files whose wiki page is 125 raw lines or fewer (checked with `wc -l`).

```
*← [Main Index](../../index.md)* | *↗ [view source](../../../source/file)*

# <path/to/file>

<One or two sentences: purpose and context. What does this file do and when is it used?>

**Works with:**
- [other-file.ext](other-file.ext.md) — why this file depends on it

---

### <Section heading — name of symbol or code block>

```code
<the code, verbatim>
```

<Explanation: what this does and why. Non-obvious invariants only.>

---

### <Next section>
...
```

---

## Format B — pre-existing file, index to sub-pages

Used for any upstream file that this project modified. The file page is an index; each added/changed symbol gets its own sub-page.

```
*← [Main Index](../../index.md)* | *↗ [view source](../../../source/file)*

# <path/to/file>

<One sentence: what this file is in QEMU/Linux, not what we changed.>

## <Category heading>

- [symbol_name](filename.ext/symbol_name.md) — one-line description of what this entry does
- [another_symbol](filename.ext/another_symbol.md) — ...
```

Sections group related symbols (`## Extension registration`, `## CPU reset`, `## Access predicates`, etc.). Each bullet links to a sub-page.

### Sub-page format (Format B only)

```
*← [Main Index](../../../index.md)* | *↑ [parent-basename](../parent-file.md)* | *↗ [view source](../../../../source/file)*

# <path/to/file> — <symbol name or label>

<One sentence: what this function/entry does.>

```c
<containing function or struct signature> {
    ...
    <the specific lines added or changed>
    ...
}
```

<Explanation if needed. Describe WHY, not WHAT. Omit if the code is self-evident.>
```

**Sub-page rules:**
- The code snippet must include the enclosing function signature or struct definition — never use a comment like `/* in CPURISCVState */` as a substitute.
- Show only enough surrounding code to establish context; do not paste the whole function.
- One sub-page per discrete symbol or action. Combine only when two changes are inseparable.
- If a symbol lives inside a conditional block (`#if`, `CONFIG_USER_ONLY`, etc.), document the condition in the explanation — the guard is part of the behavior.
- **Exception — trivial inline changes:** A one-or-two line change with no behavioral logic (debug `printf`, log statement, comment) may be described as a plain bullet in the main page instead of a sub-page, provided the bullet names the function and describes the change.
- **When renaming a symbol:** update the sub-page filename and the bullet link text in the main page. 

---

## Format C — large de novo file (anchor-linked index + walkthrough)

Used for new files whose wiki page exceeds 125 raw lines (checked with `wc -l`).

```
*← [Main Index](../../index.md)* | *↗ [view source](../../../source/file)*

# <path/to/file>

<Description. Works with: block.>

---

## Index

- [Jump to full text walkthrough](#Full%20text%20walkthrough)

**<Group>**
- [`SYMBOL`](#anchor) — brief label

---

## Full text walkthrough

### <Section>

```code
...
```

<Explanation>

*↑ [back to top](#Index)*
```

Each `###` section ends with a blank line then a back-to-top link, just above the `---` divider of the next section (or at end of file for the last section):

```
*↑ [back to top](#Index)*
```

Same italic link style as the navigation line. Obsidian resolves heading anchors by URL-encoded heading text, not slugs. `#Index` works as-is (single word). For multi-word headings use `%20` for spaces (e.g. `#Full%20text%20walkthrough`). Do not use `{#anchor}` tags on headings — Obsidian does not support them.

---

## Format D — self-explanatory file

Used for configuration files, documentation files, or anything whose content is largely self-evident.

```
*← [Main Index](../../index.md)* | *↗ [view source](../../../source/file)*

# <path/to/file>

<Summary: what this file is and what it controls.>

## <Non-obvious section> (optional)

<Context, rationale, or constraints not visible from the file itself.>
```

---

## Content rules (apply to all formats)

**Always:**
- Use current symbol names. If a symbol was renamed, use the current name.
- Write in present tense. The wiki is a live manual, not a changelog.
- Show function or struct signatures as context in code snippets.

**Never:**
- Phase or Block annotations (`**Phase 0:** ...`, `**Block 1.1:** ...`). History belongs in `docs/CHANGELOG.md`, not the wiki.
- Line number references (`(line 183)`, `line 5567`). Line numbers break as upstream evolves. Use function names.
- History prose ("this was changed from X to Y", "previously called zicx"). Show current state only.
- Comments as code context (`/* in CPURISCVState */`). Show the actual struct or function signature.
- Stale symbol names from earlier in the project (e.g. `ext_zicx`, `cxidx`, `cxdata` after the rename pass) — in text, sub-page filenames, and bullet link paths.

**Exempt:**
- `docs/adr/**` — exempt from wiki pages entirely, per `docs/superpowers/specs/2026-09-10-adr-system-design.md` §7.2.

---

## `wiki/index.md` entry format

Each row in the index table:

```
| [path/to/file](runtime-cxtg/path/to/file.md) | One-line description of what the file is |
```

The description describes the file itself, not what the project did to it.

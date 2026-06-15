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
| New file written entirely by this project | **A** (small) or **C** (large) — split on whether the full walkthrough fits in one screen |
| Pre-existing file we added symbols/lines to | **B** — index to sub-pages |
| A config/docs file where the content is self-evident | **D** — summary + non-obvious context |

When in doubt, prefer **B** for any pre-existing upstream file.

---

## Format A — small de novo file (full text walkthrough)

Used for new files short enough to document in one page.

```
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
# <path/to/file>

<One sentence: what this file is in QEMU/Linux, not what we changed.>

## <Category heading>

- [symbol_name (brief label)](filename.ext/symbol_name.md) — one-line description of what this entry does
- [another_symbol](filename.ext/another_symbol.md) — ...
```

Sections group related symbols (`## Extension registration`, `## CPU reset`, `## Access predicates`, etc.). Each bullet links to a sub-page.

### Sub-page format (Format B only)

```
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

---

## Format C — large de novo file (anchor-linked index + walkthrough)

Used for new files too long to scan without navigation.

```
# <path/to/file>

<Description. Works with: block.>

---

## Index

- [Jump to full text walkthrough](#full-text-walkthrough)

**<Group>**
- [`SYMBOL`](#anchor) — brief label

---

## Full text walkthrough

### <Section> {#anchor}

```code
...
```

<Explanation>
```

---

## Format D — self-explanatory file

Used for configuration files, documentation files, or anything whose content is largely self-evident.

```
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
- Phase or Block annotations (`**Phase 0:** ...`, `**Block 1.1:** ...`). History belongs in release notes.
- Line number references (`(line 183)`, `line 5567`). Line numbers break as upstream evolves. Use function names.
- History prose ("this was changed from X to Y", "previously called zicx"). Show current state only.
- Comments as code context (`/* in CPURISCVState */`). Show the actual struct or function signature.
- Stale symbol names from earlier in the project (e.g. `ext_zicx`, `cxidx`, `cxdata` after the rename pass).

---

## `wiki/index.md` entry format

Each row in the index table:

```
| [path/to/file](runtime-cxtg/path/to/file.md) | One-line description of what the file is |
```

The description describes the file itself, not what the project did to it.

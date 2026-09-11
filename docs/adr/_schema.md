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
- Dates are according to UTC.
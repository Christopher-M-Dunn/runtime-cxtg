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
| [ADR-001](adr/001.md) | incorporated | all | Build flag architecture — `ext_zcx` / `ext_zcxmulti` |
| [ADR-002](adr/002.md) | deferred | all | CSR addresses, instruction encodings, and extension names |

## Bugs

| ID | Status | Phase(s) | Title |
|---|---|---|---|

## Notes

| ID | Status | Phase(s) | Title |
|---|---|---|---|
| [ADR-003](adr/003.md) | active | all | Mark provisional CSR/encoding values wherever used in code |

## Archive

| ID | Status | Phase(s) | Title |
|---|---|---|---|

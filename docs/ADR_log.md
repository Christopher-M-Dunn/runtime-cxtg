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

| ID | Status | Block(s) | Title |
|---|---|---|---|
| [ADR-001](adr/001.md) | incorporated | all | Build flag architecture — `ext_zcx` / `ext_zcxmulti` |
| [ADR-002](adr/002.md) | deferred | all | CSR addresses, instruction encodings, and extension names |
| [ADR-004](adr/004.md) | pending | 6.3 | `scxstp.mode=2` (Indirect) when ZcxMulti is absent |
| [ADR-005](adr/005.md) | pending | 2.3 | `cxsel` invalid-selector handling — the `~0` clamp model |
| [ADR-007](adr/007.md) | deferred | ? | Proposed WARL simplification for `~0` and table validity |

## Bugs

| ID | Status | Block(s) | Title |
|---|---|---|---|

## Notes

| ID | Status | Block(s) | Title |
|---|---|---|---|
| [ADR-003](adr/003.md) | active | all | Mark provisional CSR/encoding values wherever used in code |
| [ADR-006](adr/006.md) | active | all | Comment the `cxsel`-vs-`~0` readback shortcut wherever used |
| [ADR-008](adr/008.md) | retired | all | Revisit `Composable-Extensions.md` against the current draft spec |
| [ADR-009](adr/009.md) | active | all | Sync `CXTG_QEMU_Action_Plan.md` with recent `todo.md` / `Composable-Extensions.md` edits |
| [ADR-010](adr/010.md) | retired | all | Incorporate `Requirements.md` into `Composable-Extensions.md`'s `## Discussion` |
| [ADR-011](adr/011.md) | active | all | Sync `README.md` |
| [ADR-012](adr/012.md) | retired | all | Documentation prep for Action Plan update — pivot to the ADR system |
| [ADR-013](adr/013.md) | active | all | Rework `Composable-Extensions.md`'s Discussion section for conciseness and ADR references |
| [ADR-014](adr/014.md) | active | all | Wiki lint procedure |
| [ADR-015](adr/015.md) | active | all | Phase/block-level gate-check flow is undocumented beyond one sentence |

## Archive

| ID | Status | Block(s) | Title |
|---|---|---|---|

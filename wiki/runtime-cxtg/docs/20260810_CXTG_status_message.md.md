---
format: D
---

*← [Main Index](../../index.md)* | *↗ [view source](../../../docs/20260810_CXTG_status_message.md)*

# docs/20260810_CXTG_status_message.md

Jan Gray's 2026-08-10 status message to the CX TG, summarizing the task group's purpose, history, and the state of each section of the draft spec. Reproduced verbatim; it is one member's stated opinion, not a TG position.

## Scope

Non-normative. Useful as a read on which parts of the spec are stable enough to implement against and which are contested — it grades each spec section A–F for done-ness and names where consensus is absent.

## Contents

- Timeline — 2018 isa-dev origins through SoftCPU SIG, the 2022 basis spec, TG formation, and current draft work
- Per-section assessment of the draft spec, in charter-deliverable order, with a done-ness grade for each
- Note that the current TG spec has no known emulation or hardware prototype, while the basis spec has several

## Points bearing on this project

- **Unpriv ISA (`cxsel`/`cxsetsel`) — graded A.** The most stable ground; low risk of churn.
- **Priv ISA (access control, context management) — graded B/D.** No TG consensus. An alternative single privileged selection-table approach, reflecting the basis spec's `scx_table`/`cx_index`/`mcx_selector`, is favored by the author over the current spec text. This is the ground Phase 6 sits on.
- **Unpriv state management — graded B**, with open disagreement on zero/discard/reinit semantics — the same ground as the `cxdiscard`/`scxdiscard` Discussion item.
- **Logic interface — placeholder.** CXU-LI vs. CV-X-IF undecided; outside this project's scope.
- **API/ABI — graded B, ABI and versioning C.** Two overlapping calling-convention specs remain to be reconciled.

## Relation to other documents

- [docs/basis_spec_comparison.md](basis_spec_comparison.md.md) — the basis-spec lineage this message refers to
- [docs/Composable-Extensions.md](Composable-Extensions.md.md) — the Discussion items these assessments corroborate
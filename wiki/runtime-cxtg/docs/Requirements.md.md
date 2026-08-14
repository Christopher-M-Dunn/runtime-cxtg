---
format: D
---

*← [Main Index](../../index.md)* | *↗ [view source](../../../docs/Requirements.md)*

# docs/Requirements.md

Offline copy of the CX TG's requirements page — what the TG *wants to achieve* for its ISA and non-ISA work products. The live copy lives on the RISC-V wiki; this copy carries a note naming its draft date.

## Scope

Direction, not settled requirements. Requirements here indicate where the spec is likely heading, but their details have not been worked out — the technical discussions about whether a given item is impractical or even possible have not happened yet. Nothing here belongs in the requirement sections of `Composable-Extensions.md`; it enters that document only through `## Discussion`, cited as TG intent, the same way basis-spec material is siloed.

The Action Plan treats it differently: capturing TG intent is what the Action Plan is for, so requirements here are directly actionable on block scope and phase ordering.

## Classification

Every requirement carries one of three tags, and the tag is the useful part — it marks which ground is settled and which is still live:

| Tag | Meaning |
|---|---|
| unmarked | proposed for inclusion |
| `[discuss]` | undecided whether it warrants inclusion |
| `[exclude]` | proposed for exclusion; potentially future work |

The document states that all `[discuss]` items are expected to be resolved to one of the other two before work commences.

## Relation to other documents

- [docs/CHARTER.md](CHARTER.md.md) — the objectives these requirements decompose
- [docs/Composable-Extensions.md](Composable-Extensions.md.md) — literal spec requirements; far more fleshed out, and records decisions rather than intent
- [docs/basis_spec_comparison.md](basis_spec_comparison.md.md) — siloed the same way: informs `## Discussion` only
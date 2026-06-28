*← [Main Index](../../index.md)*

# docs/CXTG_QEMU_Action_Plan.md

Master phased implementation plan for the Zcx/ZcxMulti QEMU implementation.

## Structure

Each phase section contains blocks with: objective, prerequisites, repos/files touched, implementation notes, tests, definition of done, and merge action.

## Phases

| Phase | Focus |
|-------|-------|
| 0 | Foundation — integrate Artur's branch, audit, build |
| 1 | Unpriv CSR and instruction scaffolding |
| 2 | Direct mode semantics |
| 3 | Context status (scxxs0-3) |
| 4 | CX state access: first CX and cxsidx/cxsdata |
| 5 | Runtime, API, and device tree |
| 6 | ZcxMulti: per-context status and indirect mode |
| 7 | Stateen integration |
| 8 | Build matrix and full integration |
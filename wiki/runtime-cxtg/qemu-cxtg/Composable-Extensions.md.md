---
format: D
---

*← [Main Index](../../index.md)* | *↗ [view source](../../../qemu-cxtg/Composable-Extensions.md)*

# qemu-cxtg/Composable-Extensions.md

CX extension requirements document.

## Contents

- Per-CSR behavior rules for cxsel (URO), cxsidx, cxsdata, and the privileged CSRs (scxstp, scxxs, scxNxs, scxdiscard)
- `cxsetsel` instruction semantics
- Atomics semantics
- Example CX context switch (verbatim spec assembly)
- Composability Criteria: definitions, the six if-and-only-if criteria, composability examples
- CX API: types, functions, isolation models, calling conventions, library examples, versioning, spec TODO list
- External specifications (spec Appendix A): devicetree `cxs.yaml` binding, psABI (`EF_RISCV_RVCX`, calling conventions), Linux hwprobe keys and prctl interface
- Unprivileged architecture models (spec Appendix C, non-normative)
- Empty/placeholder spec sections, including the orphaned `cxsel-format.adoc`
- Open questions
- Test cases

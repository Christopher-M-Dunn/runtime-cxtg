---
format: D
---

*← [Main Index](../../index.md)* | *↗ [view source](../../../qemu-cxtg/spec.md)*

# qemu-cxtg/spec.md

Zcx implementation specification. Authors: Artur Lojewski, Christopher Dunn.

## Contents

- `## Architecture` — CSR table (0xCA0/0x018/0x019, access modes), cxsel field layout (CXID/SID), cxsetsel WARL clamping rules
- `## CSR Behavior` — per-CSR semantics: access rules, reset values, read-only enforcement for cxsel, cxsidx auto-increment via cxsdata op handler
- `## Implementation Details` — file structure, CPU state fields, configuration system, CSR predicates, handlers, csr_ops[] registration
- `## Testing` — unit, integration, and functional test descriptions
- `## Compliance` — privilege spec level, CSR address ranges, QEMU coding standards

## Update policy

Review and update the full document at the end of each development phase. Use `git diff <prev-tag>..HEAD -- qemu-cxtg/` to surface what changed, then correct stale names, addresses, field names, and code examples to match current implementation.
# qemu-cxtg/spec.md

Zicx implementation specification. Author: Artur Lojewski.

**Phase 0:** created; modified to update `## Commit History` and `## Contact` (maintainer, branch, version).

## Contents

- CSR layout: cxsel (0x800), cxsidx (0x801), cxsdata (0x802)
- Instruction encoding for cxsetsel
- Privilege model
- QEMU integration notes

## Note

Extension name is `zcx` (not `zicx` as written in the spec); CSR addresses are stubs pending spec finalisation — see `docs/todo.md`.

# docs/todo.md

Deferred decisions and open questions.

**Phase 0:** created with initial deferred items.

## Deferred items (Phase 0)

- Build flag architecture — only `ext_zcx` and `ext_zcxmulti` runtime flags needed; no `CONFIG_*` compile-time symbols required
- `CSR_CXSEL` address set to `0x800` (stub); spec draft says `0xCC0` — confirm with spec owners
- `CSR_CXSIDX` (`0x801`) and `CSR_CXSDATA` (`0x802`) addresses pending spec finalisation
- Extension name `zicx` change to `zcx` — deferred to Phase 1
- `cxsetsel` instruction encoding TBD — SYSTEM opcode (`0x73`), funct3 slot unassigned

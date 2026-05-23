# qemu-cxtg/target/riscv/csr.c

CSR read/write handlers, access predicates, and the `csr_ops[]` dispatch table. The central file for all RISC-V CSR behavior (~8000 lines).

**Phase 0:** modified — implemented handlers and predicates for cxsel, cxsidx, cxsdata; registered them in `csr_ops`.

## Access predicates

- [cxsel (predicate)](csr.c/pred_cxsel.md) — gates access to CSR_CXSEL; returns `ILLEGAL_INST` unless `ext_zicx` is set
- [cxsidx (predicate)](csr.c/pred_cxsidx.md) — gates access to CSR_CXSIDX; returns `ILLEGAL_INST` unless `ext_zicx` is set
- [cxsdata (predicate)](csr.c/pred_cxsdata.md) — gates access to CSR_CXSDATA; returns `ILLEGAL_INST` unless `ext_zicx` is set

## Read handlers

- [read_cxsel](csr.c/read_cxsel.md) — delegates to `cxsel_csr_read()`; returns `RISCV_EXCP_NONE`
- [read_cxsidx](csr.c/read_cxsidx.md) — delegates to `cxsidx_csr_read()`; returns `RISCV_EXCP_NONE`
- [read_cxsdata](csr.c/read_cxsdata.md) — delegates to `cxsdata_csr_read()`; returns `RISCV_EXCP_NONE`

## Write handlers

- [write_cxsel](csr.c/write_cxsel.md) — delegates to `cxsel_csr_write()` then returns `RISCV_EXCP_ILLEGAL_INST` (cxsel is read-only)
- [write_cxsidx](csr.c/write_cxsidx.md) — delegates to `cxsidx_csr_write()`; returns `RISCV_EXCP_NONE`
- [write_cxsdata](csr.c/write_cxsdata.md) — delegates to `cxsdata_csr_write()`; returns `RISCV_EXCP_NONE`

## csr_ops[] entries

- [csr_ops CX entries](csr.c/csr_ops_cx.md) — registers CSR_CXSEL, CSR_CXSIDX, CSR_CXSDATA in the dispatch table

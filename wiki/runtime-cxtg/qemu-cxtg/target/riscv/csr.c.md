# qemu-cxtg/target/riscv/csr.c

CSR read/write handlers, access predicates, and the `csr_ops[]` dispatch table. The central file for all RISC-V CSR behavior (~8000 lines).

**Phase 0:** modified — implemented handlers and predicates for cxsel, cxsidx, cxsdata; registered them in `csr_ops`.

**Block 1.1:** modified — renamed `ext_zicx` → `ext_zcx` in all three predicates; removed dead PMP copy-paste from predicate bodies; condition flipped to `!ext_zcx` (fail-closed).

## How QEMU enforces CSR read-only access

Before any predicate or handler is called, `riscv_csrrw_check()` (line 5567) tests bits [11:10] of the CSR address:

```c
bool read_only = get_field(csrno, 0xC00) == 3;
...
if (write && read_only) {
    return RISCV_EXCP_ILLEGAL_INST;
}
```

`0xC00` is the two-bit mask for bits [11:10]. The RISC-V spec reserves `csr[11:10] == 11` (= 3) for read-only CSRs. `CSR_CXSEL = 0xC20`:

```
0xC20 = 1100 0010 0000
         ^^
         bits 11:10 = 11 → read_only = true
```

Any `csrw`/`csrs`/`csrc` to `cxsel` is rejected here with `ILLEGAL_INST` before `write_cxsel` is ever reached. The `cxsel_csr_write()` implementation in `cx.c` is belt-and-suspenders only, reachable via the `cxsetsel` TCG path but not via a direct CSR write.

## Access predicates

- [cxsel (predicate)](csr.c/pred_cxsel.md) — gates access to CSR_CXSEL; returns `ILLEGAL_INST` unless `ext_zcx` is set
- [cxsidx (predicate)](csr.c/pred_cxsidx.md) — gates access to CSR_CXSIDX; returns `ILLEGAL_INST` unless `ext_zcx` is set
- [cxsdata (predicate)](csr.c/pred_cxsdata.md) — gates access to CSR_CXSDATA; returns `ILLEGAL_INST` unless `ext_zcx` is set

## Read handlers

- [read_cxsel](csr.c/read_cxsel.md) — delegates to `cxsel_csr_read()`; returns `RISCV_EXCP_NONE`
- [read_cxsidx](csr.c/read_cxsidx.md) — delegates to `cxsidx_csr_read()`; returns `RISCV_EXCP_NONE`
- [read_cxsdata](csr.c/read_cxsdata.md) — delegates to `cxsdata_csr_read()`; returns `RISCV_EXCP_NONE`

## Write handlers

- [write_cxsel](csr.c/write_cxsel.md) — calls `cxsel_csr_write()` (no-op) and returns `RISCV_EXCP_NONE`; never reached on a direct `csrw` because `riscv_csrrw_check` traps first (see above)
- [write_cxsidx](csr.c/write_cxsidx.md) — delegates to `cxsidx_csr_write()`; returns `RISCV_EXCP_NONE`
- [write_cxsdata](csr.c/write_cxsdata.md) — delegates to `cxsdata_csr_write()`; returns `RISCV_EXCP_NONE`

## csr_ops[] entries

- [csr_ops CX entries](csr.c/csr_ops_cx.md) — registers CSR_CXSEL, CSR_CXSIDX, CSR_CXSDATA in the dispatch table

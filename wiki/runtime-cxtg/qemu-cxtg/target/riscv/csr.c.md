# qemu-cxtg/target/riscv/csr.c

CSR read/write handlers, access predicates, and the `csr_ops[]` dispatch table. The central file for all RISC-V CSR behavior (~8000 lines).

## How QEMU enforces CSR read-only access

Before any predicate or handler is called, `riscv_csrrw_check()` tests bits [11:10] of the CSR address:

```c
bool read_only = get_field(csrno, 0xC00) == 3;
...
if (write && read_only) {
    return RISCV_EXCP_ILLEGAL_INST;
}
```

`0xC00` is the two-bit mask for bits [11:10]. The RISC-V spec reserves `csr[11:10] == 11` (= 3) for read-only CSRs. `CSR_CXSEL = 0xCA0`:

```
0xCA0 = 1100 1010 0000
         ^^
         bits 11:10 = 11 → read_only = true
```

Any `csrw`/`csrs`/`csrc` to `cxsel` is rejected here with `ILLEGAL_INST` before `write_cxsel` is ever reached.

## Access predicates

- [cxsel (predicate)](csr.c/pred_cxsel.md) — gates access to CSR_CXSEL; returns `ILLEGAL_INST` unless `ext_zcx` is set
- [cxsidx (predicate)](csr.c/pred_cxsidx.md) — gates access to CSR_CXSIDX; returns `ILLEGAL_INST` unless `ext_zcx` is set
- [cxsdata (predicate)](csr.c/pred_cxsdata.md) — gates access to CSR_CXSDATA; returns `ILLEGAL_INST` unless `ext_zcx` is set

## Handlers

- [read_cxsel](csr.c/read_cxsel.md) — delegates to `cxsel_csr_read()`
- [write_cxsel](csr.c/write_cxsel.md) — delegates to `cxsel_csr_write()` (no-op); unreachable via direct CSR write (read-only trap fires first)
- [read_cxsidx](csr.c/read_cxsidx.md) — delegates to `cxsidx_csr_read()`
- [write_cxsidx](csr.c/write_cxsidx.md) — delegates to `cxsidx_csr_write()`
- [op_cxsdata](csr.c/op_cxsdata.md) — combined `.op` handler; delegates to `cxsdata_csr_op()`

## csr_ops[] entries

- [csr_ops CX entries](csr.c/csr_ops_cx.md) — registers CSR_CXSEL, CSR_CXSIDX, CSR_CXSDATA in the dispatch table
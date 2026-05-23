# qemu-cxtg/target/riscv/cx.h

CX extension CSR interface declarations. Header guard: `RISCV_CX_H`.

**Phase 0:** created

## Function prototypes

All implementations are in [cx.c](cx.c.md).

- `cxsel_csr_read` / `cxsel_csr_write`
- `cxsidx_csr_read` / `cxsidx_csr_write`
- `cxsdata_csr_op` — combined `.op` handler; replaces separate read/write to ensure single cxsidx increment per instruction

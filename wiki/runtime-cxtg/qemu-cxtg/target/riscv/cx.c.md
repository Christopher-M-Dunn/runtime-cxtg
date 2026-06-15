# qemu-cxtg/target/riscv/cx.c

CX extension CSR implementations. Provides the concrete read/write logic for cxsel, cxsidx, and cxsdata, with QEMU trace events on every access.

## Functions

- [cxsel_csr_read](cx.c/cxsel_csr_read.md) — reads `env->cxsel`; emits `trace_cxsel_csr_read`
- [cxsel_csr_write](cx.c/cxsel_csr_write.md) — no-op stub; cxsel write-trap is enforced by address encoding (URO), not this handler
- [cxsidx_csr_read](cx.c/cxsidx_csr_read.md) — reads `env->cxsidx`; emits `trace_cxsidx_csr_read`
- [cxsidx_csr_write](cx.c/cxsidx_csr_write.md) — writes `env->cxsidx`; emits `trace_cxsidx_csr_write`
- [cxsdata_csr_op](cx.c/cxsdata_csr_op.md) — combined `.op` handler for all cxsdata variants (csrr/csrw/csrrw/csrrs/csrrc); reads old value if `ret_value != NULL`, writes if `write_mask != 0`, then increments `cxsidx` exactly once regardless of variant

# qemu-cxtg/target/riscv/cx.c

CX extension CSR implementations. Provides the concrete read/write logic for cxsel, cxsidx, and cxsdata, with QEMU trace events on every access.

**Phase 0:** created

## Functions

- [cxsel_csr_read](cx.c/cxsel_csr_read.md) — reads `env->cxsel`; emits `trace_cxsel_csr_read`
- [cxsel_csr_write](cx.c/cxsel_csr_write.md) — no-op (cxsel is read-only per spec); emits `trace_cxsel_csr_write`
- [cxsidx_csr_read](cx.c/cxsidx_csr_read.md) — reads `env->cxidx`; emits `trace_cxsidx_csr_read`
- [cxsidx_csr_write](cx.c/cxsidx_csr_write.md) — writes `env->cxidx`; emits `trace_cxsidx_csr_write`
- [cxsdata_csr_read](cx.c/cxsdata_csr_read.md) — reads `env->cxdata`; emits `trace_cxsdata_csr_read`
- [cxsdata_csr_write](cx.c/cxsdata_csr_write.md) — writes `env->cxdata`; emits `trace_cxsdata_csr_write`

---
format: B
---

*← [Main Index](../../../../index.md)* | *↗ [view source](../../../../../qemu-cxtg/target/riscv/trace-events)*

# qemu-cxtg/target/riscv/trace-events

QEMU trace event definitions for the RISC-V target. Each entry generates a `trace_*()` function callable from C code.


## Trace events added

- [cxsel_csr_read](trace-events/cxsel_csr_read.md) — fires on every read of CSR_CXSEL; logs hartid, reg index, value
- [cxsel_csr_write](trace-events/cxsel_csr_write.md) — fires on every write attempt to CSR_CXSEL; logs hartid, reg index, value
- [cxsidx_csr_read](trace-events/cxsidx_csr_read.md) — fires on every read of CSR_CXSIDX; logs hartid, reg index, value
- [cxsidx_csr_write](trace-events/cxsidx_csr_write.md) — fires on every write to CSR_CXSIDX; logs hartid, reg index, value
- [cxsdata_csr_read](trace-events/cxsdata_csr_read.md) — fires on every read of CSR_CXSDATA; logs hartid, reg index, value
- [cxsdata_csr_write](trace-events/cxsdata_csr_write.md) — fires on every write to CSR_CXSDATA; logs hartid, reg index, value
- [cxsetsel](trace-events/cxsetsel.md) — fires on every cxsetsel instruction execution; logs hartid, new cxsel value, old cxsel value

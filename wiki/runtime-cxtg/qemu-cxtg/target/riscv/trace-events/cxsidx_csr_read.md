*← [Main Index](../../../../../index.md)* | *↑ [trace-events](../trace-events.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/trace-events)*

# target/riscv/trace-events — cxsidx_csr_read

trace event: fires on every read of CSR_CXSIDX; logs hart ID, register index, and value read; generates `trace_cxsidx_csr_read()` callable from `cx.c`

```
cxsidx_csr_read(uint64_t mhartid, uint32_t addr_index, uint64_t val) "hart %" PRIu64 ": read reg %" PRIu32", val: 0x%" PRIx64
```

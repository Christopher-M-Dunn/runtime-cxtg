*← [Main Index](../../../../../index.md)* | *↑ [trace-events](../trace-events.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/trace-events)*

# target/riscv/trace-events — cxsdata_csr_write

trace event: fires on every write to CSR_CXSDATA; logs hart ID, register index, and value written; generates `trace_cxsdata_csr_write()` callable from `cx.c`

```
cxsdata_csr_write(uint64_t mhartid, uint32_t addr_index, uint64_t val) "hart %" PRIu64 ": write reg %" PRIu32", val: 0x%" PRIx64
```

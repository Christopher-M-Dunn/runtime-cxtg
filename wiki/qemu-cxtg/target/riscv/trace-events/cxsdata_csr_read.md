# target/riscv/trace-events — cxsdata_csr_read

trace event: fires on every read of CSR_CXSDATA; logs hart ID, register index, and value read; generates `trace_cxsdata_csr_read()` callable from `cx.c`

```
cxsdata_csr_read(uint64_t mhartid, uint32_t addr_index, uint64_t val) "hart %" PRIu64 ": read reg %" PRIu32", val: 0x%" PRIx64
```

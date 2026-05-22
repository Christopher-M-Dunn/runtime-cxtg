# target/riscv/trace-events — cxsel_csr_read

trace event: fires on every read of CSR_CXSEL; logs hart ID, register index, and value read; generates `trace_cxsel_csr_read()` callable from `cx.c`

```
cxsel_csr_read(uint64_t mhartid, uint32_t addr_index, uint64_t val) "hart %" PRIu64 ": read reg %" PRIu32", val: 0x%" PRIx64
```

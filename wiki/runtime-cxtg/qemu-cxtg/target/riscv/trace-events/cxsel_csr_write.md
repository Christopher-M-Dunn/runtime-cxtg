# target/riscv/trace-events — cxsel_csr_write

trace event: fires on every write attempt to CSR_CXSEL; logs hart ID, register index, and attempted value; generates `trace_cxsel_csr_write()` callable from `cx.c`

```
cxsel_csr_write(uint64_t mhartid, uint32_t addr_index, uint64_t val) "hart %" PRIu64 ": write reg %" PRIu32", val: 0x%" PRIx64
```

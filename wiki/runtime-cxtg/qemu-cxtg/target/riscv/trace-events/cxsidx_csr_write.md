# target/riscv/trace-events — cxsidx_csr_write

trace event: fires on every write to CSR_CXSIDX; logs hart ID, register index, and value written; generates `trace_cxsidx_csr_write()` callable from `cx.c`

```
cxsidx_csr_write(uint64_t mhartid, uint32_t addr_index, uint64_t val) "hart %" PRIu64 ": write reg %" PRIu32", val: 0x%" PRIx64
```

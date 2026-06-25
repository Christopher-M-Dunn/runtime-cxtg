# target/riscv/trace-events — cxsetsel

Trace event: fires on every cxsetsel instruction execution; logs hart ID, new cxsel value written, and old cxsel value replaced; generates `trace_cxsetsel()` callable from `cx.c`.

```
cxsetsel(uint64_t mhartid, uint64_t new_val, uint64_t old_val) "hart %" PRIu64 ": new=0x%" PRIx64 " old=0x%" PRIx64
```

Unlike the CSR read/write events, this event has no `addr_index` parameter because cxsetsel targets a single implied register (cxsel). The old value is captured before the write so callers can observe the swap atomically in the trace log.
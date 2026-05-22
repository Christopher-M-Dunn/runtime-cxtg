# target/riscv/csr.c — csr_ops[] CX entries

table entries: registers CSR_CXSEL, CSR_CXSIDX, CSR_CXSDATA in the `csr_ops[]` dispatch table; wires each to its predicate and read/write handlers

```c
    /* CX extension CSRs */
    [CSR_CXSEL]          = { "cxsel",   cxsel,   read_cxsel,   write_cxsel   },
    [CSR_CXSIDX]         = { "cxsidx",  cxsidx,  read_cxsidx,  write_cxsidx  },
    [CSR_CXSDATA]        = { "cxsdata", cxsdata, read_cxsdata, write_cxsdata },
```

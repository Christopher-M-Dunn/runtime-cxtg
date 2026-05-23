# qemu-cxtg/target/riscv/cpu.h

RISC-V CPU state definition. Defines `CPURISCVState` (a.k.a. `CPUArchState`) and all CPU configuration structures.

**Phase 0:** modified — included `cx.h` and added three CX CSR state fields to `CPUArchState`.

## Includes added

- `#include "cx.h"` — pulls in CX CSR handler prototypes (line 183)

## CPUArchState fields added

- [cxsel](cpu.h/cxsel.md) — current CX selector value; 0 = built-in extension
- [cxsidx](cpu.h/cxidx.md) — CX state index register
- [cxsdata](cpu.h/cxdata.md) — CX state data register

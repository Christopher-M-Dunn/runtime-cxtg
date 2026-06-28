*← [Main Index](../../../../index.md)*

# qemu-cxtg/target/riscv/cpu.h

RISC-V CPU state definition. Defines `CPURISCVState` (a.k.a. `CPUArchState`) and all CPU configuration structures.

## Includes added

`cx.h` is included inside the system-mode-only block, alongside `pmp.h` and `debug.h`. It is not available in user-mode builds (`CONFIG_USER_ONLY`), which is correct — CX CSRs are hardware-level and have no meaning in Linux user-mode emulation.

```c
#if !defined(CONFIG_USER_ONLY)
#include "pmp.h"
#include "cx.h"
#include "debug.h"
#endif
```

## CPUArchState fields added

- [cxsel](cpu.h/cxsel.md) — current CX selector value; 0 = built-in extension
- [cxsidx](cpu.h/cxsidx.md) — CX state index register
- [cxsdata](cpu.h/cxsdata.md) — CX state data register

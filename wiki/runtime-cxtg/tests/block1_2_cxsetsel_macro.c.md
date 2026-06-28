---
format: A
---

*← [Main Index](../../index.md)* | *↗ [view source](../../../tests/block1_2_cxsetsel_macro.c)*

# tests/block1_2_cxsetsel_macro.c

C test for the `CXSETSEL` inline-assembly macro in `include/utils.h`. Verifies five behavioral cases: reset state, write, all-ones write, discard-rd pattern, and round-trip swap. Runs on the bare-metal harness under QEMU with `zcx=on`.

**Works with:**
- [include/utils.h](../include/utils.h.md) — provides the `CXSETSEL(rd, rs1)` macro
- [tests/common/test.h](common/test.h.md) — provides `CHECK` and `test_pass`
- [tests/common/uart.h](common/uart.h.md) — provides `uart_puts` (used by `CHECK`)
- [tests/common/crt0.S](common/crt0.S.md) — startup code and `test_pass`/`test_fail` implementations

---

### Includes

```c
#include "common/uart.h"
#include "common/test.h"
#include "../include/utils.h"
```

`uart.h` and `test.h` come from the `tests/common/` harness (via `-Itests/common`). `utils.h` is at the repo root `include/` (via `-Iinclude`), referenced with `../include/` from the test source's own directory.

---

### Case 1: swap from reset state

```c
CXSETSEL(prev, 0UL);
CHECK(prev == 0UL);
__asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
CHECK(cur == 0UL);
```

Verifies that cxsel starts at 0 on reset and that swapping 0 in returns 0 as the old value.

---

### Case 2: write a non-zero value

```c
CXSETSEL(prev, 0x0102UL);
CHECK(prev == 0UL);
__asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
CHECK(cur == 0x0102UL);
```

Writes `0x0102` into cxsel; confirms the old value returned is `0` and cxsel now reads back `0x0102`.

---

### Case 3: write all-ones

```c
CXSETSEL(prev, ~0UL);
CHECK(prev == 0x0102UL);
__asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
CHECK(cur == ~0UL);
```

Writes `~0UL` (all bits set). The stub performs no field clamping, so `~0UL` is stored verbatim. `prev` must be the previous value `0x0102`.

---

### Case 4: discard the return value

```c
{
    unsigned long discard;
    CXSETSEL(discard, 0x55UL);
    __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
    CHECK(cur == 0x55UL);
    (void)discard;
}
```

Demonstrates the pattern for callers that do not need the old value. The `CXSETSEL` macro requires an lvalue for `rd`; a local variable is used and then silenced with `(void)discard` to avoid an unused-variable warning. The write side effect is still observable via csrr.

---

### Case 5: round-trip swap

```c
{
    unsigned long old1, old2;
    CXSETSEL(old1, 0xABCDUL);
    CHECK(old1 == 0x55UL);
    CXSETSEL(old2, old1);
    CHECK(old2 == 0xABCDUL);
    __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
    CHECK(cur == 0x55UL);
}
```

Writes `0xABCD`, captures `old1` (`0x55`), then swaps `old1` back in. Confirms that `old2` is `0xABCD` and cxsel is restored to `0x55`. Tests that the swap chain is coherent across two back-to-back calls.

---

### Pass

```c
test_pass();
return 0;
```

All five cases pass; calls `test_pass()` which prints `PASSED` via UART and halts QEMU with a clean exit via `wfi` in `crt0.S`.

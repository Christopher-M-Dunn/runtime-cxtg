# tests/common/test.h

Test assertion macro and declarations for the bare-metal test harness. Included by C test files.

**Works with:**
- [crt0.S](crt0.S.md) — defines `test_pass`, `test_fail`, and `_trap_mcause`
- [uart.h](uart.h.md) — included by `test.h`; `CHECK` uses `uart_puts` for failure output

---

### Header guard and include

```c
#ifndef TEST_H
#define TEST_H

#include "uart.h"
```

Includes `uart.h` for `uart_puts`, used by the `CHECK` macro.

---

### `_trap_mcause` extern declaration

```c
extern volatile unsigned long _trap_mcause;
```

Declares the `_trap_mcause` variable defined in `crt0.S`. `volatile` because the trap handler (running asynchronously from the test's perspective) writes to it. Tests read this immediately after an instruction expected to trap, then assert the expected `mcause` value.

---

### `CHECK` macro

```c
#define CHECK(cond) \
    do { \
        if (!(cond)) { \
            uart_puts("FAIL: " #cond "\n"); \
            test_fail(); \
        } \
    } while (0)
```

Assert that `cond` is true. On failure, prints the stringified condition and calls `test_fail()`. The `do { } while (0)` wrapper makes the macro safe to use in if/else without braces.

---

### `MCAUSE_ILLEGAL_INST`

```c
#define MCAUSE_ILLEGAL_INST 2UL
```

Expected `mcause` value when a CSR access is rejected because its extension is disabled. RISC-V Privileged Spec: cause 2 = illegal instruction. Used in assertions of the form `CHECK(_trap_mcause == MCAUSE_ILLEGAL_INST)`.

---

### `test_pass` / `test_fail` declarations

```c
extern void test_pass(void);
extern void test_fail(void);
```

Declarations for the global symbols defined in `crt0.S`. C tests call `test_pass()` at the end of `main` when all `CHECK`s have passed.
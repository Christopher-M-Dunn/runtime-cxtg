---
format: C
---

*← [Main Index](../../../index.md)* | *↗ [view source](../../../../tests/common/crt0.S)*

# tests/common/crt0.S

M-mode bare-metal startup for QEMU virt (RV64). Entry at `_start` (0x80000000). Provides `test_pass`, `test_fail`, and the `_trap_mcause` variable used by tests to assert expected exception causes.

**Works with:**
- [link.ld](link.ld.md) — defines `_stack_top`, `_bss_start`, `_bss_end`; places `.text.start` first
- [test.h](test.h.md) — declares `extern volatile unsigned long _trap_mcause`, `test_pass`, `test_fail`; provides `CHECK` macro
- [uart.h](uart.h.md) — C-callable UART helpers for tests that include it (crt0.S has its own internal `_puts`)

---

## Index

- [Jump to full text walkthrough](#Full%20text%20walkthrough)

**Startup**
- [`.text.start` section header](#.text.start%20section%20header) — dedicated input section placed first by linker
- [`_start` — stack and trap handler setup](#_start%20—%20stack%20and%20trap%20handler%20setup) — sp init, mtvec install
- [`_start` — `.bss` zero loop](#_start%20—%20.bss%20zero%20loop) — clears bss before main
- [`_start` — call main](#_start%20—%20call%20main) — transfer to test; failure fallthrough

**Trap handling**
- [`_trap_handler` — M-mode exception handler](#_trap_handler%20—%20M-mode%20exception%20handler) — records mcause, advances mepc

**UART and exit**
- [`_puts` — internal UART write](#_puts%20—%20internal%20UART%20write) — null-terminated string to MMIO UART
- [`test_pass` / `test_fail` — test exit points](#test_pass%20%2F%20test_fail%20—%20test%20exit%20points) — print result and spin

**Data sections**
- [`.rodata` — exit strings](#.rodata%20—%20exit%20strings) — `PASSED\n` / `FAILED\n`
- [`.bss` — `_trap_mcause`](#.bss%20—%20_trap_mcause) — 8-byte slot written by trap handler

---

## Full text walkthrough

### `.text.start` section header

```asm
.section .text.start
.global _start
```

Dedicated input section placed before `.text` by the linker script, ensuring `_start` is the very first instruction in the image. QEMU virt boot ROM jumps to 0x80000000 after loading the ELF.

*↑ [back to top](#Index)*

---

### `_start` — stack and trap handler setup

```asm
_start:
        la      sp, _stack_top
        la      t0, _trap_handler
        csrw    mtvec, t0
```

Sets sp from the linker symbol `_stack_top` (top of the 16 KiB stack above `.bss`). Installs `_trap_handler` into `mtvec` in direct mode. CPU is already in M-mode at reset.

*↑ [back to top](#Index)*

---

### `_start` — `.bss` zero loop

```asm
        la      t0, _bss_start
        la      t1, _bss_end
_bss_zero:
        bge     t0, t1, _bss_done
        sd      zero, 0(t0)
        addi    t0, t0, 8
        j       _bss_zero
_bss_done:
```

Clears `.bss` in 8-byte increments. Linker script guarantees 8-byte alignment of `_bss_start` and `_bss_end`. This zeroes `_trap_mcause` before `main` is called.

*↑ [back to top](#Index)*

---

### `_start` — call main

```asm
        call    main
        call    test_fail
```

Transfers control to the test's `main`. If `main` returns without calling `test_pass` or `test_fail`, the fallthrough `call test_fail` fires — treated as a test failure.

*↑ [back to top](#Index)*

---

### `_trap_handler` — M-mode exception handler

```asm
        .align  2
_trap_handler:
        csrr    t0, mcause
        la      t1, _trap_mcause
        sd      t0, 0(t1)
        csrr    t0, mepc
        addi    t0, t0, 4
        csrw    mepc, t0
        mret
```

Records `mcause` into `_trap_mcause` so tests can assert the expected exception code after a trapping instruction. Advances `mepc` by 4 before `mret` to skip the faulting instruction — without this, `mret` would re-execute it. `.align 2` satisfies mtvec's 4-byte alignment requirement. Assumes all trapping instructions are 4 bytes (standard RV64 encoding).

*↑ [back to top](#Index)*

---

### `_puts` — internal UART write

```asm
_puts:
        li      t0, 0x10000000
_puts_loop:
        lb      t1, 0(a0)
        beqz    t1, _puts_ret
        sb      t1, 0(t0)
        addi    a0, a0, 1
        j       _puts_loop
_puts_ret:
        ret
```

Writes the null-terminated string at `a0` directly to the QEMU virt UART MMIO register at `0x10000000`. Clobbers `t0`, `t1`. Not a global symbol — only used by `test_pass` and `test_fail` in this file. C tests use the functions in `uart.h` instead.

*↑ [back to top](#Index)*

---

### `test_pass` / `test_fail` — test exit points

```asm
        .global test_pass
test_pass:
        la      a0, _str_passed
        call    _puts
1:      j       1b

        .global test_fail
test_fail:
        la      a0, _str_failed
        call    _puts
1:      j       1b
```

Global exit points for tests. Each prints its respective string over UART then spins. The spin loop is the intended terminal state — QEMU detects `"PASSED\n"` or `"FAILED\n"` in output and the `-no-reboot` flag prevents a reset loop.

*↑ [back to top](#Index)*

---

### `.rodata` — exit strings

```asm
        .section .rodata
_str_passed:    .asciz "PASSED\n"
_str_failed:    .asciz "FAILED\n"
```

Internal strings referenced only by `test_pass` and `test_fail`. Not global.

*↑ [back to top](#Index)*

---

### `.bss` — `_trap_mcause`

```asm
        .section .bss
        .align  3
        .global _trap_mcause
_trap_mcause:
        .dword  0
```

8-byte slot updated by `_trap_handler` on every exception. `.align 3` ensures 8-byte (dword) alignment. Zeroed by the bss loop before `main` runs. Declared `extern volatile unsigned long _trap_mcause` in `test.h`; assembly tests read it via `la`/`ld`.

*↑ [back to top](#Index)*
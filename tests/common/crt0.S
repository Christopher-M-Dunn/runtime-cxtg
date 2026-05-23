/* crt0.S — M-mode bare-metal startup for QEMU virt (RV64).
 *
 * Entry point _start is placed at 0x80000000 by the linker script.
 * QEMU virt boot ROM jumps there after loading the ELF.
 *
 * Provides:
 *   test_pass  — prints "PASSED\n" to UART and spins
 *   test_fail  — prints "FAILED\n" to UART and spins
 *
 * Exports:
 *   _trap_mcause  — updated by the trap handler on every exception;
 *                   tests read this after a trapping instruction to
 *                   assert the expected mcause value
 *
 * Trap handling:
 *   The M-mode trap handler records mcause in _trap_mcause, advances
 *   mepc by 4 (skipping the faulting instruction), and returns with
 *   mret.  Tests that expect a trap read _trap_mcause immediately
 *   after the instruction that should have faulted.
 */

        .section .text.start
        .global _start
_start:
        la      sp, _stack_top

        la      t0, _trap_handler
        csrw    mtvec, t0

        /* Zero .bss — linker script guarantees 8-byte alignment */
        la      t0, _bss_start
        la      t1, _bss_end
_bss_zero:
        bge     t0, t1, _bss_done
        sd      zero, 0(t0)
        addi    t0, t0, 8
        j       _bss_zero
_bss_done:

        call    main

        /* main returned without calling test_pass/test_fail */
        call    test_fail

        /* --- trap handler ----------------------------------------- */

        .align  2
_trap_handler:
        csrr    t0, mcause
        la      t1, _trap_mcause
        sd      t0, 0(t1)
        csrr    t0, mepc
        addi    t0, t0, 4
        csrw    mepc, t0
        mret

        /* --- UART output helpers ---------------------------------- */

        /* _puts: write null-terminated string at a0 to UART.
         * Clobbers t0, t1. */
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

        /* --- test exit points ------------------------------------- */

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

        /* --- read-only data --------------------------------------- */

        .section .rodata
_str_passed:    .asciz "PASSED\n"
_str_failed:    .asciz "FAILED\n"

        /* --- BSS -------------------------------------------------- */

        .section .bss
        .align  3
        .global _trap_mcause
_trap_mcause:
        .dword  0
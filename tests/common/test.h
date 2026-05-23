#ifndef TEST_H
#define TEST_H

#include "uart.h"

/*
 * _trap_mcause — updated by the M-mode trap handler in crt0.S each
 * time an exception fires.  Tests read this immediately after an
 * instruction expected to trap and assert the correct mcause value.
 *
 * Illegal instruction (mcause = 2) is the common case: accessing a
 * CSR whose extension is disabled raises mcause = 2.
 */
extern volatile unsigned long _trap_mcause;

/*
 * CHECK(cond) — assert that cond is true.  On failure, print the
 * condition string and call test_fail().  Tests call test_pass() at
 * the end of main when all checks have passed.
 */
#define CHECK(cond) \
    do { \
        if (!(cond)) { \
            uart_puts("FAIL: " #cond "\n"); \
            test_fail(); \
        } \
    } while (0)

/*
 * MCAUSE_ILLEGAL_INST — expected mcause value when a CSR access or
 * instruction is rejected because its extension is disabled.
 */
#define MCAUSE_ILLEGAL_INST 2UL

extern void test_pass(void);
extern void test_fail(void);

#endif /* TEST_H */
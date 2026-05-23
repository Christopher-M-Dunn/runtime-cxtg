/*
 * block1_1_csr_stubs.c — Block 1.1 test: cxsel, cxsidx, cxsdata CSR stubs
 *
 * Run with: make run-block1_1_csr_stubs
 * (QEMU_CPU defaults to rv64,zcx=on; override to test without zcx)
 *
 * Verifies:
 *   1. cxsel reads 0 at reset
 *   2. Write to cxsel (URO) traps with mcause=2 (illegal instruction)
 *   3. cxsidx read/write round-trip
 *   4. cxsdata all four CSR instruction variants (csrr, csrw, csrrw, csrrs, csrrc)
 *   5. cxsdata auto-increment: every cxsdata access increments cxsidx by exactly 1
 */

#include "test.h"

#define CSR_CXSEL   0xCA0
#define CSR_CXSIDX  0x018
#define CSR_CXSDATA 0x019

/*
 * CSR access macros — the CSR address is stringified directly into the asm
 * template so GCC never sees it as an operand.  This avoids the "i" constraint
 * mismatch that occurs when passing a CSR number through a function parameter.
 *
 * _XSTR double-expands the macro before stringifying, so CX_CSR_READ(CSR_CXSEL)
 * produces "csrr %0, 0xCC0", not "csrr %0, CSR_CXSEL".
 */
#define _STR(x)  #x
#define _XSTR(x) _STR(x)

#define CX_CSR_READ(csr) \
    ({ unsigned long __v; \
       asm volatile("csrr %0, " _XSTR(csr) : "=r"(__v) :: "memory"); \
       __v; })

#define CX_CSR_WRITE(csr, val) \
    do { unsigned long __v = (unsigned long)(val); \
         asm volatile("csrw " _XSTR(csr) ", %0" :: "r"(__v) : "memory"); \
    } while (0)

#define CX_CSR_SWAP(csr, val) \
    ({ unsigned long __old, __v = (unsigned long)(val); \
       asm volatile("csrrw %0, " _XSTR(csr) ", %1" \
                    : "=r"(__old) : "r"(__v) : "memory"); \
       __old; })

#define CX_CSR_SET_BITS(csr, mask) \
    ({ unsigned long __old, __m = (unsigned long)(mask); \
       asm volatile("csrrs %0, " _XSTR(csr) ", %1" \
                    : "=r"(__old) : "r"(__m) : "memory"); \
       __old; })

#define CX_CSR_CLEAR_BITS(csr, mask) \
    ({ unsigned long __old, __m = (unsigned long)(mask); \
       asm volatile("csrrc %0, " _XSTR(csr) ", %1" \
                    : "=r"(__old) : "r"(__m) : "memory"); \
       __old; })

int main(void)
{
    unsigned long val;

    /* 1. cxsel reads 0 at reset */
    val = CX_CSR_READ(CSR_CXSEL);
    CHECK(val == 0);

    /* 2. Write to cxsel (URO, bits[11:10]=11) must trap with illegal instruction.
     *    The trap handler in crt0.S records mcause and advances mepc+4. */
    _trap_mcause = 0;
    CX_CSR_WRITE(CSR_CXSEL, 1);
    CHECK(_trap_mcause == MCAUSE_ILLEGAL_INST);

    /* 3. cxsidx read/write round-trip */
    CX_CSR_WRITE(CSR_CXSIDX, 0x55);
    val = CX_CSR_READ(CSR_CXSIDX);
    CHECK(val == 0x55);

    /* 4+5. cxsdata: all four CSR variants, each incrementing cxsidx by exactly 1.
     *
     * Trace (cxsidx starts at 0):
     *   csrw  cxsdata, 0x00   → cxsdata=0x00, cxsidx=1
     *   csrr  cxsdata         → val=0x00,      cxsidx=2
     *   csrw  cxsdata, 0xAB   → cxsdata=0xAB, cxsidx=3
     *   csrr  cxsdata         → val=0xAB,      cxsidx=4  (verify csrw)
     *   csrrw cxsdata, 0xCD   → old=0xAB,      cxsidx=5  (atomic swap)
     *   csrr  cxsdata         → val=0xCD,      cxsidx=6  (verify csrrw)
     *   csrrs cxsdata, 0x10   → old=0xCD→0xDD, cxsidx=7
     *   csrr  cxsdata         → val=0xDD,      cxsidx=8  (verify csrrs)
     *   csrrc cxsdata, 0x0D   → old=0xDD→0xD0, cxsidx=9
     *   csrr  cxsdata         → val=0xD0,      cxsidx=10 (verify csrrc)
     */
    CX_CSR_WRITE(CSR_CXSIDX, 0);

    /* csrw — write only */
    CX_CSR_WRITE(CSR_CXSDATA, 0x00);                       /* cxsidx → 1 */
    CHECK(CX_CSR_READ(CSR_CXSIDX) == 1);

    /* csrr — read only */
    val = CX_CSR_READ(CSR_CXSDATA);                        /* cxsidx → 2 */
    CHECK(val == 0x00);
    CHECK(CX_CSR_READ(CSR_CXSIDX) == 2);

    /* csrw again to set a known value */
    CX_CSR_WRITE(CSR_CXSDATA, 0xAB);                       /* cxsidx → 3 */
    CHECK(CX_CSR_READ(CSR_CXSIDX) == 3);
    val = CX_CSR_READ(CSR_CXSDATA);                        /* cxsidx → 4 */
    CHECK(val == 0xAB);

    /* csrrw — atomic swap; returns old value; increments exactly once */
    val = CX_CSR_SWAP(CSR_CXSDATA, 0xCD);                  /* cxsidx → 5 */
    CHECK(val == 0xAB);
    CHECK(CX_CSR_READ(CSR_CXSIDX) == 5);
    val = CX_CSR_READ(CSR_CXSDATA);                        /* cxsidx → 6 */
    CHECK(val == 0xCD);

    /* csrrs — set bits; 0xCD | 0x10 = 0xDD; returns old */
    val = CX_CSR_SET_BITS(CSR_CXSDATA, 0x10);              /* cxsidx → 7 */
    CHECK(val == 0xCD);
    CHECK(CX_CSR_READ(CSR_CXSIDX) == 7);
    val = CX_CSR_READ(CSR_CXSDATA);                        /* cxsidx → 8 */
    CHECK(val == 0xDD);

    /* csrrc — clear bits; 0xDD & ~0x0D = 0xD0; returns old */
    val = CX_CSR_CLEAR_BITS(CSR_CXSDATA, 0x0D);            /* cxsidx → 9 */
    CHECK(val == 0xDD);
    CHECK(CX_CSR_READ(CSR_CXSIDX) == 9);
    val = CX_CSR_READ(CSR_CXSDATA);                        /* cxsidx → 10 */
    CHECK(val == 0xD0);

    test_pass();
    return 0;
}

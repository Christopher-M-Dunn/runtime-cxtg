/* Block 1.2 test -- cxsetsel raw instruction encoding
 *
 * Tests all five behavioral cases:
 *   1. Swap from 0: old=0, cxsel=0
 *   2. Swap in 0x0102: old=0, cxsel=0x0102
 *   3. Swap in ~0: old=0x0102, cxsel=~0 (no clamping in stub)
 *   4. rd=x0 discards return but still writes cxsel=0x55
 *   5. Round-trip: swap then swap back, value restored
 *
 * cxsetsel encoding: .insn r 0x73, 4, 0, rd, rs1, x0
 *   MATCH=0x00004073  MASK=0xFFF0707F
 *   opcode=0x73 (SYSTEM) funct3=4 funct7=0 rs2=x0
 */

.section .text
.global main
main:

/* Case 1: cxsetsel a1, a0  (a0=0) -> a1=0, cxsel=0 */
    li      a0, 0
    .insn r 0x73, 4, 0, a1, a0, x0
    bnez    a1, fail                    /* old cxsel must be 0 */
    csrr    t1, 0xCA0                   /* read cxsel (CSR address 0xCA0) */
    bnez    t1, fail                    /* cxsel must still be 0 */

/* Case 2: cxsetsel a1, a0  (a0=0x0102) -> a1=0, cxsel=0x0102 */
    li      a0, 0x0102
    .insn r 0x73, 4, 0, a1, a0, x0
    bnez    a1, fail                    /* old cxsel must be 0 (from case 1) */
    csrr    t1, 0xCA0
    li      t0, 0x0102
    bne     t1, t0, fail               /* cxsel must now be 0x0102 */

/* Case 3: cxsetsel a1, a0  (a0=~0) -> a1=0x0102, cxsel=~0 */
    li      a0, -1
    .insn r 0x73, 4, 0, a1, a0, x0
    li      t0, 0x0102
    bne     a1, t0, fail               /* old cxsel must be 0x0102 */
    csrr    t1, 0xCA0
    li      t0, -1
    bne     t1, t0, fail               /* cxsel must be ~0; no clamping in stub */

/* Case 4: cxsetsel x0, a0  (rd=x0, a0=0x55) -> cxsel=0x55, return discarded */
    li      a0, 0x55
    .insn r 0x73, 4, 0, x0, a0, x0
    csrr    t1, 0xCA0
    li      t0, 0x55
    bne     t1, t0, fail               /* cxsel must be 0x55 */

/* Case 5: round-trip */
    li      a0, 0xABCD
    .insn r 0x73, 4, 0, a1, a0, x0    /* swap in 0xABCD; get back 0x55 */
    li      t0, 0x55
    bne     a1, t0, fail               /* old must be 0x55 */
    .insn r 0x73, 4, 0, a2, a1, x0    /* swap in 0x55; get back 0xABCD */
    li      t0, 0xABCD
    bne     a2, t0, fail               /* must have gotten back 0xABCD */
    csrr    t1, 0xCA0
    li      t0, 0x55
    bne     t1, t0, fail               /* cxsel must be 0x55 again */

    j       pass

fail:
    call    test_fail

pass:
    call    test_pass

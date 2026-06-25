#ifndef UTILS_H
#define UTILS_H

/*
 * CXSETSEL rd, rs1 -- atomically swap rs1 into cxsel, return old in rd.
 *
 * Encoding: SYSTEM opcode 0x73, funct3=4 (100), funct7=0, rs2=x0
 * MATCH=0x00004073  MASK=0xFFF0707F
 *
 * Uses GAS .insn r directive -- no custom assembler required.
 */
#define CXSETSEL(rd, rs1) \
    __asm__ volatile (".insn r 0x73, 4, 0, %0, %1, x0" \
                      : "=r"(rd) : "r"(rs1))

#endif /* UTILS_H */

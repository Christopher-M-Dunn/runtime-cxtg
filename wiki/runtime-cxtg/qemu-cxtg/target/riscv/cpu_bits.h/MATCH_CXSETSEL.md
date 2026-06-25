# target/riscv/cpu_bits.h — MATCH_CXSETSEL / MASK_CXSETSEL

Binary encoding and match/mask constants for the cxsetsel instruction. Used by the decodetree pattern in `insn32.decode` and by any disassembler or decoder that needs to identify a cxsetsel word.

```c
/* cxsetsel instruction encoding:
 * SYSTEM opcode 0x73, funct3=100, funct7=0000000, rs2=x0
 */
#define MATCH_CXSETSEL  0x00004073U
#define MASK_CXSETSEL   0xFFF0707FU
```

`MATCH_CXSETSEL` is the fixed bit pattern: opcode=`1110011` (SYSTEM), funct3=`100`, funct7=`0000000`, rs2=`00000`. `MASK_CXSETSEL` masks those fixed bits; the unmasked bits are rd (bits [11:7]) and rs1 (bits [19:15]), which vary per instruction and are decoded into the `arg_r2` struct by decodetree.
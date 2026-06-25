# disas/riscv.c — SYSTEM opcode case 4 cxsetsel decode

Decode logic guard for cxsetsel within the SYSTEM (funct3=4) opcode block.

```c
        case 4:
            /* cxsetsel: funct7=0, rs2=0; bits[31:20] all zero */
            if (dec->cfg && dec->cfg->ext_zcx &&
                ((inst >> 20) == 0)) {
                op = rv_op_cxsetsel;
                break;
            }
            if (dec->cfg && dec->cfg->ext_zimop) {
                ...
            }
            break;
```

Within the SYSTEM opcode block (funct3=4), cxsetsel is recognized when: the Zcx extension is enabled, and bits [31:20] of the instruction are all zero (which encodes funct7=0, rs2=0). Once matched, the opcode is set to `rv_op_cxsetsel`. If Zcx is not enabled or the bit pattern does not match, decoding falls through to check for other ZIMOP instructions that share the same opcode space.

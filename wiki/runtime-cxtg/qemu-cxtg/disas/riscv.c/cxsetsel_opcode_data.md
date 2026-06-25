# disas/riscv.c — cxsetsel rvi_opcode_data entry

Instruction format and codec definition for cxsetsel in the disassembler's opcode table.

```c
static const rv_opcode_data rvi_opcode_data[] = {
    ...
    { "c.sspopchk", rv_codec_cmop_ss, rv_fmt_rs1, NULL, rv_op_sspopchk,
      rv_op_sspopchk, 0 },
    { "cxsetsel", rv_codec_r, rv_fmt_rd_rs1, NULL, 0, 0, 0 },
};
```

This entry defines how the cxsetsel instruction is displayed by the disassembler: its mnemonic name ("cxsetsel"), the codec it uses (R-type format), the operand format (rd, rs1), and null specialization data. The trailing zeros indicate no special primary or secondary opcodes in this table entry.

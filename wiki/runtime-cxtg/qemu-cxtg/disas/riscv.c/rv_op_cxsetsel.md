*← [Main Index](../../../../index.md)* | *↑ [riscv.c](../riscv.c.md)* | *↗ [view source](../../../../../qemu-cxtg/disas/riscv.c)*

# disas/riscv.c — rv_op_cxsetsel

Opcode enum entry for the cxsetsel instruction in the disassembler's operation code table.

```c
typedef enum {
    ...
    rv_op_c_sspopchk = 955,
    rv_op_cxsetsel = 956,
} rv_op;
```

The `rv_op` enum assigns a unique identifier to each instruction operation supported by the disassembler. This entry registers cxsetsel with opcode value 956, making it available for instruction decoding and disassembly.

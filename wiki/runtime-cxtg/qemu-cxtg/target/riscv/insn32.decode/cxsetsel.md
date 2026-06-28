*← [Main Index](../../../../../index.md)* | *↑ [insn32.decode](../insn32.decode.md)* | *↗ [view source](../../../../../../qemu-cxtg/target/riscv/insn32.decode)*

# target/riscv/insn32.decode — cxsetsel

Decodetree pattern for the cxsetsel instruction. Decodetree generates an `arg_r2` struct (fields: `rs1`, `rd`) and a `trans_cxsetsel(ctx, &arg)` dispatch call from this pattern.

```
# *** Zcx composable extensions ***
cxsetsel  0000000 00000 ..... 100 ..... 1110011  @r2
```

The fixed bits encode: funct7=`0000000`, rs2=`00000` (x0), funct3=`100`, opcode=`1110011` (SYSTEM). The wildcard `rs1` (bits [19:15]) and `rd` (bits [11:7]) are extracted by the `@r2` format into `arg_r2`. The `rs2=00000` constraint distinguishes cxsetsel from the hypervisor load/store instructions that share funct3=`100` with non-zero rs2.
# include/utils.h

Inline assembly macros for CX instructions not supported by the standard toolchain assembler. Included by C test files that exercise cxsetsel directly.

**Works with:**
- [tests/block1_2_cxsetsel_macro.c](../tests/block1_2_cxsetsel_macro.c.md) — the C test that exercises `CXSETSEL`
- [qemu-cxtg/target/riscv/insn32.decode](../qemu-cxtg/target/riscv/insn32.decode.md) — defines the instruction encoding matched by this macro

---

### `CXSETSEL` macro

```c
#define CXSETSEL(rd, rs1) \
    __asm__ volatile (".insn r 0x73, 4, 0, %0, %1, x0" \
                      : "=r"(rd) : "r"(rs1))
```

Emits the CXSETSEL instruction using the GAS `.insn r` directive. The directive takes `(opcode, funct3, funct7, rd, rs1, rs2)` — rs2 is fixed to x0 as part of the encoding. `rd` receives the old value of cxsel; `rs1` supplies the value to write. `volatile` prevents the compiler from reordering or eliding the swap. No custom assembler patch is required; the `.insn r` path works with any GAS that understands RISC-V.

Encoding: SYSTEM opcode `0x73`, funct3=4 (`100`), funct7=0, rs2=x0. `MATCH=0x00004073`, `MASK=0xFFF0707F`.

**`"r"` constraint and immediates:** the `"r"(rs1)` constraint means the compiler materializes any value — variable, immediate, or expression — into a register before the instruction. Passing `0UL` or any constant works without change.

**`volatile` vs `"memory"` clobber:** `volatile` prevents the compiler from moving or eliminating the asm. It does not constitute a full compiler memory barrier — cached memory values may not be flushed before or after the instruction. If a future implementation of cxsetsel gates access to CX-specific memory regions (i.e. cxsel influences what memory the following loads/stores can see), the macro will need a `"memory"` clobber to prevent the compiler from reordering surrounding memory operations across the swap. For the current stub, where cxsel is a plain register with no memory-ordering side effects, `volatile` alone is sufficient.

**`rd=x0` limitation:** the `"=r"(rd)` output constraint allocates any writable register — the compiler never picks x0, since x0 is not writable in GCC's register model. The `rd=x0` case (discard return, write-only) can only be exercised in hand-written assembly (see `tests/block1_2_cxsetsel.S` case 4).

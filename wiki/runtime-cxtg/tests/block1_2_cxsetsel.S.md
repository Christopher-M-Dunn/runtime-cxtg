# tests/block1_2_cxsetsel.S

Bare-metal assembly test for the `cxsetsel` instruction (SYSTEM opcode, funct3=4, funct7=0, rs2=x0). Exercises five behavioral cases for the atomic swap semantics of `cxsetsel` against CSR `cxsel` (0xCA0). Written before the instruction is implemented in QEMU — the test is expected to fail (illegal instruction trap) until QEMU decodes and executes `cxsetsel`.

**Works with:**
- [tests/common/crt0.S](common/crt0.S.md) — provides `_start`, trap handler, `test_pass`, `test_fail`
- [tests/common/link.ld](common/link.ld.md) — places `.text.start` at 0x80000000, provides `_stack_top`

---

### Instruction encoding

```asm
.insn r 0x73, 4, 0, rd, rs1, x0
```

GAS `.insn r` directive encodes the instruction directly from its fields without requiring the assembler to know the mnemonic. Fields: opcode=0x73 (SYSTEM), funct3=4, funct7=0, rs2=x0. Encoding: MATCH=0x00004073, MASK=0xFFF0707F. GAS accepts this regardless of QEMU support, so the test compiles cleanly before the instruction is implemented.

---

### Case 1 — swap from 0

```asm
    li      a0, 0
    .insn r 0x73, 4, 0, a1, a0, x0
    bnez    a1, fail
    csrr    t1, 0xCA0
    bnez    t1, fail
```

Swaps 0 into `cxsel`. Verifies that the returned old value (`a1`) is 0 (reset state) and that `cxsel` reads back 0.

---

### Case 2 — swap in non-zero value

```asm
    li      a0, 0x0102
    .insn r 0x73, 4, 0, a1, a0, x0
    bnez    a1, fail
    csrr    t1, 0xCA0
    li      t0, 0x0102
    bne     t1, t0, fail
```

Swaps 0x0102 into `cxsel`. Verifies old value returned is 0 (from case 1) and `cxsel` now reads 0x0102.

---

### Case 3 — swap in all-ones (no clamping)

```asm
    li      a0, -1
    .insn r 0x73, 4, 0, a1, a0, x0
    li      t0, 0x0102
    bne     a1, t0, fail
    csrr    t1, 0xCA0
    li      t0, -1
    bne     t1, t0, fail
```

Swaps ~0 into `cxsel`. Verifies old value is 0x0102 (from case 2) and `cxsel` holds ~0. The stub does not clamp — this tests that behavior explicitly.

---

### Case 4 — rd=x0 discards return, write still occurs

```asm
    li      a0, 0x55
    .insn r 0x73, 4, 0, x0, a0, x0
    csrr    t1, 0xCA0
    li      t0, 0x55
    bne     t1, t0, fail
```

Uses `x0` as the destination register so the returned old value is discarded. Verifies that `cxsel` is still updated to 0x55 — the write side of the swap must occur even when rd=x0.

---

### Case 5 — round-trip restores original value

```asm
    li      a0, 0xABCD
    .insn r 0x73, 4, 0, a1, a0, x0    /* swap in 0xABCD; get back 0x55 */
    li      t0, 0x55
    bne     a1, t0, fail
    .insn r 0x73, 4, 0, a2, a1, x0    /* swap in 0x55; get back 0xABCD */
    li      t0, 0xABCD
    bne     a2, t0, fail
    csrr    t1, 0xCA0
    li      t0, 0x55
    bne     t1, t0, fail
```

Swaps 0xABCD in, capturing the old value (0x55) in `a1`. Then swaps `a1` back in, recovering 0xABCD in `a2`. Confirms the full swap chain is self-consistent and `cxsel` ends at 0x55.

---

### Exit points

```asm
fail:
    call    test_fail

pass:
    call    test_pass
```

All failure branches jump to `fail`, which calls `test_fail` (prints `FAILED\n` over UART and spins). The success path falls through to `pass`, which calls `test_pass` (prints `PASSED\n` and spins). Both are provided by `crt0.S`.

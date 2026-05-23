# Phase 1 Design — Unpriv CSR and Instruction Scaffolding

**Date:** 2026-05-22
**Phase:** 1 — Unpriv CSR and Instruction Scaffolding
**Blocks:** 1.1, 1.2, 1.3 + test-infra
**Status:** Approved

---

## 1. Decisions Made

### Extension name
`zicx` / `ext_zicx` is confirmed renamed to `zcx` / `ext_zcx` throughout all files.

### CSR addresses
Placeholders updated from Artur's values to current working values. These are ~permanent
TODO entries — do not treat as stable until spec finalisation.

| CSR | Address | Access | Notes |
|-----|---------|--------|-------|
| `cxsel` | `0xC20` | URO | Standard user read-only range; csr[11:10]=11 enforces write trap in hardware |
| `cxsidx` | `0x018` | URW | Standard user read-write range |
| `cxsdata` | `0x019` | URW | Standard user read-write range |

Reference: RISC-V Privileged Spec v20250508 §2.1 Table 3.

### cxsel write behavior
cxsel is URO — direct `csrw cxsel` traps with mcause=2. The WARL nature of cxsel is
exposed exclusively through the `cxsetsel` instruction (atomic swap). Reference:
Composable-Extensions.md.

### cxsel read behavior on non-existent CSR
RISC-V Privileged Spec §2.1: "Instructions that access a non-existent CSR are reserved."
QEMU implements this as illegal-instruction via the predicate mechanism — not silently 0.
When `ext_zcx=false`, the predicate returns `RISCV_EXCP_ILLEGAL_INST` before the
read handler is called.

### cxsdata auto-increment
Spec (Composable-Extensions.md) is unambiguous: following every cxsdata access (read or
write), cxsidx is incremented by 1. After the last valid word, cxsidx is undefined.
Auto-increment is implemented in Block 1.1 (one line in each of the read and write
handlers in cx.c). WARL range enforcement (0 to state_size-1) is deferred to Block 4.2
when state_size is wired.

### cxsidx WARL
All writes accepted in Phase 1 — no range enforcement. Deferred to Block 4.2.
Added to todo.md as a permanent deferred item.

### cxsetsel encoding
SYSTEM opcode (0x73), funct3=0b100 (4) — reserved for custom use per spec group.
funct7 = 0x0, rs2 is reserved (x0) - placeholder until end of project, like 
CSR addresses (~permanent todo).

```
MATCH_CXSETSEL = 0x00004073
MASK_CXSETSEL  = 0xFE007FFF   /* masks funct7+rs2+funct3+opcode; rd and rs1 are operands */
```

### cx_sel_t type
Union of named bitfield struct (`.sel`) and raw integer (`.raw`). The `.raw` member is
used in inline asm. The `.sel` member provides named access to CXID and SID fields. The
union will have to be expanded when indirect mode is introduced.
No magic numbers — all bit positions and widths are named constants. Subject to change.

**Departure from spec naming:** The spec (Figure 3) calls the [15:8] field "IDX". This
implementation uses "SID" (State ID) for clarity — it is the index into a CX's state
contexts, not a generic array index. TODO: align with TG/spec terminology once naming is
finalised.

Direct mode cxsel layout (Figure 3 of spec):

```
[XLEN-1:16] reserved | [15:8] SID (spec: IDX) | [7:0] CXID
```

Constants (defined in both `include/ccex.h` runtime side and `cpu_bits.h` QEMU side):

```c
#define CXSEL_CXID_POSITION  0
#define CXSEL_CXID_BITS      8
#define CXSEL_SID_POSITION   8
#define CXSEL_SID_BITS       8
#define CXSEL_RSVD_POSITION  16
#define CXSEL_RSVD_BITS      (sizeof(uintptr_t) * 8 - CXSEL_RSVD_POSITION)
```

### cxsel clamping behavior (implementation-defined WARL)

Two clamping rules applied by the `cxsetsel` translate handler in QEMU before writing
to `env->cxsel`:

1. **CXID=0 (legacy): SID is reserved — clamped to 0.**
   When the CXID field is 0 (builtin/legacy extension), the SID field has no meaning.
   Any non-zero SID is masked out. The stored value will have SID=0.

2. **Negative values (sign bit set) clamped to CX_SEL_INVALID (~0UL).**
   Any value that, when interpreted as a signed integer, is negative — i.e., has the
   MSB set — is treated as invalid and clamped to ~0UL (CX_SEL_INVALID).

Combined pseudocode:
```c
if (CXID(val) == 0) {
    val &= ~CXSEL_SID_MASK;          /* clear SID for legacy */
}
if ((intptr_t)val < 0) {
    val = ~0UL;                      /* clamp to invalid; subsumes ~0UL itself */
}
env->cxsel = val;
```

These rules are also documented in `spec.md § Architecture`.

### Toolchain
`riscv64-unknown-elf-gcc` installed system-wide. Makefile uses
`CROSS_COMPILE ?= riscv64-unknown-elf-` (overridable). Dependency documented in
`README.md`. No binaries committed to the repo.

### Code style
All runtime-cxtg files follow QEMU style: C-style `/* */` comments only, 4-space
indent, 80-char line limit, braces on own line for function definitions, always-braces,
`lower_case_with_underscores`.

---

## 2. Branch Structure

Four branches in order — each merges to `cxtg-dev` before the next begins:

| Branch | Repo | Produces |
|--------|------|----------|
| `feat/test-infra` | `runtime-cxtg` | Makefile, `tests/common/`, `README.md` |
| `feat/1.1` | `qemu-cxtg` | rename, predicate fix, address update, auto-increment, tests |
| `feat/1.2` | `qemu-cxtg` + `runtime-cxtg/include/utils.h` | cxsetsel instruction + macro |
| `feat/1.3` | `runtime-cxtg` | `include/ccex.h`, `src/ccex.c`, `examples/test_wrappers.c` |

Phase 1 milestone tag `cxtg-v0.phase1` applied to both repos after all four merge.

---

## 3. Block Designs

### feat/test-infra — Bare-metal test harness

**Files created:**
```
runtime-cxtg/
  README.md
  Makefile
  tests/
    common/
      crt0.S       — M-mode startup: set sp, install mtvec trap handler, zero .bss, call main
      uart.h       — QEMU virt UART (0x10000000) putchar/puts/print_hex
      test.h       — PASS/FAIL macros, CHECK(cond) helper, trap mcause recorder
      link.ld      — bare-metal linker script (DRAM base 0x80000000)
```

**Test execution flow:**
1. QEMU loads ELF at `0x80000000`
2. `crt0.S`: set `sp`, install trap handler into `mtvec`, zero `.bss`, call `main`
3. `main` exercises feature; `CHECK(cond)` asserts expected values
4. Expected traps: trap handler records `mcause` in a global; test asserts mcause=2
5. `main` calls `test_pass()` or `test_fail()` — prints `PASSED\n` or `FAILED\n` over UART, spins

**QEMU invocation:**
```
qemu-system-riscv64 -M virt -bios <test.elf> -nographic -no-reboot
```

**Makefile targets:**
- `make tests/block1_1_csr_stubs.elf` — compile one test
- `make run-block1_1_csr_stubs` — compile + run under QEMU
- `make all-tests` — compile all tests
- `make clean`

---

### feat/1.1 — CSR stubs

**Changes in `qemu-cxtg`:**

1. **Rename `zicx` → `zcx`** (grep-and-replace):
   - `ext_zicx` → `ext_zcx`: `cpu.c`, `cpu_cfg_fields.h.inc`, `csr.c`, `cpu_helper.c`
   - `"zicx"` → `"zcx"` strings: `cpu.c` (`ISA_EXT_DATA_ENTRY`, `MULTI_EXT_CFG_BOOL`)
   - `KVM_RISCV_ISA_EXT_ZICX` → `KVM_RISCV_ISA_EXT_ZCX`: `linux-headers/asm-riscv/kvm.h`, `kvm/kvm-cpu.c`
   - Disassembler entries: `disas/riscv.c`

2. **Update CSR addresses** in `cpu_bits.h` — replace 0x800/0x801/0x802 with
   0xC20/0x018/0x019; update TODO comment to reflect new values and ~permanent status.
   Update `CXSEL_*` constants to match new layout and SID naming:
   ```c
   #define CXSEL_CXID_POSITION  0
   #define CXSEL_CXID_BITS      8
   #define CXSEL_SID_POSITION   8
   #define CXSEL_SID_BITS       8
   ```

3. **Rename `CPURISCVState` fields**: `cxidx` → `cxsidx`, `cxdata` → `cxsdata`
   in `cpu.h`, `cx.c`, `csr.c`, `cpu.c`.

4. **Fix predicate functions** in `csr.c` — remove dead PMP copy-paste from all three
   predicates (`cxsel`, `cxsidx`, `cxsdata`). Clean pattern:
   ```c
   static RISCVException cxsel(CPURISCVState *env, int csrno)
   {
       if (!riscv_cpu_cfg(env)->ext_zcx) {
           return RISCV_EXCP_ILLEGAL_INST;
       }
       return RISCV_EXCP_NONE;
   }
   ```
   The predicate is named after the CSR (QEMU convention). It is called before every CSR
   read or write. Returning RISCV_EXCP_ILLEGAL_INST raises an illegal instruction exception
   without calling the handler — this is how QEMU models a CSR that doesn't exist when an
   extension is off.

5. **Implement cxsdata auto-increment** in `cx.c` — add `env->cxsidx++` to both
   `cxsdata_csr_read` and `cxsdata_csr_write`. Natural unsigned wrap on overflow is
   correct for the stub (spec says cxsidx is undefined after the last valid word).

6. **Update `todo.md`**:
   - CSR addresses updated to 0xC20/0x018/0x019; ~permanent pending spec finalisation
   - cxsetsel opcode likewise, also ~permanent
   - `cxsidx` WARL range (0 to state_size-1) not enforced — deferred to Block 4.2
   - Remove "zicx → zcx rename" entry (done)

**Test:** `tests/block1_1_csr_stubs.S`
- `csrr cxsel` → 0 after reset, no trap
- `csrw cxsel, N` → mcause=2 (values: 0, 1, 0xFF, ~0UL)
- `csrw cxsidx, N` / `csrr cxsidx` → N round-trip (values: 0, 1, 7, 15, 255)
- `csrw cxsdata, V` / `csrr cxsdata` → V round-trip (values: 0, 0xDEADBEEF, ~0UL)
- cxsdata auto-increment: set cxsidx=0; write cxsdata → assert cxsidx reads back as 1;
  read cxsdata → assert cxsidx reads back as 2

---

### feat/1.2 — cxsetsel instruction

**Changes in `qemu-cxtg`:**

1. `cpu_bits.h` — add encoding constants:
   ```c
   /* TODO: cxsetsel funct7/rs2 pending spec assignment — SYSTEM opcode, funct3=4 */
   #define MATCH_CXSETSEL  0x00004073
   #define MASK_CXSETSEL   0xFE007FFF
   ```

2. `target/riscv/insn32.decode` — add `cxsetsel rd rs1` entry.

3. `target/riscv/translate.c` — handler: apply clamping rules (Section 1 — cxsel clamping
   behavior) to `rs1` value, read old `env->cxsel` into `rd`, write clamped value into
   `env->cxsel`. Edge cases:
   - `rd=x0`: write still occurs; return value discarded (standard RISC-V convention)
   - `rs1=x0`: writes 0 to `cxsel` (selects builtin; CXID=0, SID clamped to 0)

**Changes in `runtime-cxtg`:**

`include/utils.h` (new):
```c
/* TODO: cxsetsel encoding pending spec assignment — SYSTEM opcode, funct3=4 */
#define CXSETSEL(rd, rs1) \
    __asm__ volatile(".insn r 0x73, 0x4, 0x0, %0, %1, x0" \
                     : "=r"(rd) : "r"(rs1))
```

**Test:** `tests/block1_2_cxsetsel.S`
- `cxsetsel a1, a0` with a0=0 → a1=0 (reset value), `csrr cxsel`=0
- `cxsetsel a1, a0` with a0=0x0102 → a1=0, `csrr cxsel`=0x0102
- `cxsetsel a1, a0` with a0=~0UL → `csrr cxsel`=~0UL (CX_SEL_INVALID passes through)
- `cxsetsel x0, a0` with a0=0x55 → `csrr cxsel`=0x55 (rd=x0 discards return only)
- Back-to-back round-trip: values swap correctly
- **Clamping — CXID=0 legacy, SID cleared:**
  `cxsetsel a1, a0` with a0=0x0100 (CXID=0, SID=1) → `csrr cxsel`=0x0000 (SID clamped to 0)
  `cxsetsel a1, a0` with a0=0xFF00 (CXID=0, SID=0xFF) → `csrr cxsel`=0x0000
- **Clamping — negative (sign bit set) → CX_SEL_INVALID:**
  `cxsetsel a1, a0` with a0=0x8000000000000000 → `csrr cxsel`=~0UL
  `cxsetsel a1, a0` with a0=0x8000000000000001 → `csrr cxsel`=~0UL
  `cxsetsel a1, a0` with a0=~0UL → `csrr cxsel`=~0UL (already invalid, passes through)

---

### feat/1.3 — Runtime wrappers

**Files created in `runtime-cxtg`:**

`include/ccex.h`:
```c
#ifndef CCEX_H
#define CCEX_H

#include "utils.h"
#include <stdint.h>

#define CXSEL_CXID_POSITION  0
#define CXSEL_CXID_BITS      8
#define CXSEL_SID_POSITION   8   /* SID = State ID; spec calls this field IDX */
#define CXSEL_SID_BITS       8
#define CXSEL_RSVD_POSITION  16
#define CXSEL_RSVD_BITS      (sizeof(uintptr_t) * 8 - CXSEL_RSVD_POSITION)

typedef union {
    struct {
        uintptr_t cxid : CXSEL_CXID_BITS;
        uintptr_t sid  : CXSEL_SID_BITS;
        uintptr_t rsvd : CXSEL_RSVD_BITS;
    } sel;
    uintptr_t raw;
} cx_sel_t;

#define CX_LEGACY      ((cx_sel_t){ .raw = 0 })
#define CX_SEL_INVALID ((cx_sel_t){ .raw = ~0UL })

cx_sel_t cx_select(cx_sel_t sel);
cx_sel_t cx_get_sel(void);
int      cx_valid(cx_sel_t sel);

#endif /* CCEX_H */
```

`src/ccex.c`:
```c
#include "ccex.h"

cx_sel_t cx_select(cx_sel_t sel)
{
    cx_sel_t prev;
    CXSETSEL(prev.raw, sel.raw);
    return prev;
}

cx_sel_t cx_get_sel(void)
{
    cx_sel_t val;
    /* TODO: use named CSR once address finalised */
    __asm__ volatile("csrr %0, 0xC20" : "=r"(val.raw));
    return val;
}

int cx_valid(cx_sel_t sel)
{
    return sel.raw != CX_SEL_INVALID.raw;
}
```

**Test:** `examples/test_wrappers.c` — covers all action plan cases:
`cx_select` round-trip, `cx_get_sel`, `cx_valid(CX_LEGACY)==1`,
`cx_valid(CX_SEL_INVALID)==0`.

---

## 4. Wiki updates required

Each block must update wiki pages for every file it touches before merging.
New files require a new page at `wiki/runtime-cxtg/<path>.md` and an entry in `wiki/index.md`.

**Before writing any page:** read the four format examples at the top of `wiki/index.md` and open each linked example. Choose the format that matches the file's nature.

| Block | New wiki pages | Updated wiki pages |
|-------|---------------|-------------------|
| `feat/test-infra` | `README.md`, `Makefile`, `tests/common/crt0.S`, `uart.h`, `test.h`, `link.ld` | — |
| `feat/1.1` | — | `cpu_bits.h`, `cpu.h`, `cpu.c`, `csr.c`, `cx.c`, `cpu_cfg_fields.h.inc`, `kvm-cpu.c`, `kvm.h`, `disas/riscv.c` |
| `feat/1.2` | `utils.h` | `insn32.decode`, `translate.c`, `cpu_bits.h` |
| `feat/1.3` | `include/ccex.h`, `src/ccex.c` | `utils.h` |

---

## 5. todo.md changes

After Phase 1:
- CSR addresses: update stubs to 0xC20/0x018/0x019; mark ~permanent
- cxsetsel encoding: funct3=4 confirmed; funct7/rs2 TBD (~permanent)
- cxsidx WARL range: deferred to Block 4.2
- SID vs IDX naming: this implementation uses SID (State ID); spec uses IDX — align with
  TG/spec once naming is finalised (~permanent until spec stabilises)
- Remove: "zicx → zcx rename" (completed in Block 1.1)

---

## 6. References

- RISC-V Privileged Spec v20250508 §2.1 — CSR address ranges, non-existent CSR behavior
- `docs/Composable-Extensions.md` — cxsel URO, cxsetsel semantics, cxsdata auto-increment
- `docs/phase0_audit.md` — baseline CSR inventory, cxsetsel gap, field name discrepancies
- `docs/todo.md` — deferred decisions carried forward
- `~/Github/cx_runtime_v1/include/utils.h` — Brandon's union pattern for cx_sel_t
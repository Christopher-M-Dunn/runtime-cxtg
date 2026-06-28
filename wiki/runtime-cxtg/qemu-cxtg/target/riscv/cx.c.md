---
format: A
---

# qemu-cxtg/target/riscv/cx.c

CX extension implementation. Provides the CSR read/write handlers for cxsel, cxsidx, and cxsdata, the TCG helper for the cxsetsel instruction, and QEMU trace events on every access.

**Works with:**
- [cx.h](cx.h.md) — declares the CSR handler prototypes used by csr.c
- [helper.h](helper.h.md) — `DEF_HELPER_2(cxsetsel, ...)` generates the TCG-facing wrapper; `exec/helper-proto.h` expands it into the C prototype used by this file
- [trace-events](trace-events.md) — defines `trace_cxsel_csr_read`, `trace_cxsidx_csr_read/write`, `trace_cxsdata_csr_read/write`, `trace_cxsetsel`
- [csr.c](csr.c.md) — registers the CSR handlers in `csr_ops[]`
- [insn_trans/trans_rvzcx.c.inc](insn_trans/trans_rvzcx.c.inc.md) — calls `gen_helper_cxsetsel()`, which invokes `helper_cxsetsel` at runtime

---

### Includes

```c
#include "qemu/osdep.h"
#include "cpu.h"
#include "exec/helper-proto.h"
#include "trace.h"
```

`exec/helper-proto.h` expands the `DEF_HELPER_*` macros from `helper.h` into C function prototypes, making `helper_cxsetsel` visible before its definition. This follows the same pattern as `op_helper.c` and other QEMU TCG helper files.

---

### `cxsel_csr_read` / `cxsel_csr_write`

```c
void cxsel_csr_read(CPURISCVState *env, uint32_t reg_index, target_ulong *val)
{
    *val = env->cxsel;
    trace_cxsel_csr_read(env->mhartid, reg_index, *val);
}

void cxsel_csr_write(CPURISCVState *env, uint32_t reg_index, target_ulong val)
{
    /* cxsel is URO — direct writes trap via hardware encoding; */
    trace_cxsel_csr_write(env->mhartid, reg_index, val);
}
```

CSR table handlers for `cxsel`. The write handler is a no-op stub — `cxsel` is URO (user read-only); the address encoding prevents writes from reaching this handler in practice.

---

### `cxsidx_csr_read` / `cxsidx_csr_write`

```c
void cxsidx_csr_read(CPURISCVState *env, uint32_t reg_index, target_ulong *val)
{
    *val = env->cxsidx;
    trace_cxsidx_csr_read(env->mhartid, reg_index, *val);
}

void cxsidx_csr_write(CPURISCVState *env, uint32_t reg_index, target_ulong val)
{
    env->cxsidx = val;
    trace_cxsidx_csr_write(env->mhartid, reg_index, val);
}
```

CSR table handlers for `cxsidx`. Reads and writes `env->cxsidx` directly.

---

### `cxsdata_csr_op`

```c
RISCVException cxsdata_csr_op(CPURISCVState *env, int csrno,
                               target_ulong *ret_value,
                               target_ulong new_value, target_ulong write_mask)
{
    target_ulong old = env->cxsdata;

    if (ret_value) {
        *ret_value = old;
        trace_cxsdata_csr_read(env->mhartid, csrno, old);
    }
    if (write_mask) {
        env->cxsdata = (old & ~write_mask) | (new_value & write_mask);
        trace_cxsdata_csr_write(env->mhartid, csrno, env->cxsdata);
    }
    env->cxsidx++;
    return RISCV_EXCP_NONE;
}
```

Combined atomic handler registered as `csr_ops[].op` for all cxsdata CSR variants. Using `.op` instead of separate `.read`/`.write` ensures `cxsidx` increments exactly once per instruction regardless of variant (csrr, csrw, csrrw, csrrs, csrrc).

`write_mask` encodes the variant: `0` = read-only, `-1` = full replace (csrrw), any other value = partial write (set/clear bits).

---

### `helper_cxsetsel`

```c
target_ulong helper_cxsetsel(CPURISCVState *env, target_ulong new_val)
{
    target_ulong old = env->cxsel;
    env->cxsel = new_val;
    trace_cxsetsel(env->mhartid, new_val, old);
    return old;
}
```

TCG helper for the `cxsetsel` instruction. Reads `env->cxsel`, writes `new_val` into it, emits `trace_cxsetsel`, and returns the old value. No WARL clamping — `env->cxsel` accepts any XLEN-wide value in this stub; validation is deferred to block 2.3.

Called via `gen_helper_cxsetsel()` in `trans_cxsetsel`, which is auto-generated from `DEF_HELPER_2(cxsetsel, tl, env, tl)` in `helper.h`.

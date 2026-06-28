---
format: A
---

*← [Main Index](../../../../../index.md)*

# qemu-cxtg/target/riscv/insn_trans/trans_rvzcx.c.inc

TCG translation routines for the Zcx (Composable Extensions) instruction set. Included into `translate.c` as part of the same translation unit.

**Works with:**
- [insn32.decode](../insn32.decode.md) — defines the `cxsetsel` decode pattern with `@r2` format, generating `arg_r2 { rs1, rd }`
- [helper.h](../helper.h.md) — `DEF_HELPER_2(cxsetsel, tl, env, tl)` causes the build system to auto-generate `gen_helper_cxsetsel()`
- [translate.c](../translate.c.md) — provides `get_gpr()`, `dest_gpr()`, `gen_set_gpr()`, `translator_io_start()`, `do_csr_post()`; includes this file

---

### `trans_cxsetsel` — translate cxsetsel instruction

```c
static bool trans_cxsetsel(DisasContext *ctx, arg_r2 *a)
{
    TCGv src, dest;

    if (!ctx->cfg_ptr->ext_zcx) {
        return false;
    }
    src  = get_gpr(ctx, a->rs1, EXT_NONE);
    dest = dest_gpr(ctx, a->rd);
    translator_io_start(&ctx->base);
    gen_helper_cxsetsel(dest, tcg_env, src);
    gen_set_gpr(ctx, a->rd, dest);
    return do_csr_post(ctx);
}
```

Returns `false` when `ext_zcx` is not set, causing the decoder to treat the instruction as illegal. When the extension is enabled, reads `rs1` into a TCG temporary, calls `gen_helper_cxsetsel` to emit the runtime swap of `env->cxsel`, and writes the old value back to `rd`. `translator_io_start` marks the TB as containing I/O operations, preventing speculative re-execution. `do_csr_post` saves the opcode for exception unwind, advances the PC, and forces a TB exit — required because `env->cxsel` determines how subsequent CX instructions decode and execute.

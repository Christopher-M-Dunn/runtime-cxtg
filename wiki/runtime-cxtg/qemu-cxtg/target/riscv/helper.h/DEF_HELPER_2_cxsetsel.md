# qemu-cxtg/target/riscv/helper.h — DEF_HELPER_2(cxsetsel, tl, env, tl)

Declares the `cxsetsel` TCG helper. The macro system generates a C prototype and the `gen_helper_cxsetsel` TCG wrapper used in translation code.

```c
/* Special functions */
DEF_HELPER_2(csrr, tl, env, int)
DEF_HELPER_3(csrw, void, env, int, tl)
DEF_HELPER_4(csrrw, tl, env, int, tl, tl)
DEF_HELPER_2(csrr_i128, tl, env, int)
DEF_HELPER_4(csrw_i128, void, env, int, tl, tl)
DEF_HELPER_6(csrrw_i128, tl, env, int, tl, tl, tl, tl)
DEF_HELPER_2(cxsetsel, tl, env, tl)
```

The `tl` type maps to `target_ulong` (XLEN-wide). The two-argument form (`DEF_HELPER_2`) matches the function signature: one `env` pointer plus one `tl` value argument. The generated prototype is `target_ulong helper_cxsetsel(CPURISCVState *env, target_ulong new_val)`; the generated TCG wrapper is `gen_helper_cxsetsel(TCGv dest, TCGv_ptr env, TCGv src)`, used in `trans_rvzcx.c.inc` to emit the helper call during instruction translation.

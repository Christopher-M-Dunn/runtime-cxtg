# qemu-cxtg/target/riscv/translate.c — trans_rvzcx.c.inc include

Pulls the Zcx translation routines into the `translate.c` translation unit so that `trans_cxsetsel` and the helper call it emits share the same static scope as `do_csr_post`, `get_gpr`, `dest_gpr`, and `gen_set_gpr`.

```c
#include "insn_trans/trans_rvzimop.c.inc"
#include "insn_trans/trans_rvzcx.c.inc"        /* Zcx composable extensions */
```

Placed immediately after `trans_rvzimop.c.inc` in the Z-extension include block. `do_csr_post` is defined as `static` in `trans_rvi.c.inc`, which is included earlier in the same block; placing `trans_rvzcx.c.inc` anywhere in this section makes it visible without requiring a forward declaration.

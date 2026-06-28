# Wiki Fix Conformance Log

Issues found during Tasks 4–7. All are fixed inline during the pass that finds them.

---

## Task 5 — Format B pass

[wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu_cfg_fields.h.inc.md] — stale link filename: bullet linked to `ext_zicx.md` but actual sub-page file is `ext_zcx.md`. Fixed by updating the link.

[wiki/runtime-cxtg/qemu-cxtg/target/riscv/cpu.h/cxsel.md] — comment used as code context: `/* in CPURISCVState (CPUArchState) */` substituted for the struct definition. Prohibited by schema sub-page rule. Fixed by replacing with actual `typedef struct CPURISCVState { ... target_ulong cxsel; ... }` snippet.

[wiki/runtime-cxtg/qemu-cxtg/target/riscv/kvm/kvm-cpu.c.md] — debug printf functions (`kvm_riscv_update_cpu_cfg_isa_ext`, `kvm_riscv_add_cpu_user_properties`) listed without sub-page links. Format B requires all changed symbols to link to sub-pages; these are minor debug-only changes with no sub-pages. Logged; not fixed (creating sub-pages for two-line debug printfs has low value).

Pattern observed across multiple sub-pages: titles use label prefixes ("function:", "trace event:", "build rule:", "option handler:") that are not part of the schema's "One sentence" format. These are consistent and informative; no fix applied.

---

## Task 4 — Format A pass

[wiki/runtime-cxtg/qemu-cxtg/target/riscv/cx.c.md] — Missing H1 title; body text began immediately after nav line with no `# path/to/file` heading. Fixed by inserting `# qemu-cxtg/target/riscv/cx.c`.

[wiki/runtime-cxtg/tests/common/crt0.S.md] — 148 raw lines (> 125 threshold); converted from Format A to Format C. Added `## Index` with URL-encoded heading links and `*↑ [back to top](#Index)*` at end of each `###` section.

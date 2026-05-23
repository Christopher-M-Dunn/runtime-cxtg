# README.md

Project overview page for `runtime-cxtg`. Intended audience: a developer arriving at the repo for the first time who needs to build and run a test.

**Works with:**
- [Makefile](Makefile.md) — the build system described by the README
- [tests/common/crt0.S](tests/common/crt0.S.md) — the test infrastructure the README introduces

---

## What it covers

- **Dependencies** — `riscv64-unknown-elf-gcc` (must be on PATH) and the in-tree `qemu-cxtg` build. Both can be overridden on the `make` command line (`CROSS_COMPILE=`, `QEMU=`).
- **Build and run commands** — the four `make` targets a developer needs: single ELF, run under QEMU, all tests, clean.
- **Test infrastructure table** — one-line descriptions of the four `tests/common/` files.
- **Repository layout** — top-level directory map (`qemu-cxtg/`, `linux-cxtg/`, `include/`, `src/`, `tests/`, `examples/`, `docs/`).

## What it omits (intentionally)

The README does not explain the CX extension architecture, the QEMU implementation, or the phase structure — that context lives in `docs/CXTG_QEMU_Action_Plan.md` and the wiki.
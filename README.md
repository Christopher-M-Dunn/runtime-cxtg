# runtime-cxtg

Bare-metal test harness and runtime wrappers for the RISC-V Composable Extensions
(Zcx/ZcxMulti) QEMU implementation.

## Dependencies

- `riscv64-unknown-elf-gcc` — RISC-V bare-metal toolchain (must be on PATH)
- `qemu-cxtg` built at `qemu-cxtg/build/qemu-system-riscv64`
  (see `qemu-cxtg/CLAUDE.md` for build instructions)

Override the toolchain prefix:

    make CROSS_COMPILE=my-riscv-toolchain- run-block1_1_csr_stubs

Override the QEMU binary:

    make QEMU=/path/to/qemu-system-riscv64 run-block1_1_csr_stubs

## Building and running tests

Compile a single test ELF:

    make tests/block1_1_csr_stubs.elf

Compile and run under QEMU:

    make run-block1_1_csr_stubs

Compile all tests:

    make all-tests

Clean build artefacts:

    make clean

## Test infrastructure

| File | Purpose |
|------|---------|
| `tests/common/crt0.S` | M-mode startup: stack, mtvec, .bss zero, call main |
| `tests/common/uart.h` | QEMU virt UART (0x10000000) putchar/puts/print_hex |
| `tests/common/test.h` | CHECK macro, test_pass/test_fail declarations |
| `tests/common/link.ld` | Bare-metal linker script (DRAM base 0x80000000) |

## Repository layout

| Path | Purpose |
|------|---------|
| `qemu-cxtg/` | QEMU fork implementing the Zcx/ZcxMulti hardware model |
| `linux-cxtg/` | Kernel for S-mode testing (future work) |
| `include/` | Runtime library headers |
| `src/` | Runtime library sources |
| `tests/` | Bare-metal assembly and C test programs |
| `examples/` | C example programs using the runtime API |
| `docs/` | Project documentation and implementation plan |
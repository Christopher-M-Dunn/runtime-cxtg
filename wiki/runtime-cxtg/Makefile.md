# Makefile

Top-level build system for bare-metal test ELFs and QEMU test runs.

**Works with:**
- [tests/common/crt0.S](tests/common/crt0.S.md) — linked into every test ELF as the startup object
- [tests/common/link.ld](tests/common/link.ld.md) — passed as `-T` linker script to every compilation
- `qemu-cxtg/build/qemu-system-riscv64` — QEMU binary invoked by `run-%` targets

---

## Index

- [Jump to full text walkthrough](#Full%20text%20walkthrough)

**Variables**
- [`CROSS_COMPILE`, `CC`](#Toolchain%20variables) — toolchain prefix, default `riscv64-unknown-elf-`
- [`QEMU`, `QEMU_FLAGS`](#QEMU%20variables) — QEMU binary path and invocation flags
- [`COMMON`, `LDSCRIPT`, `CRT0`](#Path%20variables) — convenience paths used in pattern rules
- [`CFLAGS`](#CFLAGS) — march, ABI, bare-metal flags, debug

**Source discovery**
- [`TEST_SRCS_S`, `TEST_SRCS_C`, `TEST_ELFS`](#Test%20discovery) — wildcard discovery of test sources

**Pattern rules**
- [`tests/%.elf: tests/%.S`](#Assembly%20test%20pattern%20rule) — compile assembly test against crt0.S
- [`tests/%.elf: tests/%.c`](#C%20test%20pattern%20rule) — compile C test against crt0.S

**Targets**
- [`run-%`](#run-%25%20target) — compile named test ELF then run under QEMU
- [`all-tests`](#all-tests%20and%20clean) — build all discovered test ELFs
- [`clean`](#all-tests%20and%20clean) — remove compiled ELFs

---

## Full text walkthrough

### Toolchain variables

```makefile
CROSS_COMPILE ?= riscv64-unknown-elf-
CC            := $(CROSS_COMPILE)gcc
```

`CROSS_COMPILE` defaults to the system-installed bare-metal toolchain. The `?=` assignment allows override on the command line: `make CROSS_COMPILE=my-prefix- run-block1_1_csr_stubs`. Only `gcc` is needed — it drives assembly, compilation, and linking in one pass.

---

### QEMU variables

```makefile
QEMU       ?= qemu-cxtg/build/qemu-system-riscv64
QEMU_FLAGS := -M virt -nographic -no-reboot
```

`QEMU` defaults to the in-tree build. Override: `make QEMU=/path/to/qemu-system-riscv64 run-...`. `-nographic` redirects UART output to stdout. `-no-reboot` causes QEMU to exit when the guest issues a reset — the test spin loops keep QEMU alive until the terminal output is observed and the process is killed.

---

### Path variables

```makefile
COMMON   := tests/common
LDSCRIPT := $(COMMON)/link.ld
CRT0     := $(COMMON)/crt0.S
```

Convenience variables used in every pattern rule. Centralise the common paths so changing the test harness location only requires updating these three lines.

---

### CFLAGS

```makefile
CFLAGS := -march=rv64imac_zicsr -mabi=lp64
CFLAGS += -nostdlib -nostartfiles -ffreestanding -fno-builtin
CFLAGS += -g -O0
```

`rv64imac_zicsr` — base ISA plus `Zicsr`, which is required for `csrr`/`csrw` in both `crt0.S` and the assembly tests. `-nostdlib -nostartfiles` suppress stdlib and the toolchain's default `crt0`. `-fno-builtin` prevents the compiler assuming stdlib semantics for functions named `puts`, `memset`, etc. `-O0 -g` keeps code predictable and debuggable.

---

### Test discovery

```makefile
TEST_SRCS_S := $(wildcard tests/*.S)
TEST_SRCS_C := $(wildcard tests/*.c)
TEST_ELFS   := $(TEST_SRCS_S:.S=.elf) $(TEST_SRCS_C:.c=.elf)
```

Discovers all test sources under `tests/`. Both lists are empty until test files are added in blocks 1.1 and 1.2. `TEST_ELFS` is used only by `all-tests`; individual ELFs are built on demand via the pattern rules.

---

### Assembly test pattern rule

```makefile
tests/%.elf: tests/%.S $(CRT0) $(LDSCRIPT)
	$(CC) $(CFLAGS) -I$(COMMON) -T$(LDSCRIPT) -o $@ $(CRT0) $<
```

Compiles one assembly test. `$(CRT0)` is always listed first so `_start` from `crt0.S` is linked before the test's `main`. `-I$(COMMON)` allows assembly files to `.include` or C files to `#include` `uart.h` and `test.h`.

---

### C test pattern rule

```makefile
tests/%.elf: tests/%.c $(CRT0) $(LDSCRIPT)
	$(CC) $(CFLAGS) -I$(COMMON) -Iinclude -T$(LDSCRIPT) -o $@ $(CRT0) $<
```

Same as the assembly rule with `-Iinclude` added for `ccex.h` and other runtime headers introduced in block 1.3.

---

### run-% target

```makefile
run-%: tests/%.elf
	$(QEMU) $(QEMU_FLAGS) -bios $<
```

Makes the named ELF a prerequisite, triggering compilation if needed, then launches it under QEMU. `-bios` loads the ELF at DRAM base `0x80000000` — matching the linker script origin. Example: `make run-block1_1_csr_stubs`.

---

### all-tests and clean

```makefile
all-tests: $(TEST_ELFS)

clean:
	rm -f tests/*.elf examples/*.elf
```

`all-tests` builds every discovered test ELF in one shot. `clean` removes compiled ELFs from both `tests/` and `examples/` (the latter used by block 1.3 runtime wrapper examples).
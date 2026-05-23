# tests/common/link.ld

Bare-metal linker script. Places the image at DRAM base `0x80000000` (QEMU virt). Defines the symbols `_bss_start`, `_bss_end`, `_stack_bottom`, and `_stack_top` consumed by `crt0.S`.

**Works with:**
- [crt0.S](crt0.S.md) — consumes `_stack_top`, `_bss_start`, `_bss_end`; relies on `.text.start` being placed first
- [Makefile](../../Makefile.md) — passed via `-T tests/common/link.ld` to every test compilation

---

### `OUTPUT_ARCH` / `ENTRY`

```ld
OUTPUT_ARCH(riscv)
ENTRY(_start)
```

Sets the output architecture to RISC-V and the ELF entry point to `_start`. `ENTRY` ensures the ELF header's `e_entry` field points to `_start` at `0x80000000`.

---

### `MEMORY` block

```ld
MEMORY
{
    DRAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}
```

Defines a single memory region named `DRAM`. `rwx` allows the linker to place code and data here. 128 MiB matches QEMU virt's default DRAM size.

---

### `.text` section

```ld
    .text :
    {
        *(.text.start)
        *(.text*)
    } > DRAM
```

`*(.text.start)` is listed first, pulling in `_start` from `crt0.S` before any other code. All other `.text*` sections follow. Placed at `0x80000000`.

---

### `.rodata` section

```ld
    .rodata :
    {
        *(.rodata*)
    } > DRAM
```

Read-only data (strings, constants) immediately after `.text`.

---

### `.data` section

```ld
    .data :
    {
        *(.data*)
    } > DRAM
```

Initialized read-write data. Empty for the current bare-metal tests but included for completeness.

---

### `.bss` section

```ld
    .bss :
    {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        _bss_end = .;
    } > DRAM
```

Uninitialized data zeroed by `crt0.S` before `main`. `_bss_start` and `_bss_end` are consumed by the zero loop. `. = ALIGN(8)` ensures `_bss_end` is 8-byte aligned so the loop's `sd zero` stores are always aligned.

---

### Stack

```ld
    . = ALIGN(16);
    _stack_bottom = .;
    . += 16K;
    _stack_top = .;
```

16 KiB stack allocated above `.bss`. `_stack_top` is loaded into `sp` by `_start`. `ALIGN(16)` satisfies ABI requirements for the stack pointer.
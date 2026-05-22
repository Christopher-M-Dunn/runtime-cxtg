# qemu-cxtg/system/vl.c

QEMU main system emulation entry point. Parses command-line options and initializes the machine.

**Phase 0:** modified — added debug printf logging the `-cpu` option value at parse time.

## Functions modified

- [QEMU_OPTION_cpu](vl.c/QEMU_OPTION_cpu.md) — debug printf: logs the `-cpu` option string when parsed

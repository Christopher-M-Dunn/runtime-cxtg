CROSS_COMPILE ?= riscv64-unknown-elf-
CC            := $(CROSS_COMPILE)gcc

QEMU          ?= qemu-cxtg/build/qemu-system-riscv64
QEMU_CPU      ?= rv64,zcx=on
QEMU_FLAGS    := -M virt -nographic -no-reboot -cpu $(QEMU_CPU)

COMMON        := tests/common
LDSCRIPT      := $(COMMON)/link.ld

CFLAGS := -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany
CFLAGS += -nostdlib -nostartfiles -ffreestanding -fno-builtin
CFLAGS += -g -O0

CRT0 := $(COMMON)/crt0.S

TEST_SRCS_S := $(wildcard tests/*.S)
TEST_SRCS_C := $(wildcard tests/*.c)
TEST_ELFS   := $(TEST_SRCS_S:.S=.elf) $(TEST_SRCS_C:.c=.elf)

tests/%.elf: tests/%.S $(CRT0) $(LDSCRIPT)
	$(CC) $(CFLAGS) -I$(COMMON) -T$(LDSCRIPT) -o $@ $(CRT0) $<

tests/%.elf: tests/%.c $(CRT0) $(LDSCRIPT)
	$(CC) $(CFLAGS) -I$(COMMON) -Iinclude -T$(LDSCRIPT) -o $@ $(CRT0) $<

run-%: tests/%.elf
	$(QEMU) $(QEMU_FLAGS) -bios $<

all-tests: $(TEST_ELFS)

clean:
	rm -f tests/*.elf examples/*.elf

.PHONY: all-tests clean
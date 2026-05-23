# linux-headers/asm-riscv/kvm.h — KVM_RISCV_ISA_EXT_ZICX

enum value: Zicx ISA extension identifier in the KVM RISC-V ISA extension enum; used to negotiate Zicx support between QEMU and the KVM kernel module

```c
/* KVM ISA extensions */
enum KVM_RISCV_ISA_EXT_ID {
    ...
    KVM_RISCV_ISA_EXT_ZICX,
```

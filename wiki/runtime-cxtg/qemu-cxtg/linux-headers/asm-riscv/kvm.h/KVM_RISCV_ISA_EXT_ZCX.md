# linux-headers/asm-riscv/kvm.h — KVM_RISCV_ISA_EXT_ZCX

Zcx ISA extension identifier in the KVM RISC-V ISA extension enum; used to negotiate Zcx support between QEMU and the KVM kernel module.

```c
enum KVM_RISCV_ISA_EXT_ID {
    ...
    KVM_RISCV_ISA_EXT_ZCX,
```
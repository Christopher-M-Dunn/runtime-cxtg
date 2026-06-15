# target/riscv/kvm/kvm-cpu.c — kvm_multi_ext_cfgs zcx

Registers `zcx` in `kvm_multi_ext_cfgs[]` linking the QEMU config field `ext_zcx` to the KVM ABI constant `KVM_RISCV_ISA_EXT_ZCX`; allows KVM to negotiate Zcx support with the host kernel.

```c
static RISCVCPUCfgToKvmMultiExtCfg kvm_multi_ext_cfgs[] = {
    ...
    KVM_EXT_CFG("zcx", ext_zcx, KVM_RISCV_ISA_EXT_ZCX),
```
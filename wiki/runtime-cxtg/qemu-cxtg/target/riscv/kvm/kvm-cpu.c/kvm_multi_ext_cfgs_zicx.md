# target/riscv/kvm/kvm-cpu.c — kvm_multi_ext_cfgs zicx

table entry: registers `zicx` in `kvm_multi_ext_cfgs[]` linking the QEMU config field `ext_zicx` to the KVM ABI constant `KVM_RISCV_ISA_EXT_ZICX`; allows KVM to negotiate Zicx support with the host kernel

```c
static RISCVCPUCfgToKvmMultiExtCfg kvm_multi_ext_cfgs[] = {
    ...
    KVM_EXT_CFG("zicx", ext_zicx, KVM_RISCV_ISA_EXT_ZICX),
```

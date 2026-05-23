# qemu-cxtg/target/riscv/kvm/kvm-cpu.c

KVM RISC-V CPU support. Manages KVM ISA extension negotiation and CPU property exposure to userspace.

## Config entries added

- [kvm_multi_ext_cfgs zcx](kvm-cpu.c/kvm_multi_ext_cfgs_zicx.md) — registers zcx with KVM using `KVM_RISCV_ISA_EXT_ZCX`

## Functions modified (debug printfs)

- `kvm_riscv_update_cpu_cfg_isa_ext` — debug printf added
- `kvm_riscv_add_cpu_user_properties` — debug printf added

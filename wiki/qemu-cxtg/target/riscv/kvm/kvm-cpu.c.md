# qemu-cxtg/target/riscv/kvm/kvm-cpu.c

KVM RISC-V CPU support. Manages KVM ISA extension negotiation and CPU property exposure to userspace.

**Phase 0:** modified — added `zicx` to `kvm_multi_ext_cfgs`; added debug printfs.

## Config entries added

- [kvm_multi_ext_cfgs zicx](kvm-cpu.c/kvm_multi_ext_cfgs_zicx.md) — registers zicx with KVM using `KVM_RISCV_ISA_EXT_ZICX`

## Functions modified (debug printfs)

- `kvm_riscv_update_cpu_cfg_isa_ext` — debug printf added
- `kvm_riscv_add_cpu_user_properties` — debug printf added

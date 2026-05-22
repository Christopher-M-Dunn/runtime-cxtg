# target/riscv/meson.build — cx.c source entry

build rule: adds `cx.c` to the `riscv_ss` system sourceset so `target/riscv/cx.c` is compiled into the RISC-V system emulator

```python
riscv_ss.add(files(
  ...
  'cx.c',
```

*← [Main Index](../../../../index.md)* | *↑ [vl.c](../vl.c.md)* | *↗ [view source](../../../../../qemu-cxtg/system/vl.c)*

# system/vl.c — QEMU_OPTION_cpu

option handler: parses the `-cpu` command-line option; debug printf added to log the option value at parse time

```c
        case QEMU_OPTION_cpu:
            cpu_option = optarg;
            printf("# cpu option: %s\n", cpu_option);
```

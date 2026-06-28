*← [Main Index](../../../../../index.md)* | *↑ [cpu-common.c](../cpu-common.c.md)* | *↗ [view source](../../../../../../qemu-cxtg/hw/core/cpu-common.c)*

# hw/core/cpu-common.c — cpu_common_parse_features

function: parses the CPU feature string from the `-cpu` command-line option; debug printf added to log the raw string at parse time

```c
static void cpu_common_parse_features(const char *typename, char *features,
                                       Error **errp)
{
    ...
    printf("# feature string: %s\n", featurestr == NULL ? "NULL" : featurestr);
```

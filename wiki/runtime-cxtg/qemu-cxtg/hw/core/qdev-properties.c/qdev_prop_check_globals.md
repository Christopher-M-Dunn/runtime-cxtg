# hw/core/qdev-properties.c — qdev_prop_check_globals

function: validates all global device properties; debug printfs added to log each property's driver, name, and value

```c
int qdev_prop_check_globals(void)
{
    ...
        printf("# prop->driver: %s\n", prop->driver);
        printf("# prop->property: %s\n", prop->driver);
        printf("# prop->value: %s\n", prop->value);
```

# Software Watchdogs

This module provides an implementation of software watchdogs.
A watchdog is a simple decrementing counter which triggers a callback when it reaches zero.
It can be reset before it reaches zero and therefore is usually used for safety, with a task resetting the watchdog (A) and another task (B) decrementing it.
If the operation task (A) deadlocks for example, the watchdog won't be reset and the safety task (B) will trigger the callback, for example to stop the motors and reset the board.

In this implementation, watchdogs are grouped together in a "list".
The list is statically allocated and can hold up to `WATCHDOGS_PER_LIST` elements (currently 10).

## Warning note about callbacks
The callbacks may be fired from an interrupt or a critical section.
That means they should not rely on any blocking operation or operating system service.

## Example

```cpp
#include <platform-abstraction/panic.h>
#include <watchdog/watchdog.h>

void callback(void)
{
    PANIC("Watchdog fired!\n");
}

void main(void)
{
    watchdog_list_t list;
    watchdog_t dogs[10];
    watchdog_list_init(&list, dogs, sizeof(dogs));

    /* Creates a watchdog with a reset value of 5. */
    watchdog_register(&list, callback, 5);

    while (1) {
        watchdog_list_tick(&list);
    }
}
```



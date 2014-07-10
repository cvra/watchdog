#include <stdlib.h>
#include <platform-abstraction/panic.h>
#include "watchdog.h"

void watchdog_list_init(watchdog_list_t *list)
{
    list->count = 0;
}

watchdog_t* watchdog_register(watchdog_list_t *list, void (*callback)(void), int reset_value)
{
    watchdog_t *dog;

    if (list->count == WATCHDOGS_PER_LIST) {
        PANIC("Watchdog list is full!");
        return NULL;
    }

    dog = &list->watchdogs[list->count];
    list->count++;

    dog->callback = callback;
    dog->reset_value = reset_value;
    dog->value = reset_value;

    return dog;
}

void watchdog_list_tick(watchdog_list_t *list)
{
    int i;

    for (i=0;i<list->count;i++) {
        list->watchdogs[i].value --;

        if (list->watchdogs[i].value == 0) {
            list->watchdogs[i].callback();
        }
    }
}

void watchdog_reset(watchdog_t *watchdog)
{
    watchdog->value = watchdog->reset_value;
}

#include <stdlib.h>
#include <platform-abstraction/panic.h>
#include <platform-abstraction/criticalsection.h>
#include "watchdog.h"

void watchdog_list_init(watchdog_list_t *list, watchdog_t *buffer, size_t buffer_len)
{
    list->count = 0;
    list->watchdogs = buffer;
    list->max_count = buffer_len / sizeof(watchdog_t);
}

watchdog_t* watchdog_register(watchdog_list_t *list, void (*callback)(void), int reset_value)
{
    watchdog_t *dog;
    CRITICAL_SECTION_ALLOC();

    CRITICAL_SECTION_ENTER();
    if (list->count == list->max_count) {
        PANIC("Watchdog list is full!");
        CRITICAL_SECTION_EXIT();
        return NULL;
    }

    dog = &list->watchdogs[list->count];
    list->count++;

    dog->callback = callback;
    dog->reset_value = reset_value;
    dog->value = reset_value;
    CRITICAL_SECTION_EXIT();

    return dog;
}

void watchdog_list_tick(watchdog_list_t *list)
{
    int i, max_count, expired;

    CRITICAL_SECTION_ALLOC();

    CRITICAL_SECTION_ENTER();
    max_count = list->count;
    CRITICAL_SECTION_EXIT();

    for (i=0;i<max_count;i++) {
        CRITICAL_SECTION_ENTER();

        list->watchdogs[i].value --;
        if (list->watchdogs[i].value == 0) {
            expired = 1;
        } else {
            expired = 0;
        }

        CRITICAL_SECTION_EXIT();

        if (expired) {
            list->watchdogs[i].callback();
        }
    }
}

void watchdog_reset(watchdog_t *watchdog)
{
    watchdog->value = watchdog->reset_value;
}

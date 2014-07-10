#include "CppUTest/TestHarness.h"

extern "C" {
#include "../watchdog.h"
#include "platform-abstraction/panic.h"
}

static int call_counter;
static void callback(void)
{
    call_counter++;
}

static void panic_dummy(const char *file, int l, const char *msg)
{
    call_counter++;
}

TEST_GROUP(WatchdogTestGroup)
{
    watchdog_list_t list;

    void setup()
    {
        watchdog_list_init(&list);
        call_counter = 0;
    }
};

TEST(WatchdogTestGroup, CanInitList)
{
    CHECK_EQUAL(0, list.count);
    list.watchdogs[WATCHDOGS_PER_LIST-1].callback = callback;
    POINTERS_EQUAL(callback, list.watchdogs[WATCHDOGS_PER_LIST-1].callback);
}

TEST(WatchdogTestGroup, WatchdogIsAllocatedInStaticBuffer)
{
    const int reset_value = 10;
    watchdog_t *dog;
    dog = watchdog_register(&list, callback, reset_value);
    POINTERS_EQUAL(&list.watchdogs[0], dog);

    dog = watchdog_register(&list, callback, reset_value);
    POINTERS_EQUAL(&list.watchdogs[1], dog);

}

TEST(WatchdogTestGroup, NewWatchdogIsConfiguredAccurately)
{
    watchdog_t *dog;
    const int reset_value = 10;
    dog = watchdog_register(&list, callback, reset_value);

    POINTERS_EQUAL(callback, dog->callback);
    CHECK_EQUAL(reset_value, dog->reset_value);
    CHECK_EQUAL(reset_value, dog->value);
}

TEST(WatchdogTestGroup, TooManyWatchdogsReturnNUll)
{
    UT_PTR_SET(panic, panic_dummy);

    for (int i=0;i<WATCHDOGS_PER_LIST;i++) {
        watchdog_register(&list, callback, 10);
    }

    watchdog_register(&list, callback, 10);
    CHECK_EQUAL(1, call_counter);
}

TEST(WatchdogTestGroup, WatchdogTickLowersCount)
{
    watchdog_t *dog1, *dog2;
    dog1 = watchdog_register(&list, callback, 10);
    dog2 = watchdog_register(&list, callback, 5);

    watchdog_list_tick(&list);

    CHECK_EQUAL(9, dog1->value);
    CHECK_EQUAL(4, dog2->value);
}

TEST(WatchdogTestGroup, WatchdogsFiresOnce)
{
    watchdog_register(&list, callback, 1);

    watchdog_list_tick(&list);
    watchdog_list_tick(&list);
    CHECK_EQUAL(1, call_counter);
}



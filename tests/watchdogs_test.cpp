#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "../watchdog.h"
#include "platform-abstraction/panic.h"
}

static int call_counter;
static void callback(void)
{
    mock().actualCall("callback");
}

static void panic_dummy(const char *file, int l, const char *msg)
{
    mock().actualCall("PANIC");
}

TEST_GROUP(WatchdogTestGroup)
{
    watchdog_list_t list;

    void setup()
    {
        watchdog_list_init(&list);
        call_counter = 0;
    }

    void teardown()
    {
        mock().clear();
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
    mock().expectOneCall("PANIC");

    for (int i=0;i<WATCHDOGS_PER_LIST;i++) {
        watchdog_register(&list, callback, 10);
    }

    watchdog_register(&list, callback, 10);

    mock().checkExpectations();
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
    mock().expectOneCall("callback");
    watchdog_register(&list, callback, 1);

    watchdog_list_tick(&list);
    watchdog_list_tick(&list);

    mock().checkExpectations();
}

TEST(WatchdogTestGroup, OnlyCorrectWatchdogFires)
{
    mock().expectOneCall("callback");
    watchdog_register(&list, callback, 1);
    watchdog_register(&list, NULL, 2); // crashes if fired

    watchdog_list_tick(&list);
    mock().checkExpectations();
}

TEST(WatchdogTestGroup, CanFireMultipleWatchdogs)
{
    mock().expectOneCall("callback");
    mock().expectOneCall("callback");

    watchdog_register(&list, callback, 1);
    watchdog_register(&list, callback, 2);

    watchdog_list_tick(&list);
    watchdog_list_tick(&list);
    mock().checkExpectations();
}

TEST(WatchdogTestGroup, CanResetValue)
{
    watchdog_t *dog = watchdog_register(&list, callback, 10);
    watchdog_list_tick(&list);
    watchdog_reset(dog);
    CHECK_EQUAL(10, dog->value);
}

TEST(WatchdogTestGroup, CanFireMultipleTimeIfReset)
{
    watchdog_t *dog = watchdog_register(&list, callback, 1);

    mock().expectOneCall("callback");
    mock().expectOneCall("callback");

    watchdog_list_tick(&list);
    watchdog_reset(dog);
    watchdog_list_tick(&list);

    mock().checkExpectations();
}



#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#define WATCHDOGS_PER_LIST 10

typedef struct {
    void (*callback)(void);
    int reset_value;
    int value;

} watchdog_t;

typedef struct {
    int count;
    watchdog_t watchdogs[WATCHDOGS_PER_LIST];

} watchdog_list_t;

/** Inits a given watchdog list. */
void watchdog_list_init(watchdog_list_t *list);

/** Register a watchdog timer.
 * @param callback The function to call when the watchdog resets.
 * @param reset_value The value the watchdog is reset to.
 * @returns A pointer to the registered watchdog.
 */
watchdog_t* watchdog_register(watchdog_list_t *list, void (*callback)(void), int reset_value);

/** Ticks all watchdog timers.
 *
 * This function decrements the counter of every watchdogs in the list and if
 * one reaches zero, the appropriate callback is fired.
 */
void watchdog_list_tick(watchdog_list_t *list);

/** Resets a given watchdog counter. */
void watchdog_reset(watchdog_t *watchdog);


#endif

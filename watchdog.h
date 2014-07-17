#ifndef WATCHDOG_H_
#define WATCHDOG_H_

typedef struct {
    void (*callback)(void);
    int reset_value;
    int value;

} watchdog_t;

typedef struct {
    int count;
    int max_count;
    watchdog_t *watchdogs;
} watchdog_list_t;

/** Inits a given watchdog list.
 * @param [in] buffer The memory buffer to use for the watchdog list.
 * @param [in] buffer_len Number of bytes in the buffer.
 * */
void watchdog_list_init(watchdog_list_t *list, watchdog_t *buffer, size_t buffer_len);

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

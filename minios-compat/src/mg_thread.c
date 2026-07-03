/* mg_thread.c - POSIX semaphore + thread-cancellation stubs for the miniOS
 * MiniGUI port. The ONLY consumer is src/newgal/commlcd/commlcd.c, whose
 * background refresh thread (task_do_update) waits on a semaphore and is torn
 * down with pthread_cancel. miniOS's user libs give us pthread_create/join/
 * mutex but no sem_* nor cancellation.
 *
 * These are minimal placeholders sufficient to LINK (milestone A). Task #94
 * rewrites commlcd to refresh synchronously, which removes the refresh thread
 * and every symbol below - delete this file then. Declarations live in
 * minios-compat/include/semaphore.h and pthread.h. */
#include <semaphore.h>
#include <pthread.h>

int sem_init(sem_t *s, int pshared, unsigned value)
{
    (void)pshared;
    if (s)
        s->__v = (int)value;
    return 0;
}

int sem_wait(sem_t *s)
{
    if (s && s->__v > 0)
        s->__v--;
    return 0;
}

int sem_post(sem_t *s)
{
    if (s)
        s->__v++;
    return 0;
}

int sem_destroy(sem_t *s) { (void)s; return 0; }

int pthread_setcancelstate(int state, int *oldstate)
{
    (void)state;
    if (oldstate)
        *oldstate = 0;
    return 0;
}

int pthread_cancel(pthread_t th) { (void)th; return 0; }

void pthread_testcancel(void) { }

/* minios-compat <sys/sem.h> - SysV semaphore stub (standalone doesn't use it).
 * NOTE: `union semun` is intentionally NOT defined here - POSIX leaves it to
 * the application, and MiniGUI's ourhdr.h defines its own. */
#pragma once
#include <sys/ipc.h>
struct sembuf { unsigned short sem_num; short sem_op; short sem_flg; };
int semget(int key, int nsems, int semflg);
int semop(int semid, struct sembuf *sops, unsigned nsops);
int semctl(int semid, int semnum, int cmd, ...);

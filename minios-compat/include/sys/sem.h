#pragma once
#include <sys/ipc.h>
struct sembuf { unsigned short sem_num; short sem_op; short sem_flg; };
union semun { int val; void *buf; unsigned short *array; };
int semget(int key, int nsems, int semflg);
int semop(int semid, struct sembuf *sops, unsigned nsops);
int semctl(int semid, int semnum, int cmd, ...);

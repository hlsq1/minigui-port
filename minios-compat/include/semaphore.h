#pragma once
typedef struct { int __v; } sem_t;
int sem_init(sem_t *, int, unsigned);
int sem_destroy(sem_t *);
int sem_wait(sem_t *);
int sem_trywait(sem_t *);
int sem_post(sem_t *);
int sem_getvalue(sem_t *, int *);

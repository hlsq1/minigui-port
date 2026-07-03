/* minios-compat <pthread.h> - miniOS has pthread_t/mutex but no cancellation
 * API. commlcd's background refresh thread uses PTHREAD_CANCEL_ENABLE /
 * pthread_setcancelstate / pthread_testcancel. Declared here so it compiles.
 *
 * ADAPTATION NOTE: standalone MiniGUI is single-threaded; commlcd's background
 * refresh-thread model is a poor fit for miniOS. The real port should make
 * minios_fb.c flush synchronously (in the event loop) rather than rely on this
 * cancellable thread. These are stubs to keep the probe compiling. */
#pragma once
#include_next <pthread.h>
#ifndef PTHREAD_CANCEL_ENABLE
#define PTHREAD_CANCEL_ENABLE  0
#define PTHREAD_CANCEL_DISABLE 1
#define PTHREAD_CANCEL_DEFERRED 0
int  pthread_setcancelstate(int state, int *oldstate);
int  pthread_setcanceltype(int type, int *oldtype);
int  pthread_cancel(pthread_t th);
void pthread_testcancel(void);
#endif

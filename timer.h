#ifndef TIMER_H
#define TIMER_H

#include <glib.h>

typedef int (* timer_timeout_fn)(void *, void * arg);

struct Timer {
     unsigned int timeout_ms;
     timer_timeout_fn timeout_fn;
     void * fn_arg;
     _Bool is_active;
     _Bool is_expired;
     guint handle;
     GMutex lock;
};

struct Timer * timer_new(timer_timeout_fn fn, void * arg);
void timer_timeout_set(struct Timer *, unsigned int timeout_ms);
void timer_run(struct Timer *);
void timer_stop(struct Timer *);
void timer_release(struct Timer *);

#endif /* TIMER_H */

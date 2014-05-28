#ifndef TIMER_H
#define TIMER_H

struct Timer;

typedef int (* timer_timeout_fn)(void *, void * arg);

struct Timer * timer_new(timer_timeout_fn fn, void * arg);
void timer_timeout_set(struct Timer *, unsigned int timeout_ms);
void timer_run(struct Timer *);
void timer_stop(struct Timer *);
void timer_release(struct Timer *);

#endif /* TIMER_H */

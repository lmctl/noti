#include <stdlib.h>
#include <glib.h>

#include "timer.h"

struct Timer {
     unsigned int timeout_ms;
     timer_timeout_fn timeout_fn;
     void * fn_arg;
     _Bool is_active;
     guint handle;
     GMutex lock;

};

struct Timer * timer_new(timer_timeout_fn fn, void * arg)
{
     struct Timer * t;

     t = calloc(sizeof *t, 1);

     if (!t)
	  return NULL;

     g_mutex_init(&t->lock);
     t->handle = -1;
     t->timeout_fn = fn;
     t->fn_arg = arg;
 
     return t;
}

void timer_timeout_set(struct Timer * t, unsigned int timeout_ms)
{
     t->timeout_ms = timeout_ms;
}

static void timer_timeout_fn_internal(void * _t)
{
     struct Timer * t = _t;

     g_mutex_lock(&t->lock);

     t->timeout_fn(_t, t->fn_arg);
     t->is_active = 0;

     g_mutex_unlock(&t->lock);
}

void timer_run(struct Timer * t)
{
     g_mutex_lock(&t->lock);

     if (t->is_active)
	  g_source_remove(t->handle);

     t->is_active = 1;
     t->handle = g_timeout_add(t->timeout_ms, timer_timeout_fn_internal, t);

     g_mutex_unlock(&t->lock);
}

void timer_stop(struct Timer * t)
{
     g_mutex_lock(&t->lock);

     if (t->is_active) {
	  g_source_remove(t->handle);
	  t->is_active = 0;
     }

     g_mutex_unlock(&t->lock);
}

void timer_release(struct Timer * t)
{
     timer_stop(t);
     free(t);
}

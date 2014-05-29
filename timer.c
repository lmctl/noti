#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "timer.h"

static void timer_fill(struct Timer * t, timer_timeout_fn fn, void * arg)
{
     memset(t, 0, sizeof(struct Timer));

     g_mutex_init(&t->lock);
     t->handle = -1;
     t->timeout_fn = fn;
     t->fn_arg = arg;
}

struct Timer * timer_new(timer_timeout_fn fn, void * arg)
{
     struct Timer * t;

     t = malloc(sizeof *t);
     if (!t)
	  return NULL;

     timer_fill(t, fn, arg);
     return t;
}

void timer_init(struct Timer * t, timer_timeout_fn fn, void * arg)
{
     timer_fill(t, fn, arg);
}

void timer_timeout_set(struct Timer * t, unsigned int timeout_ms)
{
     g_mutex_lock(&t->lock);
     t->timeout_ms = timeout_ms;
     g_mutex_unlock(&t->lock);
}

unsigned int timer_timeout_get(struct Timer * t)
{
     unsigned int timeout_ms;

     g_mutex_lock(&t->lock);
     timeout_ms = t->timeout_ms;
     g_mutex_unlock(&t->lock);

     return timeout_ms;
}

static void timer_timeout_fn_internal(void * _t)
{
     struct Timer * t = _t;

     g_mutex_lock(&t->lock);

     if (t->timeout_fn)
	  t->timeout_fn(_t, t->fn_arg);

     t->is_active = 0;
     t->is_expired = 1;

     g_mutex_unlock(&t->lock);
}

void timer_timeout_fn_set(struct Timer * t, timer_timeout_fn fn, void * arg)
{
     g_mutex_lock(&t->lock);

     t->timeout_fn = fn;
     t->fn_arg = arg;

     g_mutex_unlock(&t->lock);
}

_Bool timer_is_expired(struct Timer * t)
{
     _Bool flag;

     g_mutex_lock(&t->lock);
     flag = t->is_expired;
     g_mutex_unlock(&t->lock);

     return flag;
}

_Bool timer_is_active(struct Timer * t)
{
     _Bool flag;

     g_mutex_lock(&t->lock);
     flag = t->is_active;
     g_mutex_unlock(&t->lock);

     return flag;
}

void timer_run(struct Timer * t)
{
     g_mutex_lock(&t->lock);

     if (t->is_active)
	  g_source_remove(t->handle);

     t->is_expired = 0;
     t->is_active = 1;
     t->handle = g_timeout_add(t->timeout_ms, (GSourceFunc)timer_timeout_fn_internal, t);

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

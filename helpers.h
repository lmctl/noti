#ifndef HELPERS_H
#define HELPERS_H

#include "notification.h"

/* Trivial lambda-like functions only */

static inline int h_notification_cmp_id(void * _n, void * _id)
{
     struct Notification * n = _n;
     uint32_t id = (long)_id;

     return n->id == id;
}

static inline int h_notification_replace(void * _old, void * _new)
{
     struct Notification * old = _old;
     struct Notification * new = _new;

     notification_update(old, new->app, new->summary, new->body, new->expire_ms);

     return 1;
}

static inline void h_notification_release(void * _n)
{
     struct Notification * n = _n;

     return notification_release(n);
}


static int h_notification_print(void * _n, void * unused)
{
     struct Notification * n = _n;

     notification_print(n);

     return 1;
}

#endif /* HELPERS_H */

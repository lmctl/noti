#ifndef HELPERS_H
#define HELPERS_H

#include "notification.h"
#include "timer.h"

/* Trivial lambda-like functions only */

static inline int h_notification_cmp_id(void * _n, void * _id)
{
     struct Notification * n = _n;
     uint32_t id = (long)_id;

     return n->id == id;
}

static int h_notification_remove_test(void * _n, void * _arg)
{
     struct Notification * n = _n;

     return n->been_shown && timer_is_expired(&n->timer);
}

static int h_notification_print(void * _n, void * unused)
{
     struct Notification * n = _n;

     notification_print(n);

     return 1;
}

#endif /* HELPERS_H */

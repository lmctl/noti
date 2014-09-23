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


static int h_notification_print(void * _n, void * unused)
{
     struct Notification * n = _n;

     notification_print(n);

     return 1;
}

#endif /* HELPERS_H */

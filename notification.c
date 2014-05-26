#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "notification.h"

#define APP_MAX 128
#define SUMMARY_MAX 128
#define BODY_MAX 2048

/*  Get monotically increasing notification id
 *
 *  Implementation note: Spec (1.2) requires us to return notification
 *  id as UINT32 but it also states:
 *
 *    [id] "It is unique, and will not be reused unless a MAXINT
 *    number of notifications have been generated. An acceptable
 *    implementation may just use an incrementing counter for the
 *    ID. The returned ID is always greater than zero. Servers must
 *    make sure not to return zero as an ID."
 *
 *  MAXINT value is not described in this spec so it's assumed it is
 *  standard INT(32)_MAX.  Consequence is that on 32-bit machines
 *  notification id wraps at 2^31-1 instead of 2^32-1 (what UINT32
 *  would allow).
 */
uint32_t get_next_id(void)
{
     static int id_seq = 0;

     /* XXX Locking ... */
     ++ id_seq;

     if (id_seq == 0 || id_seq >= INT32_MAX)
	  id_seq = 1;

     return id_seq;
}

static void notification_fill(struct Notification * n, char * app, char * summary, char * body, int32_t expire_ms)
{
     if (n->app)
	  free(n->app);

     if (n->summary)
	  free(n->summary);

     if (n->body)
	  free(n->body);

     n->expire_ms = expire_ms;
     n->app = strndup(app, APP_MAX);
     n->summary = strndup(summary, SUMMARY_MAX);
     n->body = strndup(body, BODY_MAX);
     gettimeofday(&n->timestamp, NULL);
}

struct Notification * notification_new(char * app, char * summary, char * body, int32_t expire_ms)
{
     struct Notification * n;

     n = calloc(sizeof *n, 1);
     if (!n)
	  return NULL;

     notification_fill(n, app, summary, body, expire_ms);
     n->id = get_next_id();

     return n;
}

void notification_update(struct Notification * n, char * app, char * summary, char * body, int32_t expire_ms)
{
     notification_fill(n, app, summary, body, expire_ms);

     n->is_closed = 0;
     n->is_expired = 0;
     n->been_shown = 0;
}

void notification_release(struct Notification * n)
{
     free(n->summary);
     free(n->body);
     free(n);
}

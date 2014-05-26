#include <stdlib.h>
#include <glib.h>
#include "data.h"

struct Data {
     GQueue q;
     GMutex lock;
     data_release_fn release_fn;
};

struct Data * data_new(data_release_fn fn)
{
     struct Data * ctx;

     ctx = calloc(sizeof *ctx, 1);
     g_queue_init(&ctx->q);
     g_mutex_init(&ctx->lock);
     ctx->release_fn = fn;
}

void data_add(struct Data * ctx, void *p)
{
     g_mutex_lock(&ctx->lock);
     g_queue_push_tail(&ctx->q, p);
     g_mutex_unlock(&ctx->lock);
}

void data_clear(struct Data * ctx)
{
     GList * p;

     g_mutex_lock(&ctx->lock);

     for (p = ctx->q.head; p; p = p->next)
	  ctx->release_fn(p->data);
     g_queue_clear(&ctx->q);

     g_mutex_unlock(&ctx->lock);
}

int data_remove_if(struct Data * ctx, data_condition_fn fn, void * arg)
{
     GList * p, * r;
     int removed = 0;

     g_mutex_lock(&ctx->lock);

     for (p = ctx->q.head; p; p = r) {
	  r = p->next;

	  if (fn(p->data, arg) == 1) {
	       ctx->release_fn(p->data);
	       g_queue_remove(&ctx->q, p->data);
	       removed = 1;
	  }
     }

     g_mutex_unlock(&ctx->lock);

     return removed;
}

void data_release(struct Data * ctx)
{
     data_clear(ctx);
     g_queue_free(&ctx->q);
     free(ctx);
}

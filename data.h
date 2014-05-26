#ifndef DATA_H
#define DATA_H

struct Data;

typedef void (* data_release_fn)(void *);
typedef int (* data_condition_fn)(void *, void * arg);

struct Data * data_new(data_release_fn fn);
void data_add(struct Data *, void *);
void data_clear(struct Data *);

/* remove element if fn returns 1 */
int data_remove_if(struct Data *, data_condition_fn fn, void * arg);
void data_release(struct Data *);

#endif /* DATA_H */

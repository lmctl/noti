#ifndef DATA_H
#define DATA_H

struct Data;

typedef void (* data_release_fn)(void *);
typedef void (* data_remove_if_fn)(void *);

struct Data * data_new(data_release_fn fn);
void data_add(struct Data *, void *);
void data_clear(struct Data *);
void data_remove_if(struct Data *, data_remove_if_fn fn); /* remove element if fn returns 1 */
void data_release(struct Data *);

#endif /* DATA_H */


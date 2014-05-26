#ifndef DATA_H
#define DATA_H

struct Data;

typedef void (* data_release_fn)(void *);
typedef void (* data_apply_fn)(void *, void * arg);
typedef int (* data_condition_fn)(void *, void * arg);

struct Data * data_new(data_release_fn fn);
void data_add(struct Data *, void *);
void data_clear(struct Data *);

/* remove element if fn returns 1 */
int data_remove_if(struct Data *, data_condition_fn fn, void * arg);

/* call applyfn(data_element, arg) for each data element */
void data_apply(struct Data * ctx, data_apply_fn applyfn, void * arg);

/* for each data element stored in ctx call applyfn(data_element, arg2) if testfn(data_element, arg1) returns 1 */
int data_apply_if(struct Data *, data_condition_fn testfn, void * arg1, data_apply_fn applyfn, void * arg2);

void data_release(struct Data *);

#endif /* DATA_H */

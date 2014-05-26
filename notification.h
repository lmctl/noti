#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <stdint.h>

/*  Get monotically increasing notification id
 *
 *  See implementation notes about not full uniqueness of id
 */
uint32_t get_next_id(void);

#endif /* NOTIFICATION_H */

#include <stdlib.h>
#include <stdint.h>
#include "notification.h"

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

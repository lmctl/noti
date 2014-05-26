#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <stdint.h>
#include <sys/time.h>

struct Notification {
     uint32_t id;
     char * app;
     char * summary;
     char * body;
     struct timeval timestamp;   /* time when notification has been received */
     int32_t expire_ms;          /* expire notification this many milliseconds after receiving it */
     _Bool is_closed;            /* notification closed via eplicit call to CloseNotification */
     _Bool is_expired;           /* ... expired via timer */
     _Bool been_shown;           /* has the notification been shown at all? */
};

/*  Get monotically increasing notification id
 *
 *  See implementation notes about not full uniqueness of id
 */
uint32_t get_next_id(void);

/*  Create new notification entry
 *
 *  Note that timestamp and id are automatically generated */
struct Notification * notification_new(uint32_t id, char * app, char * summary, char * body, int32_t expire_ms);

/*  Replace existing notification
 *
 *  Also updates timestamp and resets flags */
void notification_update(struct Notification * n, char * app, char * summary, char * body, int32_t expire_ms);

/*  Release previously allocated notification and its members */
void notification_release(struct Notification * n);

#endif /* NOTIFICATION_H */

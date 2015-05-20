CFLAGS		:= $(shell pkg-config --cflags glib-2.0 gio-2.0) -std=c99 -g3 -D_POSIX_C_SOURCE=200809L
LDFLAGS		:= $(shell pkg-config --libs glib-2.0 gio-2.0)

OBJS		:= notification.o main.o

all:		$(OBJS)
		$(CC) -o notigrabber $(LDFLAGS) $(OBJS)

clobber:
		rm -f notigrabber *.o *~

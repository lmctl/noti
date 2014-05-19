CFLAGS		:= $(shell pkg-config --cflags glib-2.0 gio-2.0) -std=c99 -g3
LDFLAGS		:= $(shell pkg-config --libs glib-2.0 gio-2.0)

OBJS		:= main.o

all:		$(OBJS)
		$(CC) -o traynoti $(LDFLAGS) $(OBJS)

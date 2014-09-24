#include "data.h"
#include "notification.h"
#include <gtk/gtk.h>

static GtkStatusIcon *icon;

#define ICON_N_NONE GTK_STOCK_FILE
#define ICON_N_NEW  GTK_STOCK_EDIT

struct Data *data;

void ui_init(int *ac, char **av[], struct Data *ctx)
{
     gtk_set_locale();
     gtk_init(ac, av);

     icon = gtk_status_icon_new_from_stock(ICON_N_NONE);
     gtk_status_icon_set_visible(icon, TRUE);

     data = ctx;
}


static int data_shown_test(void * _n, void * _arg)
{
     struct Notification * n = _n;

     return !n->been_shown;
}

void applyfn(void * a, void * b)
{
}

void ui_update(void)
{
     int r;

     r = data_apply_if(data, data_shown_test, 0, applyfn, 0);

     if (r)
	  gtk_status_icon_set_from_stock(icon, ICON_N_NEW);
     else
	  gtk_status_icon_set_from_stock(icon, ICON_N_NONE);
}

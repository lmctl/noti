/* traynoti: collect desktop notifications in tray app; show only if explicitly requested
 * Copyright (C) 2014  Karol Lewandowski
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <stdio.h>
#include <stdint.h>

#include <glib.h>
#include <gio/gio.h>

#include "notification.h"
#include "data.h"
#include "helpers.h"

/*  Server identification
 *
 *  This information is returned verbatim by GetServerIdentification D-Bus method
 */
static const char noti_server_name[] = "traynoti";
static const char noti_server_vendor[] = "Karol Lewandowski";
static const char noti_server_version[] = "0.1";
static const char noti_spec_version[] = "1.2";

/*  Notification Specification version 1.2 describes following capabilities:/*
 *
 *    action-icons
 *    actions
 *    body
 *    body-hyperlinks
 *    body-images
 *    body-markup
 *    icon-multi
 *    icon-static
 *    persistence
 *    sound
 *
 *  We intend to support handful of these.
 */
static const char *noti_server_capabilities[] = { "body", "summary" };

/*  D-Bus boilerplate
 */
static const char noti_dbus_service[] = "org.freedesktop.Notifications";
static const char noti_dbus_interface[] = "org.freedesktop.Notifications";
static const char noti_dbus_path[] = "/org/freedesktop/Notifications";

static const char noti_dbus_introspection_xml[] = ""
       "<node name=\"/org/freedesktop/Notifications\">"
       "   <interface name=\"org.freedesktop.Notifications\">"
       "    <method name=\"GetCapabilities\">"
       "      <arg name=\"ret\" type=\"as\" direction=\"out\"/>"
       "    </method>"
       "    <method name=\"Notify\">"
       "      <arg name=\"app_name\" type=\"s\" direction=\"in\"/>"
       "      <arg name=\"replaces_id\" type=\"u\" direction=\"in\"/>"
       "      <arg name=\"app_icon\" type=\"s\" direction=\"in\"/>"
       "      <arg name=\"summary\" type=\"s\" direction=\"in\"/>"
       "      <arg name=\"body\" type=\"s\" direction=\"in\"/>"
       "      <arg name=\"actions\" type=\"as\" direction=\"in\"/>"
       "      <arg name=\"hints\" type=\"a{sv}\" direction=\"in\"/>"
       "      <arg name=\"exipire_timeout\" type=\"i\" direction=\"in\"/>"
       "      <arg name=\"id\" type=\"u\" direction=\"out\"/>"
       "    </method>"
       "    <method name=\"GetServerInformation\">"
       "      <arg name=\"name\" type=\"s\" direction=\"out\"/>"
       "      <arg name=\"vendor\" type=\"s\" direction=\"out\"/>"
       "      <arg name=\"version\" type=\"s\" direction=\"out\"/>"
       "      <arg name=\"spec_version\" type=\"s\" direction=\"out\"/>"
       "    </method>"
       "    <method name=\"CloseNotification\">"
       "      <arg name=\"id\" type=\"u\" direction=\"in\"/>"
       "    </method>"
       "  </interface>"
       "</node>";

static struct Data *data;

static void data_n_release(void * _n)
{
     struct Notification * n = _n;

     timer_stop(&n->timer);

     return notification_release(n);
}


static int n_replace(void * _old, void * _new)
{
     struct Notification * old = _old;
     struct Notification * new = _new;

     timer_stop(&old->timer);
     notification_update(old, new->app, new->summary, new->body, new->expire_ms);
     timer_timeout_set(&old->timer, old->expire_ms);
     timer_run(&old->timer);

     return 1;
}

static void on_method_call(GDBusConnection * conn, const gchar * sender, const gchar * obj_path,
		    const gchar * iface_name, const gchar * method_name, GVariant * params,
		    GDBusMethodInvocation * invocation, gpointer user_data)
{
     GVariant * v;

     if (!g_strcmp0(method_name, "GetCapabilities")) {
	  GVariantBuilder * b = g_variant_builder_new(G_VARIANT_TYPE("as"));
	  int i;

	  for (i = 0; i < G_N_ELEMENTS(noti_server_capabilities); i++)
	       g_variant_builder_add(b, "s", noti_server_capabilities[i]);

	  v = g_variant_new("(as)", b);
	  g_dbus_method_invocation_return_value(invocation, v); //g_variant_new_tuple(&v, 1));

	  g_variant_builder_unref(b);

     } else if (!g_strcmp0(method_name, "GetServerInformation")) {

	  v = g_variant_new("(ssss)", noti_server_name, noti_server_vendor, noti_server_version, noti_spec_version);
	  g_dbus_method_invocation_return_value(invocation, v);

     } else if (!g_strcmp0(method_name, "Notify")) {

	  gchar * app_name = NULL, * summary = NULL, * body = NULL;
	  uint32_t id;
	  int32_t expire_ms;
	  struct Notification * n;

	  g_variant_get(params, "(&su&s&s&s^a&sa{sv}i)", &app_name, &id, NULL, &summary, &body, NULL, NULL, &expire_ms);

	  n = notification_new(id, app_name, summary, body, expire_ms);

	  notification_print(n);

	  if (!id) {
	       data_add(data, n);

	       if (n->expire_ms > 0) {
		    timer_init(&n->timer, 0, 0);
		    timer_timeout_set(&n->timer, n->expire_ms);
		    timer_run(&n->timer);
	       }

	  } else {
	       int r;

	       r = data_apply_if(data, h_notification_cmp_id, (void *)n->id, n_replace, (void *)n);
	       if (!r)
		    g_warning("Unable to update non-existent notification id %u", n->id);

	       notification_release(n);
	  }

	  v = g_variant_new("(u)", n->id);
	  g_dbus_method_invocation_return_value(invocation, v);

     } else if (!g_strcmp0(method_name, "CloseNotification")) {

     } else
	  g_warning("Method not supported");
}

static GDBusNodeInfo * g_introspection;

static void on_bus_acquired(GDBusConnection * conn, const gchar * name, gpointer user_data)
{
     static GDBusInterfaceVTable noti_vtable = {on_method_call, 0, 0};

     GError * error = 0;
     guint r;

     r = g_dbus_connection_register_object(conn,
					   noti_dbus_path,
					   g_introspection->interfaces[0],
					   &noti_vtable,
					   0,
					   0,
					   &error);
}

void on_name_acquired(GDBusConnection * conn, const gchar * name, gpointer user_data)
{
}

void on_name_lost(GDBusConnection *conn, const gchar *name, gpointer user_data)
{
}

/* Remove notification if, and only if:
 *
 *  - it has been shown at least once
 *
 *  - its timer has expired
 */
int d_cleanup(void)
{
     return data_remove_if(data, h_notification_remove_test, NULL);
}

int main(int ac, char * av[])
{
     GMainLoop * ev;

     data = data_new(data_n_release);

     g_introspection = g_dbus_node_info_new_for_xml(noti_dbus_introspection_xml, NULL);

     g_bus_own_name(G_BUS_TYPE_SESSION,
		    noti_dbus_service,
		    G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT | G_BUS_NAME_OWNER_FLAGS_REPLACE,
		    on_bus_acquired,
		    on_name_acquired,
		    on_name_lost,
		    0,
		    0);

     ev = g_main_loop_new(NULL, FALSE);
     g_main_loop_run(ev);

     return 0;
}

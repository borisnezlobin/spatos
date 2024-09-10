/*
 * This file is generated by gdbus-codegen, do not modify it.
 *
 * The license of this code is the same as for the D-Bus interface description
 * it was derived from. Note that it links to GLib, so must comply with the
 * LGPL linking clauses.
 */

#ifndef __THUNAR_THUMBNAILER_PROXY_H__
#define __THUNAR_THUMBNAILER_PROXY_H__

#include <gio/gio.h>

G_BEGIN_DECLS


/* ------------------------------------------------------------------------ */
/* Declarations for org.freedesktop.thumbnails.Thumbnailer1 */

#define THUNAR_TYPE_THUMBNAILER_DBUS (thunar_thumbnailer_dbus_get_type ())
#define THUNAR_THUMBNAILER_DBUS(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), THUNAR_TYPE_THUMBNAILER_DBUS, ThunarThumbnailerDBus))
#define THUNAR_IS_THUMBNAILER_DBUS(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), THUNAR_TYPE_THUMBNAILER_DBUS))
#define THUNAR_THUMBNAILER_DBUS_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), THUNAR_TYPE_THUMBNAILER_DBUS, ThunarThumbnailerDBusIface))

struct _ThunarThumbnailerDBus;
typedef struct _ThunarThumbnailerDBus ThunarThumbnailerDBus;
typedef struct _ThunarThumbnailerDBusIface ThunarThumbnailerDBusIface;

struct _ThunarThumbnailerDBusIface
{
  GTypeInterface parent_iface;


  gboolean (*handle_dequeue) (
    ThunarThumbnailerDBus *object,
    GDBusMethodInvocation *invocation,
    guint arg_handle);

  gboolean (*handle_get_schedulers) (
    ThunarThumbnailerDBus *object,
    GDBusMethodInvocation *invocation);

  gboolean (*handle_get_supported) (
    ThunarThumbnailerDBus *object,
    GDBusMethodInvocation *invocation);

  gboolean (*handle_queue) (
    ThunarThumbnailerDBus *object,
    GDBusMethodInvocation *invocation,
    const gchar *const *arg_uris,
    const gchar *const *arg_mime_hints,
    const gchar *arg_flavor,
    const gchar *arg_scheduler,
    guint arg_handle_to_unqueue);

  void (*error) (
    ThunarThumbnailerDBus *object,
    guint arg_handle,
    const gchar *const *arg_failed_uris,
    gint arg_error_code,
    const gchar *arg_message);

  void (*finished) (
    ThunarThumbnailerDBus *object,
    guint arg_handle);

  void (*ready) (
    ThunarThumbnailerDBus *object,
    guint arg_handle,
    const gchar *const *arg_uris);

  void (*started) (
    ThunarThumbnailerDBus *object,
    guint arg_handle);

};

GType thunar_thumbnailer_dbus_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *thunar_thumbnailer_dbus_interface_info (void);
guint thunar_thumbnailer_dbus_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void thunar_thumbnailer_dbus_complete_queue (
    ThunarThumbnailerDBus *object,
    GDBusMethodInvocation *invocation,
    guint handle);

void thunar_thumbnailer_dbus_complete_dequeue (
    ThunarThumbnailerDBus *object,
    GDBusMethodInvocation *invocation);

void thunar_thumbnailer_dbus_complete_get_supported (
    ThunarThumbnailerDBus *object,
    GDBusMethodInvocation *invocation,
    const gchar *const *uri_schemes,
    const gchar *const *mime_types);

void thunar_thumbnailer_dbus_complete_get_schedulers (
    ThunarThumbnailerDBus *object,
    GDBusMethodInvocation *invocation,
    const gchar *const *schedulers);



/* D-Bus signal emissions functions: */
void thunar_thumbnailer_dbus_emit_started (
    ThunarThumbnailerDBus *object,
    guint arg_handle);

void thunar_thumbnailer_dbus_emit_finished (
    ThunarThumbnailerDBus *object,
    guint arg_handle);

void thunar_thumbnailer_dbus_emit_ready (
    ThunarThumbnailerDBus *object,
    guint arg_handle,
    const gchar *const *arg_uris);

void thunar_thumbnailer_dbus_emit_error (
    ThunarThumbnailerDBus *object,
    guint arg_handle,
    const gchar *const *arg_failed_uris,
    gint arg_error_code,
    const gchar *arg_message);



/* D-Bus method calls: */
void thunar_thumbnailer_dbus_call_queue (
    ThunarThumbnailerDBus *proxy,
    const gchar *const *arg_uris,
    const gchar *const *arg_mime_hints,
    const gchar *arg_flavor,
    const gchar *arg_scheduler,
    guint arg_handle_to_unqueue,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean thunar_thumbnailer_dbus_call_queue_finish (
    ThunarThumbnailerDBus *proxy,
    guint *out_handle,
    GAsyncResult *res,
    GError **error);

gboolean thunar_thumbnailer_dbus_call_queue_sync (
    ThunarThumbnailerDBus *proxy,
    const gchar *const *arg_uris,
    const gchar *const *arg_mime_hints,
    const gchar *arg_flavor,
    const gchar *arg_scheduler,
    guint arg_handle_to_unqueue,
    guint *out_handle,
    GCancellable *cancellable,
    GError **error);

void thunar_thumbnailer_dbus_call_dequeue (
    ThunarThumbnailerDBus *proxy,
    guint arg_handle,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean thunar_thumbnailer_dbus_call_dequeue_finish (
    ThunarThumbnailerDBus *proxy,
    GAsyncResult *res,
    GError **error);

gboolean thunar_thumbnailer_dbus_call_dequeue_sync (
    ThunarThumbnailerDBus *proxy,
    guint arg_handle,
    GCancellable *cancellable,
    GError **error);

void thunar_thumbnailer_dbus_call_get_supported (
    ThunarThumbnailerDBus *proxy,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean thunar_thumbnailer_dbus_call_get_supported_finish (
    ThunarThumbnailerDBus *proxy,
    gchar ***out_uri_schemes,
    gchar ***out_mime_types,
    GAsyncResult *res,
    GError **error);

gboolean thunar_thumbnailer_dbus_call_get_supported_sync (
    ThunarThumbnailerDBus *proxy,
    gchar ***out_uri_schemes,
    gchar ***out_mime_types,
    GCancellable *cancellable,
    GError **error);

void thunar_thumbnailer_dbus_call_get_schedulers (
    ThunarThumbnailerDBus *proxy,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean thunar_thumbnailer_dbus_call_get_schedulers_finish (
    ThunarThumbnailerDBus *proxy,
    gchar ***out_schedulers,
    GAsyncResult *res,
    GError **error);

gboolean thunar_thumbnailer_dbus_call_get_schedulers_sync (
    ThunarThumbnailerDBus *proxy,
    gchar ***out_schedulers,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define THUNAR_TYPE_THUMBNAILER_DBUS_PROXY (thunar_thumbnailer_dbus_proxy_get_type ())
#define THUNAR_THUMBNAILER_DBUS_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), THUNAR_TYPE_THUMBNAILER_DBUS_PROXY, ThunarThumbnailerDBusProxy))
#define THUNAR_THUMBNAILER_DBUS_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), THUNAR_TYPE_THUMBNAILER_DBUS_PROXY, ThunarThumbnailerDBusProxyClass))
#define THUNAR_THUMBNAILER_DBUS_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), THUNAR_TYPE_THUMBNAILER_DBUS_PROXY, ThunarThumbnailerDBusProxyClass))
#define THUNAR_IS_THUMBNAILER_DBUS_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), THUNAR_TYPE_THUMBNAILER_DBUS_PROXY))
#define THUNAR_IS_THUMBNAILER_DBUS_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), THUNAR_TYPE_THUMBNAILER_DBUS_PROXY))

typedef struct _ThunarThumbnailerDBusProxy ThunarThumbnailerDBusProxy;
typedef struct _ThunarThumbnailerDBusProxyClass ThunarThumbnailerDBusProxyClass;
typedef struct _ThunarThumbnailerDBusProxyPrivate ThunarThumbnailerDBusProxyPrivate;

struct _ThunarThumbnailerDBusProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  ThunarThumbnailerDBusProxyPrivate *priv;
};

struct _ThunarThumbnailerDBusProxyClass
{
  GDBusProxyClass parent_class;
};

GType thunar_thumbnailer_dbus_proxy_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (ThunarThumbnailerDBusProxy, g_object_unref)
#endif

void thunar_thumbnailer_dbus_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
ThunarThumbnailerDBus *thunar_thumbnailer_dbus_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
ThunarThumbnailerDBus *thunar_thumbnailer_dbus_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void thunar_thumbnailer_dbus_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
ThunarThumbnailerDBus *thunar_thumbnailer_dbus_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
ThunarThumbnailerDBus *thunar_thumbnailer_dbus_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define THUNAR_TYPE_THUMBNAILER_DBUS_SKELETON (thunar_thumbnailer_dbus_skeleton_get_type ())
#define THUNAR_THUMBNAILER_DBUS_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), THUNAR_TYPE_THUMBNAILER_DBUS_SKELETON, ThunarThumbnailerDBusSkeleton))
#define THUNAR_THUMBNAILER_DBUS_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), THUNAR_TYPE_THUMBNAILER_DBUS_SKELETON, ThunarThumbnailerDBusSkeletonClass))
#define THUNAR_THUMBNAILER_DBUS_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), THUNAR_TYPE_THUMBNAILER_DBUS_SKELETON, ThunarThumbnailerDBusSkeletonClass))
#define THUNAR_IS_THUMBNAILER_DBUS_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), THUNAR_TYPE_THUMBNAILER_DBUS_SKELETON))
#define THUNAR_IS_THUMBNAILER_DBUS_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), THUNAR_TYPE_THUMBNAILER_DBUS_SKELETON))

typedef struct _ThunarThumbnailerDBusSkeleton ThunarThumbnailerDBusSkeleton;
typedef struct _ThunarThumbnailerDBusSkeletonClass ThunarThumbnailerDBusSkeletonClass;
typedef struct _ThunarThumbnailerDBusSkeletonPrivate ThunarThumbnailerDBusSkeletonPrivate;

struct _ThunarThumbnailerDBusSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  ThunarThumbnailerDBusSkeletonPrivate *priv;
};

struct _ThunarThumbnailerDBusSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType thunar_thumbnailer_dbus_skeleton_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (ThunarThumbnailerDBusSkeleton, g_object_unref)
#endif

ThunarThumbnailerDBus *thunar_thumbnailer_dbus_skeleton_new (void);


G_END_DECLS

#endif /* __THUNAR_THUMBNAILER_PROXY_H__ */

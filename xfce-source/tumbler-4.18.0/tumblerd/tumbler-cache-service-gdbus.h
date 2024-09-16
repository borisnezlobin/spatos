/*
 * This file is generated by gdbus-codegen, do not modify it.
 *
 * The license of this code is the same as for the D-Bus interface description
 * it was derived from. Note that it links to GLib, so must comply with the
 * LGPL linking clauses.
 */

#ifndef __TUMBLER_CACHE_SERVICE_GDBUS_H__
#define __TUMBLER_CACHE_SERVICE_GDBUS_H__

#include <gio/gio.h>

G_BEGIN_DECLS


/* ------------------------------------------------------------------------ */
/* Declarations for org.freedesktop.thumbnails.Cache1 */

#define TUMBLER_TYPE_EXPORTED_CACHE_SERVICE (tumbler_exported_cache_service_get_type ())
#define TUMBLER_EXPORTED_CACHE_SERVICE(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE, TumblerExportedCacheService))
#define TUMBLER_IS_EXPORTED_CACHE_SERVICE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE))
#define TUMBLER_EXPORTED_CACHE_SERVICE_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE, TumblerExportedCacheServiceIface))

struct _TumblerExportedCacheService;
typedef struct _TumblerExportedCacheService TumblerExportedCacheService;
typedef struct _TumblerExportedCacheServiceIface TumblerExportedCacheServiceIface;

struct _TumblerExportedCacheServiceIface
{
  GTypeInterface parent_iface;

  gboolean (*handle_cleanup) (
    TumblerExportedCacheService *object,
    GDBusMethodInvocation *invocation,
    const gchar *const *arg_base_uris,
    guint arg_since);

  gboolean (*handle_copy) (
    TumblerExportedCacheService *object,
    GDBusMethodInvocation *invocation,
    const gchar *const *arg_from_uris,
    const gchar *const *arg_to_uris);

  gboolean (*handle_delete) (
    TumblerExportedCacheService *object,
    GDBusMethodInvocation *invocation,
    const gchar *const *arg_uris);

  gboolean (*handle_move) (
    TumblerExportedCacheService *object,
    GDBusMethodInvocation *invocation,
    const gchar *const *arg_from_uris,
    const gchar *const *arg_to_uris);

};

GType tumbler_exported_cache_service_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *tumbler_exported_cache_service_interface_info (void);
guint tumbler_exported_cache_service_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void tumbler_exported_cache_service_complete_move (
    TumblerExportedCacheService *object,
    GDBusMethodInvocation *invocation);

void tumbler_exported_cache_service_complete_copy (
    TumblerExportedCacheService *object,
    GDBusMethodInvocation *invocation);

void tumbler_exported_cache_service_complete_delete (
    TumblerExportedCacheService *object,
    GDBusMethodInvocation *invocation);

void tumbler_exported_cache_service_complete_cleanup (
    TumblerExportedCacheService *object,
    GDBusMethodInvocation *invocation);



/* D-Bus method calls: */
void tumbler_exported_cache_service_call_move (
    TumblerExportedCacheService *proxy,
    const gchar *const *arg_from_uris,
    const gchar *const *arg_to_uris,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean tumbler_exported_cache_service_call_move_finish (
    TumblerExportedCacheService *proxy,
    GAsyncResult *res,
    GError **error);

gboolean tumbler_exported_cache_service_call_move_sync (
    TumblerExportedCacheService *proxy,
    const gchar *const *arg_from_uris,
    const gchar *const *arg_to_uris,
    GCancellable *cancellable,
    GError **error);

void tumbler_exported_cache_service_call_copy (
    TumblerExportedCacheService *proxy,
    const gchar *const *arg_from_uris,
    const gchar *const *arg_to_uris,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean tumbler_exported_cache_service_call_copy_finish (
    TumblerExportedCacheService *proxy,
    GAsyncResult *res,
    GError **error);

gboolean tumbler_exported_cache_service_call_copy_sync (
    TumblerExportedCacheService *proxy,
    const gchar *const *arg_from_uris,
    const gchar *const *arg_to_uris,
    GCancellable *cancellable,
    GError **error);

void tumbler_exported_cache_service_call_delete (
    TumblerExportedCacheService *proxy,
    const gchar *const *arg_uris,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean tumbler_exported_cache_service_call_delete_finish (
    TumblerExportedCacheService *proxy,
    GAsyncResult *res,
    GError **error);

gboolean tumbler_exported_cache_service_call_delete_sync (
    TumblerExportedCacheService *proxy,
    const gchar *const *arg_uris,
    GCancellable *cancellable,
    GError **error);

void tumbler_exported_cache_service_call_cleanup (
    TumblerExportedCacheService *proxy,
    const gchar *const *arg_base_uris,
    guint arg_since,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean tumbler_exported_cache_service_call_cleanup_finish (
    TumblerExportedCacheService *proxy,
    GAsyncResult *res,
    GError **error);

gboolean tumbler_exported_cache_service_call_cleanup_sync (
    TumblerExportedCacheService *proxy,
    const gchar *const *arg_base_uris,
    guint arg_since,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_PROXY (tumbler_exported_cache_service_proxy_get_type ())
#define TUMBLER_EXPORTED_CACHE_SERVICE_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_PROXY, TumblerExportedCacheServiceProxy))
#define TUMBLER_EXPORTED_CACHE_SERVICE_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_PROXY, TumblerExportedCacheServiceProxyClass))
#define TUMBLER_EXPORTED_CACHE_SERVICE_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_PROXY, TumblerExportedCacheServiceProxyClass))
#define TUMBLER_IS_EXPORTED_CACHE_SERVICE_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_PROXY))
#define TUMBLER_IS_EXPORTED_CACHE_SERVICE_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_PROXY))

typedef struct _TumblerExportedCacheServiceProxy TumblerExportedCacheServiceProxy;
typedef struct _TumblerExportedCacheServiceProxyClass TumblerExportedCacheServiceProxyClass;
typedef struct _TumblerExportedCacheServiceProxyPrivate TumblerExportedCacheServiceProxyPrivate;

struct _TumblerExportedCacheServiceProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  TumblerExportedCacheServiceProxyPrivate *priv;
};

struct _TumblerExportedCacheServiceProxyClass
{
  GDBusProxyClass parent_class;
};

GType tumbler_exported_cache_service_proxy_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (TumblerExportedCacheServiceProxy, g_object_unref)
#endif

void tumbler_exported_cache_service_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
TumblerExportedCacheService *tumbler_exported_cache_service_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
TumblerExportedCacheService *tumbler_exported_cache_service_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void tumbler_exported_cache_service_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
TumblerExportedCacheService *tumbler_exported_cache_service_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
TumblerExportedCacheService *tumbler_exported_cache_service_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_SKELETON (tumbler_exported_cache_service_skeleton_get_type ())
#define TUMBLER_EXPORTED_CACHE_SERVICE_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_SKELETON, TumblerExportedCacheServiceSkeleton))
#define TUMBLER_EXPORTED_CACHE_SERVICE_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_SKELETON, TumblerExportedCacheServiceSkeletonClass))
#define TUMBLER_EXPORTED_CACHE_SERVICE_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_SKELETON, TumblerExportedCacheServiceSkeletonClass))
#define TUMBLER_IS_EXPORTED_CACHE_SERVICE_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_SKELETON))
#define TUMBLER_IS_EXPORTED_CACHE_SERVICE_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), TUMBLER_TYPE_EXPORTED_CACHE_SERVICE_SKELETON))

typedef struct _TumblerExportedCacheServiceSkeleton TumblerExportedCacheServiceSkeleton;
typedef struct _TumblerExportedCacheServiceSkeletonClass TumblerExportedCacheServiceSkeletonClass;
typedef struct _TumblerExportedCacheServiceSkeletonPrivate TumblerExportedCacheServiceSkeletonPrivate;

struct _TumblerExportedCacheServiceSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  TumblerExportedCacheServiceSkeletonPrivate *priv;
};

struct _TumblerExportedCacheServiceSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType tumbler_exported_cache_service_skeleton_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (TumblerExportedCacheServiceSkeleton, g_object_unref)
#endif

TumblerExportedCacheService *tumbler_exported_cache_service_skeleton_new (void);


G_END_DECLS

#endif /* __TUMBLER_CACHE_SERVICE_GDBUS_H__ */

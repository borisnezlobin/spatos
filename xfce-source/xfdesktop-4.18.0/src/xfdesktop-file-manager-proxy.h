/*
 * This file is generated by gdbus-codegen, do not modify it.
 *
 * The license of this code is the same as for the D-Bus interface description
 * it was derived from. Note that it links to GLib, so must comply with the
 * LGPL linking clauses.
 */

#ifndef __XFDESKTOP_FILE_MANAGER_PROXY_H__
#define __XFDESKTOP_FILE_MANAGER_PROXY_H__

#include <gio/gio.h>

G_BEGIN_DECLS


/* ------------------------------------------------------------------------ */
/* Declarations for org.xfce.FileManager */

#define XFDESKTOP_TYPE_FILE_MANAGER (xfdesktop_file_manager_get_type ())
#define XFDESKTOP_FILE_MANAGER(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), XFDESKTOP_TYPE_FILE_MANAGER, XfdesktopFileManager))
#define XFDESKTOP_IS_FILE_MANAGER(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), XFDESKTOP_TYPE_FILE_MANAGER))
#define XFDESKTOP_FILE_MANAGER_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), XFDESKTOP_TYPE_FILE_MANAGER, XfdesktopFileManagerIface))

struct _XfdesktopFileManager;
typedef struct _XfdesktopFileManager XfdesktopFileManager;
typedef struct _XfdesktopFileManagerIface XfdesktopFileManagerIface;

struct _XfdesktopFileManagerIface
{
  GTypeInterface parent_iface;

  gboolean (*handle_copy_into) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *arg_target_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_copy_to) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *const *arg_target_filenames,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_create_file) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_parent_directory,
    const gchar *arg_content_type,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_create_file_from_template) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_parent_directory,
    const gchar *arg_template_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_display_application_chooser_dialog) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_uri,
    gboolean arg_open,
    gboolean arg_preselect_default_checkbox,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_display_file_properties) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_display_folder) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_display_folder_and_select) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_uri,
    const gchar *arg_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_display_preferences_dialog) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_execute) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_working_directory,
    const gchar *arg_uri,
    const gchar *const *arg_files,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_launch) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_launch_files) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_working_directory,
    const gchar *const *arg_filenames,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_link_into) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *arg_target_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_move_into) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *arg_target_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_rename_file) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id);

  gboolean (*handle_unlink_files) (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_working_directory,
    const gchar *const *arg_filenames,
    const gchar *arg_display,
    const gchar *arg_startup_id);

};

GType xfdesktop_file_manager_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *xfdesktop_file_manager_interface_info (void);
guint xfdesktop_file_manager_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void xfdesktop_file_manager_complete_display_application_chooser_dialog (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_display_folder (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_display_folder_and_select (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_display_file_properties (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_launch (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_execute (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_display_preferences_dialog (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_copy_to (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_copy_into (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_move_into (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_link_into (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_unlink_files (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_launch_files (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_rename_file (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_create_file (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);

void xfdesktop_file_manager_complete_create_file_from_template (
    XfdesktopFileManager *object,
    GDBusMethodInvocation *invocation);



/* D-Bus method calls: */
void xfdesktop_file_manager_call_display_application_chooser_dialog (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    gboolean arg_open,
    gboolean arg_preselect_default_checkbox,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_display_application_chooser_dialog_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_display_application_chooser_dialog_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    gboolean arg_open,
    gboolean arg_preselect_default_checkbox,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_display_folder (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_display_folder_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_display_folder_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_display_folder_and_select (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    const gchar *arg_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_display_folder_and_select_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_display_folder_and_select_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    const gchar *arg_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_display_file_properties (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_display_file_properties_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_display_file_properties_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_launch (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_launch_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_launch_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_execute (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *arg_uri,
    const gchar *const *arg_files,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_execute_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_execute_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *arg_uri,
    const gchar *const *arg_files,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_display_preferences_dialog (
    XfdesktopFileManager *proxy,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_display_preferences_dialog_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_display_preferences_dialog_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_copy_to (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *const *arg_target_filenames,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_copy_to_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_copy_to_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *const *arg_target_filenames,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_copy_into (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *arg_target_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_copy_into_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_copy_into_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *arg_target_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_move_into (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *arg_target_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_move_into_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_move_into_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *arg_target_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_link_into (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *arg_target_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_link_into_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_link_into_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_source_filenames,
    const gchar *arg_target_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_unlink_files (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_filenames,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_unlink_files_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_unlink_files_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_filenames,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_launch_files (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_filenames,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_launch_files_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_launch_files_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_working_directory,
    const gchar *const *arg_filenames,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_rename_file (
    XfdesktopFileManager *proxy,
    const gchar *arg_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_rename_file_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_rename_file_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_filename,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_create_file (
    XfdesktopFileManager *proxy,
    const gchar *arg_parent_directory,
    const gchar *arg_content_type,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_create_file_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_create_file_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_parent_directory,
    const gchar *arg_content_type,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);

void xfdesktop_file_manager_call_create_file_from_template (
    XfdesktopFileManager *proxy,
    const gchar *arg_parent_directory,
    const gchar *arg_template_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean xfdesktop_file_manager_call_create_file_from_template_finish (
    XfdesktopFileManager *proxy,
    GAsyncResult *res,
    GError **error);

gboolean xfdesktop_file_manager_call_create_file_from_template_sync (
    XfdesktopFileManager *proxy,
    const gchar *arg_parent_directory,
    const gchar *arg_template_uri,
    const gchar *arg_display,
    const gchar *arg_startup_id,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define XFDESKTOP_TYPE_FILE_MANAGER_PROXY (xfdesktop_file_manager_proxy_get_type ())
#define XFDESKTOP_FILE_MANAGER_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), XFDESKTOP_TYPE_FILE_MANAGER_PROXY, XfdesktopFileManagerProxy))
#define XFDESKTOP_FILE_MANAGER_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), XFDESKTOP_TYPE_FILE_MANAGER_PROXY, XfdesktopFileManagerProxyClass))
#define XFDESKTOP_FILE_MANAGER_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), XFDESKTOP_TYPE_FILE_MANAGER_PROXY, XfdesktopFileManagerProxyClass))
#define XFDESKTOP_IS_FILE_MANAGER_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), XFDESKTOP_TYPE_FILE_MANAGER_PROXY))
#define XFDESKTOP_IS_FILE_MANAGER_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), XFDESKTOP_TYPE_FILE_MANAGER_PROXY))

typedef struct _XfdesktopFileManagerProxy XfdesktopFileManagerProxy;
typedef struct _XfdesktopFileManagerProxyClass XfdesktopFileManagerProxyClass;
typedef struct _XfdesktopFileManagerProxyPrivate XfdesktopFileManagerProxyPrivate;

struct _XfdesktopFileManagerProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  XfdesktopFileManagerProxyPrivate *priv;
};

struct _XfdesktopFileManagerProxyClass
{
  GDBusProxyClass parent_class;
};

GType xfdesktop_file_manager_proxy_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (XfdesktopFileManagerProxy, g_object_unref)
#endif

void xfdesktop_file_manager_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
XfdesktopFileManager *xfdesktop_file_manager_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
XfdesktopFileManager *xfdesktop_file_manager_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void xfdesktop_file_manager_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
XfdesktopFileManager *xfdesktop_file_manager_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
XfdesktopFileManager *xfdesktop_file_manager_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define XFDESKTOP_TYPE_FILE_MANAGER_SKELETON (xfdesktop_file_manager_skeleton_get_type ())
#define XFDESKTOP_FILE_MANAGER_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), XFDESKTOP_TYPE_FILE_MANAGER_SKELETON, XfdesktopFileManagerSkeleton))
#define XFDESKTOP_FILE_MANAGER_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), XFDESKTOP_TYPE_FILE_MANAGER_SKELETON, XfdesktopFileManagerSkeletonClass))
#define XFDESKTOP_FILE_MANAGER_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), XFDESKTOP_TYPE_FILE_MANAGER_SKELETON, XfdesktopFileManagerSkeletonClass))
#define XFDESKTOP_IS_FILE_MANAGER_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), XFDESKTOP_TYPE_FILE_MANAGER_SKELETON))
#define XFDESKTOP_IS_FILE_MANAGER_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), XFDESKTOP_TYPE_FILE_MANAGER_SKELETON))

typedef struct _XfdesktopFileManagerSkeleton XfdesktopFileManagerSkeleton;
typedef struct _XfdesktopFileManagerSkeletonClass XfdesktopFileManagerSkeletonClass;
typedef struct _XfdesktopFileManagerSkeletonPrivate XfdesktopFileManagerSkeletonPrivate;

struct _XfdesktopFileManagerSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  XfdesktopFileManagerSkeletonPrivate *priv;
};

struct _XfdesktopFileManagerSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType xfdesktop_file_manager_skeleton_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (XfdesktopFileManagerSkeleton, g_object_unref)
#endif

XfdesktopFileManager *xfdesktop_file_manager_skeleton_new (void);


G_END_DECLS

#endif /* __XFDESKTOP_FILE_MANAGER_PROXY_H__ */
// SPDX-License-Identifier: MIT
// Copyright (c) 2011-2013, Christopher Jeffrey
// Copyright (c) 2013 Richard Grenville <pyxlcy@gmail.com>
// Copyright (c) 2018 Yuxuan Shui <yshuiv7@gmail.com>

#pragma once

/// Common functions and definitions for configuration parsing
/// Used for command line arguments and config files

#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <xcb/render.h>        // for xcb_render_fixed_t, XXX
#include <xcb/xcb.h>
#include <xcb/xfixes.h>

#include <libconfig.h>
#include <picom/types.h>

#include "compiler.h"
#include "log.h"
#include "utils/kernel.h"
#include "utils/list.h"
#include "wm/defs.h"

typedef struct session session_t;

/// @brief Possible backends
enum backend {
	BKEND_XRENDER,
	BKEND_GLX,
	BKEND_XR_GLX_HYBRID,
	BKEND_DUMMY,
	BKEND_EGL,
	NUM_BKEND,
};

typedef struct win_option_mask {
	bool shadow : 1;
	bool fade : 1;
	bool focus : 1;
	bool blur_background : 1;
	bool full_shadow : 1;
	bool redir_ignore : 1;
	bool opacity : 1;
	bool clip_shadow_above : 1;
} win_option_mask_t;

typedef struct win_option {
	bool shadow;
	bool fade;
	bool focus;
	bool blur_background;
	bool full_shadow;
	bool redir_ignore;
	double opacity;
	bool clip_shadow_above;
} win_option_t;

enum vblank_scheduler_type {
	/// X Present extension based vblank events
	VBLANK_SCHEDULER_PRESENT,
	/// GLX_SGI_video_sync based vblank events
	VBLANK_SCHEDULER_SGI_VIDEO_SYNC,
	/// An invalid scheduler, served as a scheduler count, and
	/// as a sentinel value.
	LAST_VBLANK_SCHEDULER,
};

enum animation_trigger {
	/// When a hidden window is shown
	ANIMATION_TRIGGER_SHOW = 0,
	/// When a window is hidden
	ANIMATION_TRIGGER_HIDE,
	/// When window opacity is increased
	ANIMATION_TRIGGER_INCREASE_OPACITY,
	/// When window opacity is decreased
	ANIMATION_TRIGGER_DECREASE_OPACITY,
	/// When a new window opens
	ANIMATION_TRIGGER_OPEN,
	/// When a window is closed
	ANIMATION_TRIGGER_CLOSE,
	/// When a window's geometry changes
	ANIMATION_TRIGGER_GEOMETRY,

	ANIMATION_TRIGGER_INVALID,
	ANIMATION_TRIGGER_COUNT = ANIMATION_TRIGGER_INVALID,
};

static const char *animation_trigger_names[] attr_unused = {
    [ANIMATION_TRIGGER_SHOW] = "show",
    [ANIMATION_TRIGGER_HIDE] = "hide",
    [ANIMATION_TRIGGER_INCREASE_OPACITY] = "increase-opacity",
    [ANIMATION_TRIGGER_DECREASE_OPACITY] = "decrease-opacity",
    [ANIMATION_TRIGGER_OPEN] = "open",
    [ANIMATION_TRIGGER_CLOSE] = "close",
    [ANIMATION_TRIGGER_GEOMETRY] = "geometry",
};

struct script;
struct win_script {
	/// A running animation can be configured to prevent other animations from
	/// starting.
	uint64_t suppressions;
	struct script *script;
	/// true if this script is generated by us, false if this is a user choice.
	int output_indices[NUM_OF_WIN_SCRIPT_OUTPUTS];
	bool is_generated;
};

extern const char *vblank_scheduler_str[];

/// Internal, private options for debugging and development use.
struct debug_options {
	/// Try to reduce frame latency by using vblank interval and render time
	/// estimates. Right now it's not working well across drivers.
	int smart_frame_pacing;
	/// Override the vblank scheduler chosen by the compositor.
	int force_vblank_scheduler;
	/// Release then immediately rebind every window pixmap each frame.
	/// Useful when being traced under apitrace, to force it to pick up
	/// updated contents. WARNING, extremely slow.
	int always_rebind_pixmap;
	/// When using damage, replaying an apitrace becomes non-deterministic, because
	/// the buffer age we got when we rendered will be different from the buffer age
	/// apitrace gets when it replays. When this option is enabled, we saves the
	/// contents of each rendered frame, and at the beginning of each render, we
	/// restore the content of the back buffer based on the buffer age we get,
	/// ensuring no matter what buffer age apitrace gets during replay, the result
	/// will be the same.
	int consistent_buffer_age;
};

extern struct debug_options global_debug_options;

struct included_config_file {
	char *path;
	struct list_node siblings;
};

enum window_unredir_option {
	/// This window should trigger unredirection if it meets certain conditions, and
	/// it should terminate unredirection otherwise. Termination of unredir is always
	/// suppressed if there is another window triggering unredirection, this is the
	/// same for `WINDOW_UNREDIR_TERMINATE` as well.
	///
	/// This is the default choice for windows.
	WINDOW_UNREDIR_WHEN_POSSIBLE_ELSE_TERMINATE,
	/// This window should trigger unredirection if it meets certain conditions.
	/// Otherwise it should have no effect on the compositor's redirection status.
	WINDOW_UNREDIR_WHEN_POSSIBLE,
	/// This window should always take the compositor out of unredirection, and never
	/// trigger unredirection.
	WINDOW_UNREDIR_TERMINATE,
	/// This window should not cause either redirection or unredirection.
	WINDOW_UNREDIR_PASSIVE,
	/// This window always trigger unredirection
	WINDOW_UNREDIR_FORCED,

	/// Sentinel value
	WINDOW_UNREDIR_INVALID,
};

struct window_maybe_options {
	/// Radius of rounded window corners, -1 means not set.
	int corner_radius;

	/// Window opacity, NaN means not set.
	double opacity;

	/// Window dim level, NaN means not set.
	double dim;

	/// The name of the custom fragment shader for this window. NULL means not set.
	const char *shader;

	/// Whether transparent clipping is excluded by the rules.
	enum tristate transparent_clipping;
	/// Whether a window has shadow.
	enum tristate shadow;
	/// Whether to invert window color.
	enum tristate invert_color;
	/// Whether to blur window background.
	enum tristate blur_background;
	/// Whether this window should fade.
	enum tristate fade;
	/// Do not paint shadow over this window.
	enum tristate clip_shadow_above;
	/// Whether the window is painted.
	enum tristate paint;
	/// Whether this window should be considered for unredirect-if-possible.
	enum window_unredir_option unredir;
	/// Whether shadow should be rendered beneath this window.
	enum tristate full_shadow;

	/// Window specific animations
	struct win_script animations[ANIMATION_TRIGGER_COUNT];
};

// Make sure `window_options` has no implicit padding.
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wpadded"
/// Like `window_maybe_options`, but all fields are guaranteed to be set.
struct window_options {
	double opacity;
	double dim;
	const char *shader;
	unsigned int corner_radius;
	enum window_unredir_option unredir;
	bool transparent_clipping;
	bool shadow;
	bool invert_color;
	bool blur_background;
	bool fade;
	bool clip_shadow_above;
	bool paint;
	bool full_shadow;

	struct win_script animations[ANIMATION_TRIGGER_COUNT];
};
#pragma GCC diagnostic pop

static inline bool
win_options_no_damage(const struct window_options *a, const struct window_options *b) {
	// Animation changing does not immediately change how window is rendered, so
	// they don't cause damage.
	return memcmp(a, b, offsetof(struct window_options, animations)) == 0;
}

/// Structure representing all options.
typedef struct options {
	// === Config ===
	/// Path to the config file
	char *config_file_path;
	/// List of config files included by the main config file
	struct list_node included_config_files;
	// === Debugging ===
	bool monitor_repaint;
	bool print_diagnostics;
	/// Render to a separate window instead of taking over the screen
	bool debug_mode;
	/// For picom-inspect only, dump windows in a loop
	bool inspect_monitor;
	xcb_window_t inspect_win;
	// === General ===
	/// Use the legacy backends?
	bool use_legacy_backends;
	/// Path to write PID to.
	char *write_pid_path;
	/// Name of the backend
	struct backend_info *backend;
	/// The backend in use (for legacy backends).
	int legacy_backend;
	/// Log level.
	int log_level;
	/// Whether to sync X drawing with X Sync fence to avoid certain delay
	/// issues with GLX backend.
	bool xrender_sync_fence;
	/// Whether to avoid using stencil buffer under GLX backend. Might be
	/// unsafe.
	bool glx_no_stencil;
	/// Whether to avoid rebinding pixmap on window damage.
	bool glx_no_rebind_pixmap;
	/// Custom fragment shader for painting windows, as a string.
	char *glx_fshader_win_str;
	/// Whether to detect rounded corners.
	bool detect_rounded_corners;
	/// Force painting of window content with blending.
	bool force_win_blend;
	/// Resize damage for a specific number of pixels.
	int resize_damage;
	/// Whether to unredirect all windows if a full-screen opaque window
	/// is detected.
	bool unredir_if_possible;
	/// List of conditions of windows to ignore as a full-screen window
	/// when determining if a window could be unredirected.
	struct list_node unredir_if_possible_blacklist;
	/// Delay before unredirecting screen, in milliseconds.
	int unredir_if_possible_delay;
	/// Forced redirection setting through D-Bus.
	switch_t redirected_force;
	/// Whether to stop painting. Controlled through D-Bus.
	switch_t stoppaint_force;
	/// Whether to enable D-Bus support.
	bool dbus;
	/// Path to log file.
	char *logpath;
	/// Number of cycles to paint in benchmark mode. 0 for disabled.
	int benchmark;
	/// Window to constantly repaint in benchmark mode. 0 for full-screen.
	xcb_window_t benchmark_wid;
	/// A list of conditions of windows not to paint.
	struct list_node paint_blacklist;
	/// Whether to show all X errors.
	bool show_all_xerrors;
	/// Whether to avoid acquiring X Selection.
	bool no_x_selection;
	/// Window type option override.
	win_option_t wintype_option[NUM_WINTYPES];
	struct win_option_mask wintype_option_mask[NUM_WINTYPES];
	/// Whether to set realtime scheduling policy for the compositor process.
	bool use_realtime_scheduling;

	// === VSync & software optimization ===
	/// VSync method to use;
	bool vsync;
	/// Whether to use glFinish() instead of glFlush() for (possibly) better
	/// VSync yet probably higher CPU usage.
	bool vsync_use_glfinish;
	/// Whether use damage information to help limit the area to paint
	bool use_damage;
	/// Disable frame pacing
	bool frame_pacing;

	// === Shadow ===
	/// Red, green and blue tone of the shadow.
	double shadow_red, shadow_green, shadow_blue;
	int shadow_radius;
	int shadow_offset_x, shadow_offset_y;
	double shadow_opacity;
	/// Shadow blacklist. A linked list of conditions.
	struct list_node shadow_blacklist;
	/// Whether bounding-shaped window should be ignored.
	bool shadow_ignore_shaped;
	/// Whether to crop shadow to the very X RandR monitor.
	bool crop_shadow_to_monitor;
	/// Don't draw shadow over these windows. A linked list of conditions.
	struct list_node shadow_clip_list;
	bool shadow_enable;

	// === Fading ===
	/// How much to fade in in a single fading step.
	double fade_in_step;
	/// How much to fade out in a single fading step.
	double fade_out_step;
	/// Fading time delta. In milliseconds.
	int fade_delta;
	/// Whether to disable fading on window open/close.
	bool no_fading_openclose;
	/// Whether to disable fading on ARGB managed destroyed windows.
	bool no_fading_destroyed_argb;
	/// Fading blacklist. A linked list of conditions.
	struct list_node fade_blacklist;
	bool fading_enable;

	// === Opacity ===
	/// Default opacity for inactive windows.
	/// 32-bit integer with the format of _NET_WM_WINDOW_OPACITY.
	double inactive_opacity;
	/// Default opacity for inactive windows.
	double active_opacity;
	/// Whether inactive_opacity overrides the opacity set by window
	/// attributes.
	bool inactive_opacity_override;
	/// Frame opacity. Relative to window opacity, also affects shadow
	/// opacity.
	double frame_opacity;
	/// Whether to detect _NET_WM_WINDOW_OPACITY on client windows. Used on window
	/// managers that don't pass _NET_WM_WINDOW_OPACITY to frame windows.
	bool detect_client_opacity;

	// === Other window processing ===
	/// Blur method for background of semi-transparent windows
	enum blur_method blur_method;
	// Size of the blur kernel
	int blur_radius;
	// Standard deviation for the gaussian blur
	double blur_deviation;
	// Strength of the dual_kawase blur
	int blur_strength;
	/// Whether to blur background when the window frame is not opaque.
	/// Implies blur_background.
	bool blur_background_frame;
	/// Whether to use fixed blur strength instead of adjusting according
	/// to window opacity.
	bool blur_background_fixed;
	/// Background blur blacklist. A linked list of conditions.
	struct list_node blur_background_blacklist;
	/// Blur convolution kernel.
	struct conv **blur_kerns;
	/// Number of convolution kernels
	int blur_kernel_count;
	/// Custom fragment shader for painting windows
	char *window_shader_fg;
	/// Rules to change custom fragment shader for painting windows.
	struct list_node window_shader_fg_rules;
	/// How much to dim an inactive window. 0.0 - 1.0, 0 to disable.
	double inactive_dim;
	/// Whether to use fixed inactive dim opacity, instead of deciding
	/// based on window opacity.
	bool inactive_dim_fixed;
	/// Conditions of windows to have inverted colors.
	struct list_node invert_color_list;
	/// Rules to change window opacity.
	struct list_node opacity_rules;
	/// Limit window brightness
	double max_brightness;
	// Radius of rounded window corners
	int corner_radius;
	/// Rounded corners blacklist. A linked list of conditions.
	struct list_node rounded_corners_blacklist;
	/// Rounded corner rules. A linked list of conditions.
	struct list_node corner_radius_rules;

	// === Focus related ===
	/// Whether to try to detect WM windows and mark them as focused.
	bool mark_wmwin_focused;
	/// Whether to mark override-redirect windows as focused.
	bool mark_ovredir_focused;
	/// Whether to use EWMH _NET_ACTIVE_WINDOW to find active window.
	bool use_ewmh_active_win;
	/// A list of windows always to be considered focused.
	struct list_node focus_blacklist;
	/// Whether to do window grouping with <code>WM_TRANSIENT_FOR</code>.
	bool detect_transient;
	/// Whether to do window grouping with <code>WM_CLIENT_LEADER</code>.
	bool detect_client_leader;

	// === Calculated ===
	/// Whether we need to track window leaders.
	bool track_leader;

	// Don't use EWMH to detect fullscreen applications
	bool no_ewmh_fullscreen;

	// Make transparent windows clip other windows, instead of blending on top of
	// them
	bool transparent_clipping;
	/// A list of conditions of windows to which transparent clipping
	/// should not apply
	struct list_node transparent_clipping_blacklist;

	bool dithered_present;
	// === Animation ===
	struct win_script animations[ANIMATION_TRIGGER_COUNT];
	/// Array of all the scripts used in `animations`. This is a dynarr.
	struct script **all_scripts;

	struct list_node rules;
	bool has_both_style_of_rules;
} options_t;

extern const char *const BACKEND_STRS[NUM_BKEND + 1];

bool load_plugin(const char *name, const char *include_dir);

bool must_use parse_long(const char *, long *);
bool must_use parse_int(const char *, int *);
struct conv **must_use parse_blur_kern_lst(const char *, int *count);
/// Parse the path prefix of a c2 rule. Then look for the specified file in the
/// given include directories. The include directories are passed via `user_data`.
void *parse_window_shader_prefix(const char *src, const char **end, void *user_data);
/// Same as `parse_window_shader_prefix`, but the path is relative to the current
/// working directory. `user_data` is ignored.
void *parse_window_shader_prefix_with_cwd(const char *src, const char **end, void *);
void *parse_numeric_prefix(const char *src, const char **end, void *user_data);
char *must_use locate_auxiliary_file(const char *scope, const char *path,
                                     const char *include_dir);
int must_use parse_blur_method(const char *src);
void parse_debug_options(struct debug_options *);

const char *xdg_config_home(void);
char **xdg_config_dirs(void);

/// Parse a configuration file
/// Returns the actually config_file name used, allocated on heap
/// Outputs:
///   shadow_enable = whether shadow is enabled globally
///   fading_enable = whether fading is enabled globally
///   win_option_mask = whether option overrides for specific window type is set for given
///                     options
///   hasneg = whether the convolution kernel has negative values
bool parse_config_libconfig(options_t *, const char *config_file);

/// Parse a configuration file is that is enabled, also initialize the winopt_mask with
/// default values
/// Outputs and returns:
///   same as parse_config_libconfig
bool parse_config(options_t *, const char *config_file);

/**
 * Parse a backend option argument.
 */
static inline attr_pure int parse_backend(const char *str) {
	for (int i = 0; BACKEND_STRS[i]; ++i) {
		if (strcasecmp(str, BACKEND_STRS[i]) == 0) {
			return i;
		}
	}
	// Keep compatibility with an old revision containing a spelling mistake...
	if (strcasecmp(str, "xr_glx_hybird") == 0) {
		log_warn("backend xr_glx_hybird should be xr_glx_hybrid, the misspelt "
		         "version will be removed soon.");
		return BKEND_XR_GLX_HYBRID;
	}
	// cju wants to use dashes
	if (strcasecmp(str, "xr-glx-hybrid") == 0) {
		log_warn("backend xr-glx-hybrid should be xr_glx_hybrid, the alternative "
		         "version will be removed soon.");
		return BKEND_XR_GLX_HYBRID;
	}
	return NUM_BKEND;
}

/**
 * Parse a VSync option argument.
 */
static inline bool parse_vsync(const char *str) {
	if (strcmp(str, "no") == 0 || strcmp(str, "none") == 0 ||
	    strcmp(str, "false") == 0 || strcmp(str, "nah") == 0) {
		return false;
	}
	return true;
}

/// Generate animation script for legacy fading options
void generate_fading_config(struct options *opt);

static inline void log_warn_both_style_of_rules(const char *option_name) {
	log_warn("Option \"%s\" is set along with \"rules\". \"rules\" will take "
	         "precedence, and \"%s\" will have no effect.",
	         option_name, option_name);
}
enum animation_trigger parse_animation_trigger(const char *trigger);

// vim: set noet sw=8 ts=8 :
dnl
dnl This file was autogenerated from "configure.ac.in".
dnl Edit that file instead!
dnl

dnl
dnl Copyright (c) 2003-2022
dnl         The Xfce development team. All rights reserved.
dnl
dnl Written for Xfce by Benedikt Meurer <benny@xfce.org>.
dnl

dnl Version information
m4_define([xfsm_version_major], [4])
m4_define([xfsm_version_minor], [18])
m4_define([xfsm_version_micro], [0])
m4_define([xfsm_version_nano], []) dnl leave this empty to have no nano version
m4_define([xfsm_version_build], [9eab2c80])
m4_define([xfsm_version_tag], [])
m4_define([xfsm_version], [xfsm_version_major().xfsm_version_minor().xfsm_version_micro()ifelse(xfsm_version_nano(), [], [], [.xfsm_version_nano()])ifelse(xfsm_version_tag(), [git], [xfsm_version_tag()-xfsm_version_build()], [xfsm_version_tag()])])

m4_define([xfsm_debug_default], [ifelse(xfsm_version_tag(), [git], [yes], [minimum])])
m4_define([intltool_minimum_version], [0.35])

dnl Initialize autoconf
AC_COPYRIGHT([Copyright (c) 2003-2022
        The Xfce development team. All rights reserved.

Written for Xfce by Benedikt Meurer <benny@xfce.org>.])
AC_INIT([xfce4-session], [xfsm_version], [https://bugzilla.xfce.org/])
AC_PREREQ([2.60])
AC_REVISION([$Id$])
AC_PROG_MAKE_SET()

dnl Initialize automake
AM_INIT_AUTOMAKE([1.11 dist-bzip2 tar-ustar no-dist-gzip foreign])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AM_MAINTAINER_MODE()
AM_SILENT_RULES([yes])

dnl Set helper path prefix
AC_ARG_WITH([helper-path-prefix],
            [AC_HELP_STRING([--with-helper-path-prefix=PATH],
                            [Path prefix under which helper executables will be installed (default: $libdir)])],
            [HELPER_PATH_PREFIX="$withval"],
            [HELPER_PATH_PREFIX="$libdir"])
AC_SUBST([HELPER_PATH_PREFIX])

dnl check for UNIX variants
AC_USE_SYSTEM_EXTENSIONS
AM_CONDITIONAL([HAVE_OS_CYGWIN], [test x"`uname | grep \"CYGWIN\"`" != x""])
if test x"`uname | grep \"CYGWIN\"`" != x""; then
	AC_DEFINE([HAVE_OS_CYGWIN], [1], [Compiling under cygwin])
fi

dnl check for basic programs
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
IT_PROG_INTLTOOL([intltool_minimum_version], [no-xml])
m4_ifdef([AC_PROG_SED], [AC_PROG_SED], [AC_CHECK_PROG(SED, sed, sed)])

dnl check for libtool
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

dnl check for standard header files
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h fcntl.h \
                  netdb.h signal.h stdarg.h sys/resource.h \
                  sys/socket.h sys/time.h sys/wait.h sys/utsname.h time.h \
                  unistd.h math.h sys/types.h])
AC_CHECK_FUNCS([getaddrinfo gethostbyname gethostname setsid strdup])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

dnl Check for required libraries
AC_CHECK_LIBM
AC_SUBST(LIBM)

dnl Check for X11 installed
XDT_CHECK_LIBX11_REQUIRE()

dnl Check for session management support
XDT_CHECK_LIBSM()

dnl Check for _IceTransNoListen
ac_LIBS="$LIBS"
LIBS="$LIBS $LIBSM_LDFLAGS $LIBSM_LIBS"
AC_CHECK_FUNCS([_IceTransNoListen])
LIBS="$ac_LIBS"

dnl Check for i18n support
XDT_I18N([])

dnl Check for required packages
XDT_CHECK_PACKAGE([LIBXFCE4UTIL], [libxfce4util-1.0], [4.15.2])
XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-2], [4.15.1])
XDT_CHECK_PACKAGE([GTK], [gtk+-3.0], [3.24.0])
XDT_CHECK_PACKAGE([GIO], [gio-2.0], [2.66.0])
XDT_CHECK_PACKAGE([GMODULE], [gmodule-2.0], [2.66.0])
XDT_CHECK_PACKAGE([LIBWNCK], [libwnck-3.0], [3.10])
XDT_CHECK_PACKAGE([XFCONF], [libxfconf-0], [4.12.0])

dnl Check for Polkit/PolicyKit
XDT_CHECK_OPTIONAL_PACKAGE([POLKIT], [polkit-gobject-1], [0.102],
                           [polkit], [Polkit support])

AC_DEFINE(GLIB_VERSION_MIN_REQUIRED, GLIB_VERSION_2_66, [Ignore post 2.66 deprecations])
AC_DEFINE(GLIB_VERSION_MAX_ALLOWED, GLIB_VERSION_2_66, [Prevent post 2.66 APIs])

AC_DEFINE(GDK_VERSION_MIN_REQUIRED, GDK_VERSION_3_24, [Ignore post 3.24 deprecations])
AC_DEFINE(GDK_VERSION_MAX_ALLOWED, GDK_VERSION_3_24, [Prevent post 3.24 APIs])

dnl Check for debugging support
XDT_FEATURE_DEBUG([xfsm_debug_default])

dnl Check for linker optimizations
XDT_FEATURE_LINKER_OPTS()

dnl Check for iceauth
AC_PATH_PROG([ICEAUTH], [iceauth])
if test x"$ICEAUTH" != x""; then
  AC_DEFINE_UNQUOTED([ICEAUTH_CMD], ["$ICEAUTH"], [path to iceauth])
else
  AC_MSG_ERROR([iceauth missing, please check your X11 installation])
fi

dnl Find a location for the session desktop file
AC_MSG_CHECKING([what xsession-prefix to use])
AC_ARG_WITH([xsession-prefix],
            AC_HELP_STRING([--with-xsession-prefix=XSESSION_PREFIX], [What should be the prefix for the xsession .desktop file?]),
  [], [with_xsession_prefix="/usr"])
XSESSION_PREFIX=$with_xsession_prefix
AC_SUBST([XSESSION_PREFIX])
AC_MSG_RESULT([$with_xsession_prefix])

dnl Check for legacy session management
AC_ARG_ENABLE([legacy-sm],
AC_HELP_STRING([--enable-legacy-sm], [Enable X11R5 session management])
AC_HELP_STRING([--disable-legacy-sm], [Disable X11R5 session management]),
  [], [enable_legacy_sm=yes])
AC_MSG_CHECKING([whether to enable X11R5 session management])
if test x"$enable_legacy_sm" != x"yes"; then
  AC_MSG_RESULT([no])
else
  AC_DEFINE([LEGACY_SESSION_MANAGEMENT], [1], [Define for X11R5 sm compat])
  AC_MSG_RESULT([yes])
fi

dnl Compile time default choice of backend
AC_ARG_WITH([backend],
	    AS_HELP_STRING([--with-backend=<option>],
			   [Default backend to use linux, freebsd, openbsd]))
# default to a sane option
AC_CANONICAL_HOST
if test x$with_backend = x; then
	AS_CASE([$host],
		[*-linux*],   [with_backend=linux],
		[*-*freebsd*], [with_backend=freebsd],
		[*-*dragonfly*], [with_backend=freebsd],
		[*-openbsd*], [with_backend=openbsd])
fi
AC_DEFINE_UNQUOTED(BACKEND, "$with_backend", [backend])
AC_SUBST(BACKEND, "$with_backend")

if test x$with_backend = xlinux; then
    AC_DEFINE(BACKEND_TYPE_LINUX, 1, [Linux suspend/hibernate/hybrid-sleep backend])
fi
if test x$with_backend = xfreebsd; then
    AC_DEFINE(BACKEND_TYPE_FREEBSD, 1, [FreeBSD suspend/hibernate backend])
fi
if test x$with_backend = xopenbsd; then
    AC_DEFINE(BACKEND_TYPE_OPENBSD, 1, [OpenBSD suspend/hibernate backend])
fi

AC_CONFIG_FILES([
Makefile
doc/Makefile
icons/Makefile
icons/16x16/Makefile
icons/24x24/Makefile
icons/24x24-actions/Makefile
icons/32x32/Makefile
icons/48x48-actions/Makefile
icons/48x48/Makefile
icons/128x128/Makefile
icons/scalable/Makefile
libxfsm/Makefile
po/Makefile.in
settings/Makefile
scripts/Makefile
scripts/xinitrc.in
xfce4-session/Makefile
xfce4-session-logout/Makefile
xfsm-shutdown-helper/Makefile
])
AC_OUTPUT

dnl ***************************
dnl *** Print configuration ***
dnl ***************************
echo
echo "Build Configuration:"
echo
echo " * Debugging support:              $enable_debug"
if test x"$POLKIT_FOUND" = x"yes"; then
echo " * Polkit/PolicyKit support:       yes"
else
echo " * Polkit/PolicyKit support:       no"
fi
if test x"$enable_legacy_sm" = x"yes"; then
echo " * Legacy session management:      yes"
else
echo " * Legacy session management:      no"
fi
echo " * Backend:                        ${with_backend}"

echo

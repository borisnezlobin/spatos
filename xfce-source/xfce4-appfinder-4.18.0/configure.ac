dnl
dnl This file was autogenerated from "configure.ac.in".
dnl Edit that file instead!
dnl

dnl
dnl Copyright (c) 2008-2022 - The Xfce Development Team.
dnl

dnl ***************************
dnl *** Version information ***
dnl ***************************
m4_define([xfce4_appfinder_version_major], [4])
m4_define([xfce4_appfinder_version_minor], [18])
m4_define([xfce4_appfinder_version_micro], [0])
m4_define([xfce4_appfinder_version_nano], []) dnl leave this empty to have no nano version
m4_define([xfce4_appfinder_version_build], [02897cb])
m4_define([xfce4_appfinder_version_tag], [])
m4_define([xfce4_appfinder_version], [xfce4_appfinder_version_major().xfce4_appfinder_version_minor().xfce4_appfinder_version_micro()ifelse(xfce4_appfinder_version_nano(), [], [], [.xfce4_appfinder_version_nano()])ifelse(xfce4_appfinder_version_tag(), [git], [xfce4_appfinder_version_tag()-xfce4_appfinder_version_build()], [xfce4_appfinder_version_tag()])])

dnl *******************************************
dnl *** Debugging support for GIT  snapshots ***
dnl *******************************************
m4_define([appfinder_debug_default], [ifelse(xfce4_appfinder_version_tag(), [git], [yes], [minimum])])

dnl ***************************
dnl *** Initialize autoconf ***
dnl ***************************
AC_COPYRIGHT([Copyright (c) 2008-2022 The Xfce development team. All rights reserved.])
AC_INIT([xfce4-appfinder], [xfce4_appfinder_version], [https://gitlab.xfce.org/xfce/xfce4-appfinder/], [xfce4-appfinder])
AC_PREREQ([2.60])
AC_CONFIG_MACRO_DIRS([m4])

dnl ***************************
dnl *** Initialize automake ***
dnl ***************************
AM_INIT_AUTOMAKE([1.8 dist-bzip2 tar-ustar no-dist-gzip foreign])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE()
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl *******************************
dnl *** Check for UNIX variants ***
dnl *******************************
AC_AIX()
AC_ISC_POSIX()
AC_MINIX()

dnl ********************************
dnl *** Check for basic programs ***
dnl ********************************
AC_PROG_CC()
AM_PROG_CC_C_O()
AC_PROG_LD()
AC_PROG_INSTALL()
IT_PROG_INTLTOOL([0.35.0])

dnl **************************
dnl *** Initialize libtool ***
dnl **************************
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

dnl **********************************
dnl *** Check for standard headers ***
dnl **********************************
AC_HEADER_STDC()
AC_CHECK_HEADERS([stdlib.h string.h errno.h unistd.h])
AC_CHECK_FUNCS([daemon setsid])

dnl ******************************
dnl *** Check for i18n support ***
dnl ******************************
XDT_I18N([])

dnl ***********************************
dnl *** Check for required packages ***
dnl ***********************************
XDT_CHECK_PACKAGE([GLIB], [glib-2.0], [2.66.0])
XDT_CHECK_PACKAGE([GTHREAD], [gthread-2.0], [2.66.0])
XDT_CHECK_PACKAGE([GTK], [gtk+-3.0], [3.24.0])
XDT_CHECK_PACKAGE([GIO], [gio-2.0], [2.66.0])
XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-2], [4.14.0])
XDT_CHECK_PACKAGE([LIBXFCE4UTIL], [libxfce4util-1.0], [4.15.2])
XDT_CHECK_PACKAGE([GARCON], [garcon-1], [0.3.0])
XDT_CHECK_PACKAGE([XFCONF], [libxfconf-0], [4.14.0])

dnl ***********************************
dnl *** Check for debugging support ***
dnl ***********************************
XDT_FEATURE_DEBUG([appfinder_debug_default])

dnl **************************************
dnl *** Check for linker optimizations ***
dnl **************************************
XDT_FEATURE_LINKER_OPTS()

dnl *********************************
dnl *** Substitute platform flags ***
dnl *********************************
AC_MSG_CHECKING([PLATFORM_CPPFLAGS])
AC_MSG_RESULT([$PLATFORM_CPPFLAGS])
AC_SUBST([PLATFORM_CPPFLAGS])
AC_MSG_CHECKING([PLATFORM_CFLAGS])
AC_MSG_RESULT([$PLATFORM_CFLAGS])
AC_SUBST([PLATFORM_CFLAGS])
AC_MSG_CHECKING([PLATFORM_LDFLAGS])
AC_MSG_RESULT([$PLATFORM_LDFLAGS])
AC_SUBST([PLATFORM_LDFLAGS])

AC_OUTPUT([
Makefile
data/Makefile
data/icons/Makefile
data/icons/16x16/Makefile
data/icons/24x24/Makefile
data/icons/32x32/Makefile
data/icons/48x48/Makefile
data/icons/128x128/Makefile
data/icons/scalable/Makefile
src/Makefile
po/Makefile.in
])

dnl ***************************
dnl *** Print configuration ***
dnl ***************************
echo
echo "Build Configuration:"
echo
echo " * Toolkit:           GTK+-3"
echo " * Debugging Support: $enable_debug"
echo


dnl
dnl File:	configure.in
dnl Id:		$Id$
dnl Author:	Zbigniew Zagorski <zzbigg@o2.pl>
dnl
dnl
dnl Process this file with autoconf to produce a configure script.

dnl ---------------------------------------------------------------------------
dnl initialization
dnl ---------------------------------------------------------------------------

AC_PREREQ(2.59)
VERSIONFF=`cat $srcdir/VERSION`
AC_INIT(
    zcompat,
    0.1.23 ,
    [zzbigg@o2.pl])
AC_CONFIG_AUX_DIR(misc)
AC_CONFIG_SRCDIR([include/zcompat/zcompat.h])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_SYSTEM
AC_EXEEXT
AC_BAKEFILE([m4_include(autoconf_inc.m4)])

case "$target" in
*-cygwin|*-cygwin*)
	CYGWIN="yes";
	;;
*-mingw|*-mingw32)
	MINGW32="yes";
	GNUWIN="yes"
	;;
esac


AC_DEFINE_UNQUOTED(HAVE_ZPIO,1,[always have zpio library])
AC_DEFINE_UNQUOTED(HAVE_ZCOMPAT,1,[always have zcompat library])
AC_DEFINE_UNQUOTED(HAVE_ZTOOLS,1,[always have ztools library])
AC_DEFINE_UNQUOTED(HAVE_ZMOD,1,[always have zmod library])
AC_DEFINE_UNQUOTED(BUILD_ZCOMPAT,1,[tell other libraries that we are inside zcompat])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

AC_CHECK_PROGS(DBGEN, dbgen axl_mksgml src2docbook)
AC_PATH_PROG(XMLTO, xmlto)
# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([m], [floor])
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([wsock32], [accept])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(
    [arpa/inet.h fcntl.h limits.h malloc.h netdb.h netinet/in.h stddef.h stdlib.h \
    string.h sys/param.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STAT
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS(
    [atexit dup2 gethostbyname memmove memset mkdir modf \
    pow rmdir socket strcasecmp strchr strcspn strdup strerror \
    strncasecmp strrchr strstr strtol pthread_mutex_timedlock ])

if test "X$GNUWIN" != "Xno" ; then
    LIBS="$LIBS -lwsock32"
    AC_DEFINE_UNQUOTED(HAVE_CREATEFILE,1,[define if have CreateFile system call])
    AC_DEFINE_UNQUOTED(HAVE_LOADLIBRARY,1,[define if have LoadLibrary system call])
fi

CPPFLAGS="-DHAVE_CONFIG_H $CPPFLAGS"
LDFLAGS="$LDFLAGS $LIBS"
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 doc/docmake.sh
                 tests/Makefile])
AC_OUTPUT
cat << EOF
Configured zcompat $VERSION with options:

    Target                  $target

    CFLAGS                  $CFLAGS
    CPPFLAGS                $CPPFLAGS
    LDFLAGS                 $LDFLAGS
    Installation location   $prefix

Type
    \$make
to compile library, then type
    #make install
to install the zcompat.
EOF

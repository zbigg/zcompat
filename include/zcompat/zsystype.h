/*
* Header:	zsystype.h
*
* Id:		$Id: zsystype.h,v 1.5 2003/05/07 10:50:52 trurl Exp $
*
**********
* ChangeLog:
*
    version 1.0.4-4	30 Sept 2002

    change 5
	* fixed NAME_MAX problem on MINGW and CYGWIN
	* added detection od CYGWIN environment

    change 4
	* minor changes
	* added ZLONG_LONG, ZLONGEST_LONG
    change 3
	* added ZDLL_PRIVATE
	* added ZPATH_MAX, ZFILE_MAX
	* added Z_DIR_CURRENT, Z_DIR_CURRENT_T
	* added Z_DIR_SEPARATOR, Z_DIR_SEPARATOR_T
	* added Z_PATH_SEPARATOR, Z_PATH_SEPARATOR_T
    change 2
	* added detection of OS/2

**********
* Description:
*
    Tries to find system and architecture where program is compiled.

    Note: If you find that on ypur system it does not detect
	configuration and if you fix it , please post this modified
	code to me.

    Author Zbigniew Zagorski <zzbigg@o2.pl>

**********
* Copyright:
*
    This program is Copyright(c) Zbigniew Zagorski 1999-2001,
    All rights reserved, and is distributed as free software under the
    license in the file "LICENSE", which is included in the distribution.

    Part of code from systype.h from "UNIX Network Programming" by
    Richard Stevens.

**********
* Features:
*
 * define ZSYS_NET_SUPPORT
    - on unix, and win32 systems, if defined mean ( i think ) that BSD -
      sockets are available

 * ZLONG_LONG is defined if 64-bit integer is available

 * defines ZLONGEST_LONG
   as longest integer type on this system

 * define Z_PATH_MAX & Z_FILE_MAX
     as length of longest path/filename available on system,
     used as maximal length of buffer for path/filename.

 * defines Z_DIR_CURRENT
     "./" or ".\"
 * defines Z_PATH_SEPARATOR[_T]
     ':' on unices
     ';' on DOS-like systems
     version with _T at end is text constant "..." not '.'

 * defines Z_DIR_SEPARATOR[_T]
     to: '/' on unices
	 '\' on DOS and Windows

 * defined ZDLL_PRIVATE - if Dynamic Linked Libaries have
     own C Run-Time linking and private alloc functions.

     If defined, it means that you can't do malloc in foo.dll,
     return it to main program and free allocated object in
     your program bar.exe.
     For example on Windows: library have other allocation
	routines than program

 * defines    - depending type of system
    ZSYS_UNIX		all unices
    ZSYS_WIN		MS Windows 16 & 32 bit
    ZSYS_WIN16		MS Windows 16bit (3.1,3.11)
    ZSYS_WIN32		MS Windows 32bit (95,98,NT,2000)
    ZSYS_MSDOS		MS-DOS
    ZSYS_OS2		IBM OS2
    ZSYS_DOS		MS-DOS & other DOS-like systems
    ZSYS_BEOS		BeOS

 * defines    - distribution, developer , kind of operating system
    ZSYSD_LINUX
    ZSYSD_BSD		Berkeley System Distribution
    ZSYSD_FREEBSD	FreeBSD

			** those below are from R.S. "UNIX Network ..." **
    ZSYSD_PYRAMID	i don't know anything about this systems, thus i can't
    ZSYSD_XENIX 	guarantee this code good

 * defines ZSYST_BSD, ZSYST_SYSV, ZSYST_MSDOS, ZSYST_WIN
, ZSYST_OS2
    - type of operating system

 * define: ZDEV_xxx as current development environment
    ZDEV_GCC		GNU C Compiler, also egcs
    ZDEV_EGCS

    ZDEV_VC		MS Visual C++
    ZDEV_MSC

    ZDEV_BORLAND	Borland C++, Turbo C++
    ZDEV_BCC

    ZDEV_BCB		Borland C++ Builder (HOW TO DETECT ?)

    ZDEV_DJGPP		DJGPP, also sets ZDEV_GCC

    ZDEV_BEOS		BeOS - nothing more for this platform

    ZDEV_MINGW		Mingw32 - ......

    ZDEV_WATCOM 	Watcom C - ......

 ****** NOT READY
 * Imported and exported (DLL) defines are set according to enviroment:
    * Borland C++ 5.0+
    * Visual C++
   Set ZDYNAMIC if you build dynamic verion of library or compile link
   program with dynamic library. Set ZEXPORT_SYM if you build library.
 ****** NOT READY

*/

#if !defined(__zsystype_h_) || defined(ZSYSTYPE_REBUILD)
#define __zsystype_h_
#define INC_ZSYSTYPE
#define ZOS_TYPE_UNIX	0
#define ZOS_TYPE_WIN32	1
#define ZOS_TYPE_WIN16	2
#define ZOS_TYPE_DOS	3
#define ZOS_TYPE_OS2	4

#define ZOS_NAME_BSD		"BSD"
#define ZOS_NAME_BEOS		"BeOS"
#define ZOS_NAME_DOS		"MS-DOS"
#define ZOS_NAME_FREEBSD	"FreeBSD"
#define ZOS_NAME_LINUX		"Linux"
#define ZOS_NAME_OPENBSD	"OpenBSD"
#define ZOS_NAME_PYR		"Pyramid"
#define ZOS_NAME_SYSV		"System V"
#define ZOS_NAME_WIN		"win"
#define ZOS_NAME_XENIX		"Xenix"
#define ZOS_NAME_OS2		"IBM OS/2"

#define ZARCH_NAME_I386 	"i386"
#define ZARCH_NAME_MC68K	"MC68000"
#define ZARCH_NAME_VAX		"VAX"
#define ZARCH_NAME_ALPHA	"ALPHA"



#ifdef ZSYSTYPE_REBUILD
#	undef ZSYSTYPE_REBUILD

#	undef ZEXPORT
#	undef ZIMPORT
#	undef ZDYN_SPEC
#	undef ZLIB_VAR
#	undef ZLIB_FUNC
#	undef ZLIB_ARRAY
#	undef ZLIB_CMETHOD
#	undef ZLIB_FUNCPTR
#	undef ZAPI
#	undef ZFAR
#	undef ZFUNC
#endif

#ifdef _MSC_VER
#define ZDEV_MSC _MSC_VER
#define ZDEV_VC  _MSC_VER
#	define ZDEV_NAME    "MSC"
#endif

#ifdef __BORLANDC__
#define ZDEV_BORLAND __BORLANDC__
#define ZDEV_BCC     __BORLANDC__

#ifdef BCB /* TODO: how to detect Borland C++ Builder */
#define ZDEV_BCB
#define ZDEV_BUILDER
#define ZDEV_CPPBUILDER
#	    define ZDEV_NAME    "bcc-cppbuilder"
#	else /* BCB */
#	    define ZDEV_NAME    "bcc"
#	endif

#endif /* BORLAND */

#ifdef __WATCOMC__
#	define	ZDEV_WATCOMC __WATCOMC__
#	define	ZDEV_WATCOM __WATCOMC__
#	define ZDEV_NAME    "WatcomC"
#endif

#ifdef __BEOS__
#	define ZDEV_BEOS __BEOS__
#	define ZSYS_NAME ZSYS_BEOS
#	define ZSYS_BEOS
#endif

#ifdef __MINGW32__
#	define ZDEV_MINGW32 __MINGW32__
#	define ZDEV_MINGW __MINGW32__
#	define ZDEV_NAME    "gcc-mingw32"
#endif

#ifdef __CYGWIN__
#	define ZDEV_CYGWIN __CYGWIN__
#	define ZDEV_CYGWIN __CYGWIN__
#	define ZDEV_NAME    "gcc-cygwin"
#endif

#if defined  __DJGPP__ || defined DJGPP/* DJGPP - ALL */
#define ZDEV_DJGPP __DJGPP__
#	define ZSYSD_MICROSOFT
#	undef	WE_HAVE_XNPRINTF
#	define WE_HAVE_XNPRINTF 0
#	define ZLONG_LONG long long
#	define ZLONGEST_LONG ZLONG_LONG
#	define ZDEV_NAME    "gcc-djgpp"
#ifdef RSXNT			/* RSXNT */
#define ZDEV_RSXNT
#	    define ZDEV_NAME    "gcc-djgpp-rsxnt"
#ifndef ZSYS_WIN32
#	define ZSYS_WIN32
#	define ZSYS_WIN
#endif
#else				/* DJGPP */
#	define ZSYS_MSDOS
#	define ZSYS_NAME ZOS_NAME_DOS"-DJGPP"
#endif /* RSXNT*/
#endif /* DJGPP */

#ifdef __GNUC__
#	define ZDEV_GCC  __GNUC__
#	define ZDEV_EGCS __GNUC__
#	define ZLONG_LONG long long
#	ifndef ZDEV_NAME
#	    define ZDEV_NAME    "gcc"
#	endif
#endif

#if( defined unix || defined ZSYS_UNIX) && !defined(ZDEV_DJGPP)
/* all unixes without djgpp which defines 'unix'*/
/* unix defined */

#define ZSYS_UNIX

/* used in older programs */
#define WE_LOVE_UNIX 1

#define ZSYS_NET_SUPPORT

#undef	WE_HAVE_XNPRINTF
#define WE_HAVE_XNPRINTF 1

#ifdef linux			/* LINUX */

#	define ZSYSD_LINUX
#	define ZSYS_NAME ZOS_NAME_LINUX
#	define ZSYST_BSD
#	define ZSYST_SYSV

#else /* end of linux */

#ifdef vax			/* VAX*/
#	define ZSYSD_BSD
#	define ZSYST_BSD

#	define ZSYS_NAME ZOS_NAME_BSD

#	define ZARCH_VAX
#	define ZARCH_NAME ZARCH_NAME_VAX
#else /* end of vax */

#ifdef pyr			/* pyramid */

#	define ZSYSD_PYRAMID
#	define ZSYST_BSD
#	define ZSYS_NAME ZOS_NAME_PYRAMID

#else

#if defined(FREEBSD) || defined(__FreeBSD__)/* freebsd */
#	define ZSYSD_FREEBSD
#	define ZSYSD_BSD
#	define ZSYST_BSD
#	define ZSYS_NAME ZOS_NAME_FREEBSD

#endif /* end of FREEBSD */

#endif /* end of pyr */
#endif /* end of vax */
#endif /* end of linux */
#endif /* unix */


#if defined (i386 ) || defined (mi386)
#	define ZARCH_I386
#	define ZARCH_NAME ZARCH_NAME_I386
#else
#if defined( mc68k ) || defined( _M_MRX000 )
#	define ZARCH_MC68K
#	define ZARCH_NAME ZARCH_NAME_MC68K
#else
#if defined( alpha ) || defined( ALPHA ) || defined( _ALPHA_ )
#	define ZARCH_ALPHA
#	define ZARCH_NAME ZARCH_NAME_ALPHA
#else
#endif /* alpha */
#endif /* mc68k */
#endif /* i386	*/


#ifdef	M_XENIX 			/* xenix */
#	define ZSYS_UNIX
#	define ZSYSD_XENIX

#	define ZSYS_NAME ZOS_NAME_XENIX

#	define ZARCH_I386
#	define ZARCH_NAME ZARCH_NAME_I386
#endif

#if defined(OS2) || defined(__os2__) || defined(ZSYS_OS2)
#	define ZSYS_OS2 __os2__
#	define ZSYS_NAME ZOS_NAME_OS2
#	define ZARCH_I386
#	define ZARCH_NAME ZARCH_NAME_I386
#endif

#if( defined( MSDOS ) || defined( __MSDOS__ ) || defined (DOS) || defined( ZSYS_MSDOS)) && !defined (ZDEV_DJGPP)/* ms-dos */
#	define ZSYS_MSDOS
#	define ZSYS_DOS

#	define ZARCH_I386
#	define ZARCH_NAME ZARCH_NAME_I386

#	undef  ZSYS_NAME
#	define ZSYS_NAME ZOS_NAME_DOS

#	undef  WE_HAVE_XNPRINTF
#	define WE_HAVE_XNPRINTF 0

#	ifndef ZDEV_DJGPP
#		define ZFAR_SPEC __far
#	else
#		define ZFAR_SPEC
#	endif

#	define Z_FILE_MAX 9
#	define Z_PATH_MAX 80

#endif

					/* MS-Windows(R or TM) */
#if ( defined( _Windows ) || defined (__WIN32__) || defined(WIN32) || defined( _WIN32 ) || defined( ZSYS_WIN ))

/* used in older programs */
#define WE_LOVE_BILL 1
#define ZSYS_WIN

#undef	WE_HAVE_XNPRINTF

#define ZARCH_I386
#define ZARCH_NAME ZARCH_NAME_I386

#define ZDLL_PRIVATE

#undef	ZSYS_NAME

#if defined (__WIN32__) || defined ( WIN32 ) || defined( _WIN32 )
#	define ZSYS_NAME ZOS_NAME_WIN"32"
#	define ZSYS_WIN32
#	define ZSYS_NET_SUPPORT
#	define ZFAR_SPEC
#else
#	define ZSYS_NAME ZOS_NAME_WIN"16"
#	define ZSYS_WIN16
#	define ZFAR_SPEC __far
#endif

#ifdef ZDEV_BORLAND

#	ifdef ZDEV_BCB /* i dont' know how to detect C++ Builder which have vsnprintf */
#		define WE_HAVE_XNPRINTF 1
#		define ZLONG_LONG long long
#		define ZLONGEST_LONG ZLONG_LONG
#	else /* Borland C++ */
#		define WE_HAVE_XNPRINTF 0
#	endif

#	define ZEXPORT __declspec(dllexport)
#	define ZIMPORT __declspec(dllimport)

#	define ZCALL_SPEC __cdecl

#	if (ZDEV_BORLAND >= 0x500)
#		define ZLONG_LONG __int64
#	endif

#	define __attribute__(attr)
#endif

#ifdef ZDEV_VC /* Visual C++ */
#	define WE_HAVE_XNPRINTF 0

#	define ZEXPORT __declspec(dllexport)
#	define ZIMPORT __declspec(dllimport)

#	define ZCALL_SPEC __cdecl

#	define ZLONG_LONG __int64

#	define ZLONGEST_LONG ZLONG_LONG

#	define __attribute__(attr)
#endif

#if defined( ZDEV_GCC )
#	define ZEXPORT __attribute__ ((dllexport,cdecl))
#	define ZIMPORT __attribute__ ((dllimport,cdecl))

#	ifndef ZDYN_SPEC
#		ifdef ZLIB_DLLSRC
#			define ZDYN_SPEC ZEXPORT
#		else
#			define ZDYN_SPEC ZIMPORT
#		endif
#	endif

#	define ZFUNC(ret_t,name,params) 	ret_t name params ZDYN_SPEC
#	define ZLIB_FUNC(type,name,args)	type name args ZDYN_SPEC
#	define ZLIB_VAR(type,name)		extern type name ZDYN_SPEC
#	define ZLIB_ARRAY(type,name)		extern type name[] ZDYN_SPEC
#	define ZLIB_CMETHOD(type,name,args)	type (* name) args ZDYN_SPEC
#	define ZLIB_FUNCPTR(type,name,args)	type (* name) args ZDYN_SPEC

#endif /* ZDEV_GCC on Windows */

#endif /* WINDOWS */

#if defined(ZSYS_PM)
#    if defined (__WATCOMC__)
#        define ZEXPORT __declspec(dllexport)
        /*
           __declspec(dllimport) prepends __imp to imported symbols. We do NOT
           want that!
         */
#        define ZIMPORT
#    elif defined(__EMX__)
#        define ZEXPORT
#        define ZIMPORT
#    elif (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))
#        define ZEXPORT _Export
#        define ZIMPORT _Export
#    endif
#elif defined(ZSYS_MAC) || defined(ZSYS_COCOA)
#    ifdef __MWERKS__
#        define ZEXPORT __declspec(export)
#        define ZIMPORT __declspec(import)
#    endif
#endif

/* for other platforms/compilers we don't anything */
#ifndef ZEXPORT
#    define ZEXPORT
#    define ZIMPORT
#endif


#ifndef ZEXPORT
#   define ZEXPORT
#endif

#ifndef ZIMPORT
#   define ZIMPORT
#endif

#ifndef ZDYN_SPEC
#   ifdef ZLIB_DLLSRC
#	define ZDYN_SPEC ZEXPORT
#   else
#	define ZDYN_SPEC ZIMPORT
#   endif
#endif

#ifndef ZCALL_SPEC
#   define ZCALL_SPEC
#endif

#ifndef ZAPI
#   define ZAPI ZEXPORT
#endif

#ifndef ZUTIL_API
#   define ZUTIL_API ZAPI
#endif

#ifndef ZFUNC
#   define ZFUNC(ret_t,name,params)	ZDYN_SPEC ret_t ZCALL_SPEC name params
#endif

#ifndef ZLIB_FUNC
#   define ZLIB_FUNC(type,name,args)	ZDYN_SPEC type ZCALL_SPEC name args
#endif

#ifndef ZLIB_VAR
#   define ZLIB_VAR(type,name)		ZDYN_SPEC type name
#endif

#ifndef ZLIB_ARRAY
#   define ZLIB_ARRAY(type,name)		ZDYN_SPEC type name[]
#endif

#ifndef ZLIB_CMETHOD
#   define ZLIB_CMETHOD(type,name,args) ZDYN_SPEC type (* ZCALL_SPEC name) args
#endif

#ifndef ZLIB_FUNCPTR
#   define ZLIB_FUNCPTR(type,name,args) ZDYN_SPEC type (* ZCALL_SPEC name) args
#endif

#if defined(ZSYS_WIN) || defined(ZSYS_DOS) || defined(ZSYS_OS2) || defined(ZDEV_DJGPP)

#   define Z_DIR_SEPARATOR	'\\'
#   define Z_DIR_CURRENT	".\\"
#   define Z_PATH_SEPARATOR	';'

#   define Z_DIR_SEPARATOR_T 	"\\"
#   define Z_PATH_SEPARATOR_T	";"
#endif

#if defined( ZSYS_UNIX ) && !defined( ZDEV_DJGPP )
#   define Z_DIR_SEPARATOR	'/'
#   define Z_DIR_CURRENT	"./"
#   define Z_PATH_SEPARATOR 	':'

#   define Z_DIR_SEPARATOR_T 	"/"
#   define Z_PATH_SEPARATOR_T 	":"
#endif

#ifndef Z_DIR_SEPARATOR
#   define Z_DIR_SEPARATOR 	'/'
#endif

#ifndef Z_DIR_SEPARATOR_T
#   define Z_DIR_SEPARATOR_T 	"/"
#endif

#ifndef Z_DIR_CURRENT
#   define Z_DIR_CURRENT	"./"
#endif

#ifndef Z_PATH_SEPARATOR
#   define Z_PATH_SEPARATOR	':'
#endif

#ifndef Z_PATH_SEPARATOR_T
#   define Z_PATH_SEPARATOR_T	":"
#endif


#if !defined(Z_PATH_MAX) || !defined(Z_FILE_MAX)
#if !defined(PATH_MAX)	 || !defined(NAME_MAX)

#   if (defined(ZDEV_VC) || defined(ZDEV_BCC)) && !defined(_POSIX_)
#	define _POSIX_ 1
#	if !defined(_LIMITS_) && !defined(__LIMITS_H)
#		include <limits.h>
#	endif
#	undef _POSIX_
#   else
#	  include <limits.h>
#   endif
#   if defined (ZDEV_MINGW) || defined(ZDEV_CYGWIN)
#       define NAME_MAX    PATH_MAX
#   endif


#   ifndef Z_PATH_MAX
#	define Z_PATH_MAX	((PATH_MAX)+1)
#   endif

#   ifndef Z_FILE_MAX
#	define Z_FILE_MAX	((NAME_MAX)+1)
#   endif

#endif
#endif

#ifndef ZDEV_NAME
#   define ZDEV_NAME		"unknown"
#endif

#ifndef ZSYS_NAME
#   define ZSYS_NAME		"unknown"
#endif

#ifndef ZARCH_NAME
#   define ZARCH_NAME		"unknown"
#endif

#ifndef ZLONGEST_LONG
#   ifdef ZLONG_LONG
#	define ZLONGEST_LONG ZLONG_LONG
#   else
#	define ZLONGEST_LONG long
#   endif
#endif

#endif /* _zsystype_h_ */


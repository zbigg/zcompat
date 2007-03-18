/*
* File: 	zmod.h
*
* Project:	ZCompat Portable Modules
*
* Description:
*    Library providing abstract layer for Dynamic Link libraries
*    portable trough UNIX(dlopen) and Win32 systems.
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
* $Id$
*
    FOR DOCUMENTATION SEE zmod.c - implementation of ZMOD
*/
#ifndef __zmod_h_
#define __zmod_h_

#include "zcompat/zsystype.h"
#include "zcompat/zdefs.h"

#ifdef ZMOD_STATIC_LINK
#define ZMOD_PREFIX ZMOD_MODULE_NAME
#define ZMOD_STATIC static
#else
#define ZMOD_PREFIX "_"
#define ZMOD_STATIC static
#endif

#if defined ZSYS_WIN && ( !defined ON_WINDOWS_DLX )
/*************** WINDOWS */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef ZDEV_BORLAND
#define ZMOD_FUNC __stdcall
#define ZMOD_EXPORT ZMOD_FUNC __export
#define ZMOD_IMPORT ZMOD_FUNC __declspec(dllimport)
#define ZMOD_CALLBACK_FUNC(c,a,b) long ZMOD_EXPORT c##zmod_callback(int a,unsigned long b)
#define ZMOD_CALLBACK_FUNCNN(a,b) long ZMOD_EXPORT zmod_callback(int a,unsigned long b)
typedef long (* ZMOD_EXPORT zmod_callback_t)(int,unsigned long);

#elif defined ZDEV_VC
#define ZMOD_FUNC __stdcall
#define ZMOD_EXPORT ZMOD_FUNC __declspec(dllexport)
#define ZMOD_IMPORT ZMOD_FUNC __declspec(dllimport)
#define ZMOD_CALLBACK_FUNC(c,a,b) long ZMOD_FUNC c##zmod_callback(int a,unsigned long b)
#define ZMOD_CALLBACK_FUNCNN(a,b) long ZMOD_FUNC zmod_callback(int a,unsigned long b)
typedef long (* /*ZMOD_FUNC*/ zmod_callback_t)(int,unsigned long);

#else
#define ZMOD_FUNC
#define ZMOD_EXPORT ZMOD_FUNC
#define ZMOD_CALLBACK_FUNC(c,a,b) long c##zmod_callback(int a,unsigned long b)
#define ZMOD_CALLBACK_FUNCNN(a,b) long zmod_callback(int a,unsigned long b)
typedef long (* zmod_callback_t)(int,unsigned long);
#endif

#define ZMOD_DLL
/* DJGPP */
#elif(defined ZSYS_MSDOS && defined ZDEV_DJGPP) || (defined ZSYS_WIN && defined ON_WINDOWS_DLX )

#include "dlx.h"
#define ZMOD_DLX

#define ZMOD_FUNC
#define ZMOD_EXPORT ZMOD_FUNC

#elif defined ZSYS_UNIX
/* UNIX */
#define ZMOD_SO

#include <dlfcn.h>
#define ZMOD_FUNC
#define ZMOD_EXPORT ZMOD_FUNC
#endif

#ifndef ZMOD_FUNC
#define ZMOD_FUNC
#endif

#ifndef ZMOD_EXPORT
#define ZMOD_EXPORT ZMOD_FUNC
#endif

#ifndef ZMOD_CALLBACK_FUNC
#define ZMOD_CALLBACK_FUNC(c,a,b) long ZMOD_FUNC c##zmod_callback(int a,unsigned long b)
#define ZMOD_CALLBACK_FUNCNN(a,b) long ZMOD_FUNC zmod_callback(int a,unsigned long b)
typedef long (* zmod_callback_t)(int,unsigned long);
#endif

struct exp_sym_info {
	int	type;
	char*	name;
	void*	ptr;
};
struct imp_sym_info {
	char*	module;
	char*	name;
	void**	ptr;
};
typedef struct exp_sym_info exp_sym_info;
typedef struct imp_sym_info imp_sym_info;

struct mod_info {
    char*	name;
    char*	internal_name;
    int 	ver_major;
    int 	ver_minor;
    char*	note;
    exp_sym_info* exp;
    imp_sym_info* imp;
    char**	lib_imp;
};
typedef struct mod_info mod_info;

struct mod_rec {
    mod_info*		info;
    int 		exp_no;
    int 		ref;
    char*		file_name;
    zmod_callback_t	callback;
#if   defined ZMOD_DLL
    HANDLE	handle;
#elif defined ZMOD_DLX
    hdlx_t	handle;
#elif defined ZMOD_SO
    void*	handle;
#endif
};
typedef struct mod_rec mod_rec;

typedef  mod_rec* mod_t;

#ifdef __cplusplus
extern "C" {
#endif

extern	mod_t mod_global;
mod_t	zmod_creat(mod_info* module_info);
mod_t	zmod_load(const char* module_name);
int	zmod_free(mod_t module_handle);
int	zmod_enum(void (*enum_f)(mod_t));
const char* zmod_error(void);
char*	zmod_set_find_path(char* path);
char*	zmod_pop_find_path();
int	zmod_push_find_path(char* path);
mod_info* zmod_get_modinfo(mod_t module_handle);
void*	zmod_lockfunc(mod_t module_handle,const char* fname);
void*	zmod_lock_sym(const char* mod,const char* sym);
int	zmod_free_sym(const char* mod,const char* sym);
int	zmod_unlockfunc(mod_t module_handle,void* func);


#ifdef __cplusplus
}
#endif

#define ZMOD_BEGIN_USE_MODULE	static void __zmod_use_module(void) {

#ifdef  ZMOD_STATIC_LINK
#define ZMOD_USE_MODULE(mod) 			\
    {						\
	long x;					\
	extern long mod##zmod_callback(int,long);	\
	x = mod##zmod_callback(1,0);		\
	if( x ) 				\
	    zmod_creat((mod_info*)x);		\
    }
#else
#define ZMOD_USE_MODULE(a)
#endif
#define ZMOD_END_USE_MODULE      }

#define ZMOD_USE_MODULES()	__zmod_use_module()
/*******************************/
/****			   *****/
/****	     NEW	   *****/
/****			   *****/
/*******************************/
#ifdef __cplusplus
#define ZMOD_IMPLEMENT_MODULE_BEGIN extern "C" {  static long __zmod_callback(int,unsigned long);
#else
#define ZMOD_IMPLEMENT_MODULE_BEGIN static long __zmod_callback(int,unsigned long);
#endif

#ifdef ZMOD_STATIC_LINK
#define REAL_ZMOD_CALLBACK(name)	\
ZMOD_CALLBACK_FUNC(name,what,param) 	\
{					\
    return __zmod_callback(what,param);	\
}
#else
#define REAL_ZMOD_CALLBACK(name)	\
ZMOD_CALLBACK_FUNCNN(what,param) 	\
{					\
    return __zmod_callback(what,param);	\
}
#endif
#define ZMOD_INFO( internal_name, long_name, version_major, version_minor, description ) \
static mod_info* 	_zmod_program_info = NULL;\
static mod_info 	_zmod_module_info = {	\
	long_name,				\
	#internal_name,				\
	version_major,version_minor,		\
	description,				\
	_zmod_module_entries,	\
	0,					\
	_zmod_lib_imp		\
};						\
REAL_ZMOD_CALLBACK(internal_name);

#define ZMOD_NO_INIT	    static void (*_zmod_init)(void) = 0;
#define ZMOD_DEF_INIT(f)		     \
	static void (*_zmod_init)(void) = f;

#define ZMOD_DEINIT_FUNC ZMOD_PREFIX ## _zmod_deinit

#define ZMOD_NO_DEINIT	      static void (*_zmod_deinit)(void) = 0;
#define ZMOD_DEF_DEINIT(f)		     \
	static void (*_zmod_deinit)(void) = f;

#if defined  ZMOD_DLX
#	define ZMOD_LIB_IMP_BEGIN	    LIBLOADS_BEGIN
#	define ZMOD_LIB_IMP(name)		LIBLOAD(name)
#	define ZMOD_LIB_IMP_END	    LIBLOADS_END static char* _zmod_lib_imp[] = { (char*)0 };
#else /* !ZMOD_DLX */
#	define ZMOD_LIB_IMP_BEGIN	    static char* _zmod_lib_imp[] = {
#	define ZMOD_LIB_IMP(name)		#name,
#	define ZMOD_LIB_IMP_END	    		(char*)0 };
#endif
#define ZMOD_NO_LIB_IMP 	ZMOD_LIB_IMP_BEGIN ZMOD_LIB_IMP_END

#define ZMOD_ENTRIES_BEGIN	static exp_sym_info _zmod_module_entries[] = {
#define ZMOD_ENTRY(name)	    { 1, #name	,(void*)(name)	 },
#define ZMOD_ENTRY_A(name,symbol)   { 1, #name	,(void*)(symbol) },
#define ZMOD_ENTRIES_END	{ 0,(void*)0,0 } };
#define ZMOD_NO_ENTRIES 	ZMOD_ENTRIES_BEGIN ZMOD_ENTRIES_END

#if defined ZMOD_DLX
#	define ZMOD_EXPORTS_BEGIN	LIBEXPORT_BEGIN LIBENTRY(__zmod_callback)
#	define ZMOD_EXP(name)			LIBEXPORT(name)
#	define ZMOD_EXPORTS_END	    	LIBEXPORT_END
#else /* !DLX */
#if 0
#	define ZMOD_EXPORTS_BEGIN static exp_sym_info _zmod_module_exports[] = {
#	define ZMOD_EXP(name)		{ 0, #name  ,(void*)(name) ) },
#	define ZMOD_EXPORTS_END	    { 0,(void*)0,0 } };
#else
#	define ZMOD_EXPORTS_BEGIN
#	define ZMOD_EXP(name)
#	define ZMOD_EXPORTS_END
#endif
#endif
#define ZMOD_NO_EXPORTS 	ZMOD_EXPORTS_BEGIN ZMOD_EXPORTS_END

#ifdef __cplusplus
#define ZMOD_IMPLEMENT_MODULE_END_1 };
#else
#define ZMOD_IMPLEMENT_MODULE_END_1
#endif

#if defined ZMOD_DLX
#define ZMOD_IMPLEMENT_MODULE_END \
static long __zmod_callback(int what,unsigned long param)	\
{ \
    if( what == 1 ) { \
	_zmod_program_info = (mod_info*)param; \
	return (long)(&_zmod_module_info); \
    } \
    return 0; \
} \
lib_begin ( void ) \
{	if ( ap_process == AP_ALLOC ) { /* first call of library */ \
	  AP_EXPORTLIB(); \
	} else \
	if ( ap_process == AP_INIT ) { /* initialization */ \
	    if( _zmod_init != 0 ) 			\
		((void (*)(void))_zmod_init)(); \
	} else \
	if ( ap_process == AP_FREE ) { /* last call of library */ \
	   if( _zmod_deinit != 0 ) \
		((void (*)(void))_zmod_deinit)(); \
	}; \
} lib_end; \
ZMOD_IMPLEMENT_MODULE_END_1

/*
	   if( _zmod_deinit != 0 ) ((void (*)(void))_zmod_deinit)(); \
*/
#else
#define ZMOD_IMPLEMENT_MODULE_END \
static long __zmod_callback(int what,unsigned long param)	\
{ \
    if( what == 1 ) { \
	_zmod_program_info = (mod_info*)param; \
	if( _zmod_init != 0 ) 			\
	    ((void (*)(void))_zmod_init)(); \
	return (long)(& _zmod_module_info ); \
    } 		\
    if( what == 2 ) {	\
	   if( _zmod_deinit != 0 ) \
		((void (*)(void))_zmod_deinit)(); \
    } \
    return 0; \
} \
ZMOD_IMPLEMENT_MODULE_END_1
#endif


/*
ZMOD_IMPLEMENT_MODULE_BEGIN
    ZMOD_NO_INIT
    |
    ZMOD_DEF_INIT(funkcja_inicjujaca)

    ZMOD_NO_DEINIT
    |
    ZMOD_DEF_DEINIT(funkcja_konczaca)
-- funkcja init & deinit : void (*)(void) musi byc zadeklarowana wczesniej

    ZMOD_LIB_IMP_BEGIN
	ZMOD_LIB_IMP(allegro)
	ZMOD_LIB_IMP(libc)
	ZMOD_LIB_IMP(zpio)
	ZMOD_LIB_IMP(ztools)
    ZMOD_LIB_IMP_END

    ZMOD_EXPORTS_BEGIN
	ZMOD_ENTRY(main)
	ZMOD_EXP(load_jpg)
	ZMOD_EXP(save_jpg)
    ZMOD_EXPORTS_END
ZMOD_IMPLEMENT_MODULE_END

*/

#endif

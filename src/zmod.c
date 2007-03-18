/*
* File: 	zmod.c
*
* Id:           $Id: zmod.c,v 1.11 2003/06/27 19:04:35 trurl Exp $
*
* Project:	ZBIG Portable Modules
*
* Description:
*    Library providing abstract layer for Dynamic Link libraries
*    portable trough UNIX(dlopen) and Win32 systems.
*
*    In DJGPP(MS-DOS) environment library uses DLX dynamic loaded
*    executables taken from SEAL. See Thanks.
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
**************************************************************************
NOTE:
    If you don't have int access(const char*, int)

#define ZMOD_NO_ACCESS 

    so it will emaulate access with open
    or change function zmod_access at the bottom.


##
##	=Module ZMOD Portable Dynamic Module Loader
##
##	This library provides portable access to dynamic module
##	loading under Win32 and UNIX systems. It also provides
##	clear interface so it can be ported to any platform.
##
##	Currently it it well tested on 3 platforms: Win32, Linux
##	and FreeBSD. It's compatible with most popular compilers
##	for these platforms: GCC, MSC, BCC.
##
##	=Using modules
##	
##	This section show how to use load and use modules
##	prepared to use with ZMOD - called zmods.
##
##      ==Introduction
##
##	To use module you need link your program against <zcompat>
##	libary and use followig functions:
##	- zmod_load
##	- zmod_lock_sym
##	- zmod_set_find_path
##
##      ==Loading modules
##
##	In order to load module you must invoke <zmod_load> in such manner
##
##	| mod_t mod_handle;
##	| mod_handle = zmod_load("mymodule");
##	| if( !mod_handle )
##	|	show_and_handle_error( zmod_error() );
##
##	<mod_t> is pointer to a structure that contains information about loaded 
##	module. It's structure is explained in section <Obtaining information about module>.
##
##	Handle obtained by this call should be disposed by calling <zmod_free>.
##	Module name provided to <zmod_load> functions can be "bare" name of module
##	eg. without path or extension or full featured filename if it's known.
##	
##	in first case (as in example) ZMOD searches some directories for files
##	<mymodule.xxx> where are common module extensions for different platforms:
##	    - Win32	: exe, dll, mod
##	    - *nix	: so, mod
##
##	So if you build yor modules to have system specific extensions (a very reasonable
##	solution) then your program that loads modules can be "system" independent
##	for this one case - name of module file.
##
##	Next, you may question about directories that are searched for module file.
##	These are:
##	- ./		  : current directory
##	- path-stack	  : paths on path stack
##	- $ZMOD_LIBRARY_PATH : PATH-like list of search directories
##				on unices it's separated by : (colon) 
##				and on win32 by ; (semicolon)
##	- userpath	  : path set by zmod_set_find_path
##				in such manner as $ZMOD_LIBRARY_PATH
##
##	Path stack is a stack of directory names that can be changed
##	by <zmod_push_find_path> and <zmod_pop_find_path> initially it's
##	empty.
##	
##	==Resolving symbols
##
##	As we load dynamic link module to attach some custom data or code to
##	our program, there might be functions to get pointers to this data.
##	There is one function: <zmod_lock_sym>. To use you need to know name of export
##	in the module and pass it with module handle as follows:
##
##	| {
##	|     int (*our_function) (int,int);
##	|     int result;
##	|     our_function = 
##	|         (int (*)(int,int)) 
##	|              zmod_lock_sym(mod_handle,"the_function");
##	|     if( !our_function ) 
##	|         show_and_handle_error( zmod_error());
##	|     
##	|     result = our_function(2,5);
##	| }
##
##	The module and obtained symbol address don't need to be released in any way.
##
##	==Obtaining information about module
##
##	If you wan't to receive information about module, you can use <mod_info*>
##	that can be obtained by <zmod_get_modinfo> which points to structure with
##	following fields:
##
##	* char* <file_name> - full path-name of loaded module, not necessary absolute
##				path,
##	* XXX <handle>	    - handle to platform specific object returned
##				by os loader function eg. HANDLE on Win32
##				or void* on unices,
##	* mod_info* info    - pointer to structure containing info about loaded
##				module provided by this module. This pointer is
##				returned by <zmod_get_modinfo>. Fields are explained 
##				as follows.
##
##	
##      =Writing module
##
##      This section describes how to write and compile module so it can be used
##      by ZMod functions to load.
##      
##      ==Module source
##
##      Somewhre in code of module you should put code like following:
##
##      |ZMOD_IMPLEMENT_MODULE_BEGIN
##      |    ZMOD_DEF_INIT(module_init);
##      |    ZMOD_DEF_DEINIT(module_deinit);
##      |
##      |    ZMOD_NO_LIB_IMP
##      |    ZMOD_NO_EXPORTS
##      |    ZMOD_ENTRIES_BEGIN
##      |	ZMOD_ENTRY(create_SQLite_object)
##      |    ZMOD_ENTRIES_END
##      |    ZMOD_INFO(
##      |	sqlite,
##      |	"axlSQLite",
##      |	1,1,
##      |	"SQLite module for AXL"
##      |    )
##      |ZMOD_IMPLEMENT_MODULE_END
##
##      Description of macros:
##      * ZMOD_IMPLEMENT_MODULE_BEGIN - begin implementation of module
##                                      information (required)
##      * ZMOD_DEF_INIT(function)     - declare that this function
##                                      should be called after module loading
##                                      and
##                                      before any use of module
##      * ZMOD_NO_INIT                - inform that there is no initialization
##                                      function, use instead of ZMOD_DEF_INIT
##      * ZMOD_DEF_DEINIT, ZMOD_NO_DEINIT - same as above, deinit function is called
##                                      before module is unloaded that means
##                                      while xref:zmod-free call or while program
##                                      exit
##      * ZMOD_NO_LIB_IMP, ZMOD_NO_EXPORTS - these must be here for no good
##                                      reason
##      * ZMOD_ENTRIES_BEGIN/END      - begin/end list of symbols exported
##                                      from module. Use following macros
##                                      to export specific entities:
##              ** ZMOD_ENTRY(name)              - export symbol <name>
##              ** ZMOD_ENTRY_A(alias,name)      - export symbol <name> under
##                                                <alias> name
##      * ZMOD_INFO(id,"name",vmajor,vminor,"desc") -
##                                      define description of module ... no 
##                                      special reason for that but ... must
##                                      be there
##      * ZMOD_IMPLEMENT_MODULE_END -   finish implementation ... must be there
##
##      Second example:
##      EXAMPLE: PostreSQL module for AXL
##	|ZMOD_IMPLEMENT_MODULE_BEGIN
##	|    ZMOD_NO_INIT
##	|    ZMOD_NO_DEINIT
##	|
##	|    ZMOD_NO_LIB_IMP
##	|    ZMOD_NO_EXPORTS
##	|    ZMOD_ENTRIES_BEGIN
##	|    
##	|	ZMOD_ENTRY_A(create_postgres_object,create_postgres_object)
##	|	
##	|	ZMOD_ENTRY_A(postgres_connect,post_connect)
##	|	
##	|	ZMOD_ENTRY_A(connection_close,conn_close)
##	|	ZMOD_ENTRY_A(connection_exec,conn_exec)
##	|
##	|	ZMOD_ENTRY_A(result_fname,res_fname)
##	|	ZMOD_ENTRY_A(result_getvalue,res_getvalue)
##	|	ZMOD_ENTRY_A(result_getvalue_integer,res_getvalue_integer)
##	|	ZMOD_ENTRY_A(result_getvalue_float,res_getvalue_float)
##	|	ZMOD_ENTRY_A(result_close,res_close)
##	|
##	|    ZMOD_ENTRIES_END
##	|    ZMOD_INFO(
##	|	postgres,
##	|	"axlPostgreSQL",
##	|	1,0,
##	|	"PostgreSQL interface for AXL"
##	|    )
##	|ZMOD_IMPLEMENT_MODULE_END
##	
##      =Building module
##
##      First of all module should be compiled as shared library or DLL
##      according to platform you use.
##
##      Second, it MUST export <zmod_callback> function outside of DLL. This is
##      done if you use macros described in previous section.
##
##      ==Linking
##      To link <MODULE> in some environments use:      
##      * MODULE - is name of your module
##      * ...    - are normal object files, libraries and other like
##                 on normal link
##
##      ===Win32
##
##      To build shared library on win32 using Microsoft(R) C Compiler use:
##      |link /DLL /NOLOGO /OUT:MODULE.dll ...
##      To build shared library on win32 using MinGW or Cygwin gcc use:
##      |gcc --shared -o MODULE.dll ...
##
##      ===Linux
##
##      To build shared library on Linux/Unix use following command:
##      |gcc --shared -o MODULE.so ...
##
##      =Static link
##
##      If you're building  apllication which originally used ZMod
##      modules and have some already written modules, and want
##      to port i to environment which doesn't support dynamic code
##      loading you can use static link. You can use it if you wan't
##      statically link your module into program for any reason.
##
##      ==Preparing code
##      
##      If your modules are written using ZMOD_xxx macros you don't
##      havre do change your module code in any way.
##      
##      The only place you've got to change is your application
##      initialization code eg. somewhere at start of <main()> function.
##
##      For initialization of statically linked modules use 
##      ZMOD_BEGIN_USE_MODULE, ZMOD_USE_MODULE and 
##      ZMOD_USE_MODULES macros
##      as in following example:
##      
##      |#include <zcompat/zmod.h>
##      |
##      |ZMOD_BEGIN_USE_MODULE
##      |    ZMOD_USE_MODULE(allsnd)
##      |    ZMOD_USE_MODULE(allgui)
##      |ZMOD_END_USE_MODULE
##      |
##      |int main(int argc,char* argv[])
##      |{
##      |    int somevar;
##      |    ZMOD_USE_MODULES();
##      |    //...
##      |}
##
##      This code in declares <__zmod_use_module()> function which
##      loads modules using statically available functions such
##      as <allsndzmod_callback> and <allguizmod_callback> which
##      are declared in these modules. <ZMOD_USE_MODULES> calls
##      this function. It should be called anywhere but before first
##      use of these modules.
##
##      ==Static compilation
##
##      You must compile your <modules> and the one file which contains
##      ZMOD_USE_MODULES calls with <ZMOD_STATIC_LINK> definition !!!
##      
#################################################################
##
##  =Function zmod_creat
##	create a "virtual" module object.
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| mod_t	zmod_creat(mod_info* module_info);
##
##  =Description
##
##    Create a "virtual" module object.
##
##    Handle to new module is returned.
##
##    Handle should be disposed by zmod_free.
##
#################################################################
##
##  =Function zmod_load
##	load module from file.
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| mod_t	zmod_load(const char* module_name);
##
##  =Description
##
##    Load module from file.
##
##    The file_name can be absolute or relative path to module file in system.
##    It can be only file name (without path and extension). In that case zmod_load
##    searches for module in search directories. Search directories are :
##	- ./		  : current directory
##	- $ZMOD_LIBRARY_PATH : PATH-like list of search directories
##	- userpath	  : path set by zmod_set_find_path
##
##    It tries all of system specific extensions:
##	-	Win32: dll,exe
##	-	Unix : so,mod,drv
##	-	DJGPP: dlx,mod,drv
##
##  =Returns.
##
##    Handle to opened module is returned on success, els NULL is returned.
##
##
#################################################################
##
##
##  =Function zmod_free
##	free module handle
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| int	zmod_free(mod_t module_handle);
##
##  =Description
##
##    Dispose module handle, if it's the last module
##    handle module is unloaded.
##
##    Note that ZMOD Library doesn't unload previoulsy loaded modules.
##
##  =Returns
##	TODO: Findout what it returns.
##
#################################################################
##
##
##  =Function zmod_enum
##	enumerate existing modules
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| int	zmod_enum(void (*enum_f)(mod_t p));
##
##  =Description
##    Enumerate trough all loaded and virtual modules.
##
##    You should pass pointer to function declared:
##	|	void mod_enum_callback(mod_t p);
##    ; as the only parameter to _zmod_enum_.
##
##    <enum_f> is called with module handle for each module.
##
##    p will be pointer to struct <mod_rec> declared above.
##
##
##
#################################################################
##
##
##  =Function zmod_error
##	get error information
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| const char* zmod_error(void);
##
##  =Description
##    Return string descripting cause of last error in ZMOD Library.
##
#################################################################
##
##  =Function zmod_lockfunc
##	get address of exported function
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| void*	zmod_lockfunc(mod_t module_handle,const char* fname);
##
##  =Description
##	Get poiner to function exported by module pointed by
##	<module_handle>. <fname> points to string containing
##	name of requested symbol.
##
##  =Returns
##	- entry point of requested function
##	- NULL on error
##
#################################################################
##
##
##  =Function zmod_set_find_path
##	set module search path
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| char*	zmod_set_find_path(char* path);
##
##  =Description
##    Set search path and return the old one. If <path> is NULL only
##    current search path is returned.
##
##
#################################################################
##
##
##  =Function zmod_push_find_path
##	push search path
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| int	zmod_push_find_path(const char* path);
##
##  =Description
##
##    Push path (PATH-like string separated by ; or :) to
##    search stack. Returns 0 on success, -1 on error. The
##    only error is stack owerflow.
##
#################################################################
##
##  =Function zmod_pop_find_path
##	pop search path
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| char*	zmod_pop_find_path();
##
##  =Description
##
##	Returns top element of search stack and drops it from
##	the stack. Returns NULL when stack is already empty.
##
#################################################################
##
##  =Function zmod_lock_sym
##	get symbol address in module by symbol's name
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| void*	zmod_lock_sym(const char* mod,const char* sym);
##
##  =Description
##	Get poiner to function exported by module which name
##	is provided by <mod>. If the module isn't loaded yet,
##	it is loaded automaticaly . <fname> points to string containing
##	name of requested symbol.
##
##  =Returns
##	- entry point of requested function
##	- NULL on error
##
#################################################################
##  =Function zmod_free_sym
##	free symbol in module by symbol's name
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	|int	zmod_free_sym(const char* mod,const char* sym);
##
##  =Description
##	<TODO> UNDOCUMENTED FUNCTION
##
##
#################################################################
##
##  =Function zmod_get_modinfo
##	get module information
##
##  =Synopsis
##	| #include "zcompat/zmod.h"
##	|
##	| mod_info* zmod_get_modinfo(mod_t module_handle);
##
##  =Description
##	<TODO> UNDOCUMENTED FUNCTION
##
##
#################################################################

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "zcompat/zcompat.h"
#include "zcompat/zmod.h"
#include "zcompat/ztools.h"


#ifdef ZMOD_SO
#include <unistd.h>
#endif

#ifdef ZMOD_DLL
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <io.h>
#endif

#ifdef HAVE_ZPIO
#include "zcompat/zpio.h"
#endif

#ifndef HAVE_STRDUP
static char* strdup(const char* x)
{
    if( !x ) 
	return 0;
    else {
	int l = strlen(x);
	char* s = (char*)malloc(l+1);
	strcpy(s,x);
	return s;
    }
}
#endif

#if 0
ZMOD_DEFINE_EXPORT_TABLE(mod_zmod_exp)
	ZMOD_EXPFUNC1(zmod_creat),
	ZMOD_EXPFUNC1(zmod_load),
	ZMOD_EXPFUNC1(zmod_free),
	ZMOD_EXPFUNC1(zmod_enum),
	ZMOD_EXPFUNC1(zmod_lock_sym),
	ZMOD_EXPFUNC1(zmod_free_sym),
	ZMOD_EXPFUNC1(zmod_lockfunc),
	ZMOD_EXPFUNC1(zmod_unlockfunc),
ZMOD_END_EXPORT_TABLE;

ZMOD_DEFINE_IMPORT_TABLE(empty_imp)
ZMOD_END_IMPORT_TABLE;

mod_info mod_zmod_info = {
/* name 		*/ "zmod",
/* internal name	*/ "zmod",
/* ver major.minor	*/ 1,0,
/* note 		*/ "zmod manager",
/* export table 	*/ mod_zmod_exp,
/* import table 	*/ empty_imp
};
#else
long mod_zmod_info = 0;

#endif
mod_info* auto_modules[] = {
#if 0
    &mod_zmod_info,
#endif
    0
};

sbtree* 	modules = 0;

struct mod_find_info {
	char* name;
	char* org_name;
	int n;
};
typedef struct mod_find_info mod_find_info;

/* internal functions */

static int	init_modules_list(void);
static void	done_modules_list(void);

static long	zmod_call(mod_t mod, int func, long param);
static int	zmod_loaded(mod_t mod);

static mod_t	zmod_list_get(const char* name);
static int	zmod_list_add(const char* name,mod_t mod_h);
static mod_t	zmod_list_del(mod_t mod);

static void	zmod_sort_symbols(exp_sym_info* start,int size);

static int	zmod_do_load(mod_t mod,const char* filename);
static int	zmod_do_free(mod_t mod);

static int	zmod_get_imports(mod_t mod);
static int	zmod_free_imports(mod_t mod);

char*	zmod_set_find_path(char* path);
char	zmod_last_error[200] = "succes";

static int	zmod_find_file_by_paths(
    char*	path[],
    const char* file,
    int 	type,
    char*	buf,
    size_t	max);

static int	zmod_find_file_by_path(
    const char* path,
    const char* file,
    int 	type,
    char*	buf,
    size_t	max);

static int	zmod_path_is_abs(const char* path);
static int	zmod_file_has_ext(const char* name);
static int	zmod_file_set_ext(const char* name,const char* ext,char* buf,int max);
static int	zmod_find_file_name(const char* name,char* buf,int max);

#if !defined R_OK /* haven't got x_OK constants */
#if defined(ZDEV_BORLAND) | defined(ZDEV_MSC)
#define ZMR_OK	2
#define ZMW_OK	4
#define ZMX_OK	1
#define ZMF_OK	0
#else /* default for all machines */
#define ZMR_OK	2
#define ZMW_OK	4
#define ZMX_OK	1
#define ZMF_OK	0
#endif
#else /* have got x_OK constants */
#define ZMR_OK	R_OK
#define ZMW_OK	W_OK
#define ZMX_OK	X_OK
#define ZMF_OK	F_OK
#endif /* have got x_OK constants */
static int	zmod_access(const char* path, int type);

const char*	zmod_error(void)
{
    return zmod_last_error;
}

static void zmod_set_error(const char* a)
{
    strncpy(zmod_last_error,a ? a : "success",sizeof(zmod_last_error)-1);
}
static int	init_modules_list(void)
{
    int i = 0;
    if( modules == NULL ) {
	modules = sbtree_new();
    if( modules == NULL ) {
	    zmod_set_error("modules initialization failed");
	    perror(zmod_last_error);
	    _exit(1);
	    return -1;
	}
    }
    while( auto_modules[i] ) {
	zmod_creat(auto_modules[i++]);
    }
    zc_atexit(done_modules_list);
    return 0;
}
static void	done_modules_list(void)
{
    if( modules ) {
	static int	zmod_free_nl(mod_t mod);
	SBTI_START_ENUM(modules,i,k,mod_t) {
	    zmod_free_nl(i);
	} SBTI_STOP_ENUM
	sbtree_delete(modules);
    }
}

static long zmod_call(mod_t mod, int func, long param)
{
    if( mod->callback != NULL ) {
#if defined( ZDEV_BORLAND ) && ZDEV_BORLANDC > 0x410
	   long ( __import __stdcall * f) (int,long) = mod->callback;
#elif defined ZDEV_VC && ( !defined ON_WINDOWS_DLX )
	   long ( __stdcall * f) (int,long) = (long ( __stdcall *) (int,long))mod->callback;
#else
	   zmod_callback_t f = mod->callback;
#endif
	return f(func,param);
    }
    return 0;
}
static mod_t zmod_list_get(const char* name)
{
    if( !modules )
	init_modules_list();
    return (mod_t)sbtree_find(modules,name);
}
static int	  zmod_list_add(const char* name,mod_t mod_h)
{
    if( ! modules )
	init_modules_list();
    return sbtree_add(modules,name,(void*)mod_h);
}
static mod_t		zmod_list_del(mod_t mod)
{
    if( modules )
	return (mod_t)sbtree_del(modules,mod->info->internal_name);
    return NULL;
}
mod_t zmod_creat(mod_info* info)
{
    mod_t mod;
    mod = zmod_list_get(info->internal_name);
    if( mod ) return 0;
    mod = (mod_t)calloc(1,sizeof(mod_rec));
    if( !mod ) {
	zmod_set_error(strerror(errno));
	return 0;
    }
    mod->file_name = strdup("internal");
    mod->ref = 1;
    mod->info = info;
    mod->callback = (zmod_callback_t)0;
#ifdef ZMOD_DLL
#if 1
    mod->handle = 0;
#else
    mod->handle = GetModuleHandle(0);
#endif
#elif defined ZMOD_SO || defined ZMOD_DLX
    mod->handle = 0;
#endif
    zmod_list_add(mod->info->internal_name,mod);

    if( mod->info->exp != NULL ) {
	int i=0;
	while( mod->info->exp[i].name != NULL ) i++;
	if( i > 0 )
	    zmod_sort_symbols(mod->info->exp,i);
	mod->exp_no = i;
    } else
	mod->exp_no = 0;
    return mod;
}


#ifdef ZMOD_DLX
#include "exp/zmod.exp" /* zmod_export_table */

extern char* libc_export_table[];
static int dlx_exports_loaded = 0;
#endif
/** Load module from specified file.
*/
static int zmod_do_load(mod_t mod,const char* filename)
{
#ifdef ZMOD_DLL
    mod->handle = LoadLibrary(filename);
    if( mod->handle ) {
	mod->file_name = (char*)calloc(256,1);
	GetModuleFileName(mod->handle,mod->file_name,255);
	(void*) mod->callback = /*(long(ZMOD_FUNC *)(int,long))*/ (void*)(GetProcAddress(mod->handle,"zmod_callback"));
	if( !mod->callback ) {
	    zmod_set_error("DLL: no zmod_callback export in module");
	    FreeLibrary(mod->handle);
	    return 0;
	}
	return 1;
    }
    return 0;
#elif defined ZMOD_DLX
    if( dlx_exports_loaded == 0 ) {
	DLXImport(zmod_export_table);
#ifdef ZDEV_DJGPP
	DLXImport(libc_export_table);
#endif
	dlx_exports_loaded = 1;
    }
    mod->handle = DLXLoad((char*)filename,NULL);
    if( mod->handle != 0 ) {
	mod->file_name = strdup(filename);
	mod->callback = (zmod_callback_t)
		      ( DLXGetEntry(mod->handle,"zmod_callback"));
	if( mod->callback == NULL ) {
	    zmod_set_error("DLX: no zmod_callback entry in module");
	    DLXUnload(mod->handle);
	    mod->handle = 0;
	    return 0;
	}
	return 1;
    } else
	    zmod_set_error(DLXErrorStr());
    return 0;
#elif defined ZMOD_SO
    mod->handle = dlopen(filename,RTLD_LAZY);
    if( mod->handle ) {
	mod->file_name = strdup(filename);
	mod->callback = (zmod_callback_t)
	    (dlsym(mod->handle,"zmod_callback"));
	if( !mod->callback ) {
	    zmod_set_error("SO: no zmod_callback entry in module");
	    dlclose(mod->handle);
	    return 0;
	}
	return 1;
    }
    zmod_set_error(dlerror());
    return 0;
#endif
}
/** Check if module is loaded.
*/
static int zmod_loaded(mod_t mod)
{
#if defined ZMOD_DLL || defined ZMOD_SO || defined ZMOD_DLX
    return (mod->handle != 0);
#endif
}

mod_t zmod_load(const char* module_name)
{
    mod_t mod;
    char *x;
    char buf[Z_FILE_MAX];
    char fname[Z_PATH_MAX];
    strcpy(buf,module_name);
    x = strrchr(buf,'.');
    if( x )
	if( strchr(x,'\\') == NULL ) {
		*x = 0;
	}
    mod = zmod_list_get(buf);
    if( mod ) {
	mod->ref++;
	return mod;
    }
    /* allocate mod_rec */ {
	mod =  (mod_t)calloc(1,sizeof(mod_rec));
	if( !mod )
	    return 0;

	mod->ref = 1;
	mod->file_name = 0;
    }
    /* find module file */ 
    {
	if( zmod_find_file_name(module_name,fname,Z_PATH_MAX) < 0 ) {
	    zmod_set_error(strerror(errno));
	    free(mod);
	    return NULL;
	}
    }
    /* load module */ {
	zmod_do_load(mod,fname);
	if( ! zmod_loaded(mod) ) {
	    free(mod);
	    return 0;
	}
    }
    /* get module info */
    {
	mod->info = (mod_info*) zmod_call(mod,1,(long)&mod_zmod_info);
    }
    if( mod->info == NULL ) {
	free(mod);
	return 0;
    }
    if( mod->info != NULL ) {
	if( zmod_list_add(mod->info->internal_name,mod) == 0 ) {/* OK */
	    if( zmod_get_imports(mod) == 0 ) {
		zmod_call(mod,0,0);
		zmod_do_free(mod);
		return 0;
	    }
	} else {
	    zmod_call(mod,0,0);
	    zmod_do_free(mod);
	    return 0;
	}
    }
    if( mod->info->exp != NULL )
    /* sort symbols if they exist */{
	int i=0;
	while( mod->info->exp[i].name != NULL ) i++;
	if( i > 0 )
	    zmod_sort_symbols(mod->info->exp,i);
	mod->exp_no = i;
    } else
	mod->exp_no = 0;

    return mod;
}

/** Free module.
    Decrease reference count of module,
    if 0 delete it.
*/
int	zmod_free(mod_t mod)
{
    if( mod == NULL )
	return -1;
    if( --(mod->ref) <= 0 ) {
	zmod_call(mod,0,0);
	zmod_free_imports(mod);
	zmod_list_del(mod);
	zmod_do_free(mod);
	return 0;
    }
    return 0;
}
/*
    free the module
	-don't check reference count
	-don't delete from module list

*/
static int	zmod_free_nl(mod_t mod)
{
    if( mod == NULL )
	return -1;
    zmod_call(mod,0,0);
    zmod_free_imports(mod);
    zmod_do_free(mod);
    return 0;
}
/** Free module record.
    Delete module without checking reference count.

    Shouldn't be called directly.
*/
static int zmod_do_free(mod_t mod)
{
#ifdef ZMOD_DLL
    if( mod ) {
	FreeLibrary(mod->handle);
	mod->handle = 0;
	if( mod->file_name)
	    free(mod->file_name);
	mod->file_name = 0;
	mod->info = 0;
	free(mod);
	return 0;
    }
    return -1;
#elif defined ZMOD_DLX
    if( mod ) {
	DLXUnload(mod->handle);
	mod->handle = 0;
	if( mod->file_name)
	    free(mod->file_name);
	mod->file_name = 0;
	mod->info = 0;
	free(mod);
	return 0;
    }
    return -1;
#elif defined ZMOD_SO
    if( mod ) {
	dlclose(mod->handle);
	mod->handle = 0;
	if( mod->file_name)
	    free(mod->file_name);
	mod->file_name = 0;
	mod->info = 0;
	free(mod);
	return 0;
    }
    return -1;
#endif
}

int zmod_enum(void (*enum_f)(mod_t))
{
    sbti i,end;
    mod_t x;
    if( ! modules )
	init_modules_list();
    if( ! modules )
	return -1;
    end = sbtree_end(modules,0);
    sbti_inc(end);
    for( i = sbtree_begin(modules,0); sbti_eq(i,end); sbti_inc(i) ) {
	x = sbti_get(i);
	if( x )
	    enum_f(x);
    }
    sbti_free(i);
    sbti_free(end);
    return 0;
}
void* zmod_lockfunc(mod_t m,const char* fname)
{
    if( !m )
	return NULL;
    if( !m->info )
	return NULL;
    {
	int n		= m->exp_no;
	exp_sym_info* s = m->info->exp;
	int		last = n;
	int		first = 0;
	do {
	    int ci = (last+first) / 2;
	    int c = strcmp(fname,s[ci].name);
	    if( c > 0 ) {
		first = ci+1;
		if( first >= n )
		    return NULL;
	    } else if( c < 0 ) {
		last = ci;
		if( last == 0 )
		    return NULL;
	    } else
		return s[ci].ptr;
	} while( last != first );
	return NULL;
    }
}
int	zmod_unlockfunc(mod_t mod,void* func)
{
    return 0;
}
mod_info* zmod_get_modinfo(mod_t mod)
{
    return mod ? mod->info : 0;
}

void*	zmod_lock_sym(const char* mod_name,const char* sym_name)
{
    mod_t mod;
    void* sym;
    mod = zmod_load(mod_name);
    sym = zmod_lockfunc(mod,sym_name);
    if( sym )
	return sym;
    if( mod )
	zmod_free(mod);
    return NULL;
}
int		zmod_free_sym(const char* mod_name,const char* sym_name)
{
    mod_t mod;
/*    void* sym; */
    mod = zmod_list_get(mod_name);
/*	zmod_unlockfunc(mod,sym_name); */
    zmod_free(mod);
    return 0;
}

static int		zmod_get_imports(mod_t mod)
{
    imp_sym_info* c;
    if(!mod || !mod->info) return 0;
    if(!mod->info->imp) return 1;
    c = mod->info->imp;
    while( c->name ) {
	if( !c->module ) { /* in future look for global exports */
	    return 0;
	}
	if ( !c->ptr ) {
	    return 0;
	}
	if( (*(c->ptr) = zmod_lock_sym(c->module,c->name)) == 0 )
	    return 0;
	c++;
    }
    return 1;
}
static int		zmod_free_imports(mod_t mod)
{
    imp_sym_info* c;
    if(!mod ) return 0;
    if(!mod->info) return 0;
    if(!mod->info->imp) return 1;
    c = mod->info->imp;
    while( c->name ) {
	if( c->module && c->name )
	    zmod_free_sym(c->module,c->name);
	*(c->ptr) = 0;
	c++;
    }
    return 1;
}
/*
static char* zmod_strdup3(const char* s1,const char* s2,const char* s3)
{
    char* s;
    int len = 1;
    int a,b;
    len += (a = strlen(s1));
    len += (b = strlen(s2));
    len += strlen(s3);
    s =(char*)malloc(len);
    if( !s )
	return 0;
    strcpy(s	  ,s1);
    strcpy(s +a   ,s2);
    strcpy(s +a +b,s3);
    return s;
}
*/
static char* paths_stack[20];
static int paths_ptr = 0;
static char*   zmod_find_path = NULL;

static char*  zmod_ext_table[] = {
#ifdef ZMOD_DLL
    ".dll",
    ".exe",
#elif defined ZMOD_DLX
    "",
    ".dlx",
    ".mod",
    ".drv",
#elif defined ZMOD_SO
    ".so",
    ".mod",
    ".drv",
#endif
    0
};
char*	zmod_set_find_path(char* path)
{
    char* x = zmod_find_path;
    if( path != NULL)
	zmod_find_path = path;
    return x;
}

int zmod_push_find_path(char* path)
{
    if( path == NULL || paths_ptr == countof(paths_stack) )
	return -1;

    paths_stack[paths_ptr++] = path;
    return 0;
}

char* zmod_pop_find_path()
{
    if( paths_ptr == 0 ) return NULL;
    return paths_stack[--paths_ptr];
}

static int	zmod_find_file_name(const char* name,char* buf,int max)
{
    /* for each extension
	try each of the paths (zmod_find_file_by_paths) */
    char* paths[countof(paths_stack)+3];
    char tmp[Z_PATH_MAX];
    char** ext = zmod_ext_table;
    int pi = 0;
    char* x;
    paths[pi] = NULL;
    paths[pi++] = Z_DIR_CURRENT;
    if( paths_ptr > 0 ) {
	int i = paths_ptr-1;
	for( ; i >= 0 ; i-- )
	    paths[pi++] = paths_stack[i];
    }
    if( zmod_find_path && *zmod_find_path) 
	paths[pi++] = zmod_find_path;
    x = getenv("ZMOD_LIBRARY_PATH");
    if( x && *x )
	paths[pi++] = x;
    paths[pi] = NULL;
    buf[0] = 0;
    if( zmod_path_is_abs(name) ) {
	/* if path is absolute then check only extensions */
	if( zmod_file_has_ext(name) ) {
	    /* filename and path is defined strictly : don't enumerate */
	    if( zmod_access(name,ZMR_OK) == 0 ) {
		strncpy(buf,name,max);
		buf[max-1] = 0;
		return 0;
	    }
	} else {
	    /* filename hasn't extesion: check with all default extensions */
	    while( *ext != NULL ) {
		zmod_file_set_ext(name,*ext,/*->*/ tmp,Z_PATH_MAX);
		if( zmod_access(tmp,ZMR_OK) == 0 ) {
		    strncpy(buf,tmp,max);
		    buf[max-1] = 0;
		    return 0;
		}
		ext++;
	    }
	}
    } else {
	/* path is relative, search through whole path */
	if( zmod_file_has_ext(name) ) {
	    /* filename has extesion */
	    return zmod_find_file_by_paths(paths,name,ZMR_OK,/*->*/buf,max);
	} else
	    /* filename hasn't got extension: check for all default
	       extensions */
	    while( *ext ) {
		zmod_file_set_ext(name,*ext,/*->*/tmp,Z_PATH_MAX);
		if( zmod_find_file_by_paths(paths,tmp,ZMR_OK,/*->*/buf,max) == 0 )
		    return 0;
		ext++;
	    }
    }
    /* file not found:
	errno should contain proper value: ENOENT or EACCESS.
    */
    return -1;
}


static int exp_sym_cmp(const void * _a, const void * _b);
static void	zmod_sort_symbols(exp_sym_info* start,int size)
{
    if( start == NULL )
	return;
    qsort(start,size,sizeof(exp_sym_info),exp_sym_cmp);
}

static int exp_sym_cmp(const void * _a, const void * _b)
{
    register exp_sym_info* a = (exp_sym_info*)_a;
    register exp_sym_info* b = (exp_sym_info*)_b;
    if( a->name && b->name )
	return strcmp(a->name,b->name);
    else
	return 0;
}

static int	zmod_find_file_by_path(
    const char* path,
    const char* file,
    int 	type,
    char*	buf,
    size_t	max)
{
    return zpio_find_file_by_path(path,file,type,buf,max);
}

static int	zmod_find_file_by_paths(
    char*	path[],
    const char* file,
    int 	type,
    char*	buf,
    size_t	max)
{
    do {
	if( zmod_find_file_by_path(*path++,file,type,buf,max) == 0 )
	    return 0;
    } while( *path != NULL );
    return -1;
}
static int	zmod_path_is_abs(const char* path)
{
    return zpio_path_is_abs(path);
}
static int	zmod_file_has_ext(const char* name)
{
    return zpio_file_has_ext(name);
}
static int	zmod_file_set_ext(const char* name,const char* ext,char* buf,int max)
{
    return zpio_file_set_ext(name,ext,buf,max);
}
static int	zmod_access(const char* path, int type)
{
    return zpio_access(path,type);
}



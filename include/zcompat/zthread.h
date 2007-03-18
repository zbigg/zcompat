/*
* Header: zthread.h
*
* Description: Header for zthread library.
*
* Author Zbigniew Zagórski <zzbigg@o2.pl>
*/
/*
##
## =Module zthread
##
##  zthread, a C language simple abstract layer for
##  creating threads and mutexes. Portable for UNIX
##  (where POSIX threads are available) and Win32 platform.
##
## =Description
##
##  This simple library can be used only to create detached 
##  threads and anonymoues (unnamed) mutexes.
##
## =Author
##
##  Author Zbigniew Zagorski zzbigg (at) o2 dot pl
##
*/
#ifndef __zthread_h_
#define __zthread_h_

#define INC_ZTHREAD

#ifndef INC_ZDEFS
#include "zdefs.h"
#endif

#ifndef INC_ZSYSTYPE
#include "zcompat/zsystype.h"
#endif

/* On unix use POSIX threads interface. */
#if defined ZSYS_UNIX || defined USE_PTHREAD

#ifndef NO_PTHREAD
#define ZTH_POSIX	1
#define ZTH_OK
#endif

#endif

/* On Win32 use WinAPI interface */

#if defined ZSYS_WIN32 && !defined USE_PTHREAD

#define ZTH_WIN32	1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#define ZTH_OK
#endif

#ifndef ZTH_OK
typedef int  zthread;
typedef int  zmutex_t;
typedef int* zmutex;

#define zth_begin(a,b) ((zthread)0)
#define zth_get_sys_handle(a) ((zthread)0)
#define zth_self() ((zthread)0)
#define zth_exit(a) ((a))

#define zmt_init(a) (0)
#define zmt_frees(a) (0)
#define zmt_new() ((zmutex)0)
#define zmt_free(a) (0)

#define zmt_lock(a,b) (-1)
#define zmt_try_lock(a) (-1)

#else /* Define zthread interface */

/* C Extern Identifiers. */
#ifdef __cplusplus
extern "C" {
#endif


#if defined ZTH_POSIX
typedef pthread_mutex_t	zmutex_t;
typedef zmutex_t*	zmutex;

typedef unsigned long	zthread;

#endif

#if defined ZTH_WIN32
typedef HANDLE		zmutex_t;
typedef zmutex_t*	zmutex;

typedef unsigned long	zthread;
#endif

/*
############################################
## =Function zth_begin
##	Begin a new thread
## =Synopsis
##  |	#include "zthread.h"
##  |	zthread	zth_begin(
##  |	    void  (* _thread_function)(void*),
##  |	    void* _thread_arg);
##
##  =Description
##	Begin a new thread
##  =Return value
##
*/
ZCEXPORT zthread    zth_begin(
    void  (* _thread_function)(void*),
    void* _thread_arg
    );
    
/** Get identifier of calling thread.
*/
/*
############################################
## =Function zth_self
##	Get identifier of calling thread
## =Synopsis
##  |	#include "zthread.h"
##  |	zthread	zth_self(void);
##
##  =Description
##	
##  =Return value
##	zthread handle of calling thread.
##
*/
ZCEXPORT zthread    zth_self(void);

/** Exit calling thread.
*/

/*
############################################
## =Function zth_exit
##	exit calling thread
## =Synopsis
##  |	#include "zthread.h"
##  |	void	zth_exit(void* retval);
##
##  =Description
##	Exit calling thread.
##
##	Argument <retval> have no meaning now. Should be NULL.
##  =Return value
##	This function doesn't return.
##
*/
ZCEXPORT void    zth_exit(void* retval);

/** Get system thread handle.

*/
/*
############################################
## =Function zth_get_sys_handle
##	return system handle
## =Synopsis
##  |	#include "zthread.h"
##  |	unsigned long zth_get_sys_handle(zthread th);
##
##  =Description
##    On various systems threads have different interfaces. This
##    function returns specific system representation of thread.
##    On unices (using POSIX threads) it returns pthread_t. On 
##    Win32 it returns thread identifier (returned by CreateThread)
##    and so on.
##
##  =Return value
##	System handle for zthread handle.
##
*/
ZCEXPORT unsigned long    zth_get_sys_handle(zthread th);

/*
############################################
## =Function zmt_init
##	initialize static mutex structure
## =Synopsis
##  |	#include "zthread.h"
##  |	int	zmt_init(zmutex m);
##
##  =Description
##	Initilaize mutex structure zmutex_t.	
##
##  =Return value
##	Returns -1 on error and 0 on success.
*/
ZCEXPORT int    zmt_init(zmutex m);
/*
############################################
## =Function zmt_done
##	deinitialize static mutex
## =Synopsis
##  |	#include "zthread.h"
##  |	int	zmt_done(zmutex m);
##
##  =Description
##
##	Deinitialize mutex structure zmutex_t.	
##
##  =Return value
##	Returns -1 on error (mutex is locked by other thread)
##	and 0 on success.
##
*/
ZCEXPORT int    zmt_done(zmutex m);
ZCEXPORT int    zmt_frees(zmutex m); /* EQ: xmt_done */

/*
############################################
## =Function zmt_new
##	create new mutex on heap
## =Synopsis
##  |	#include "zthread.h"
##  |	zmutex	zmt_new(void);
##
##  =Description
##	Create new mutex on heap.
##	New mutex is not owned by anybody.
##
##  =Return value
##	Function <zmt_new> returns handle to new mutex
##	which should be released by <zmt_free>.
*/
ZCEXPORT zmutex    zmt_new(void);


/*
############################################
## =Function zmt_free
##	    destroy mutex
## =Synopsis
##  |	#include "zthread.h"
##  |	int 	zmt_free(zmutex m);
##
##  =Description
##    If the mutex isn't locked then destroy it and release memory.
##
##  =Return value
##    Returns 1 on succes, 0 if the mutex is locked by another 
##    thread, -1 on error.
*/
ZCEXPORT int     zmt_free(zmutex m);

/*
############################################
## =Function zmt_lock
##	lock the mutex.
## =Synopsis
##  |	#include "zthread.h"
##  |	int 	zmt_lock(zmutex m,unsigned long usec);
##
##  =Description
##    Lock the mutex.
##
##    Try to get ownership of mutex. If the mutex is owned 
##    by another thread function waits for possiblity to gain
##    ownership not longer than usec miliseconds.
##    
##    If usec == ZMT_INFINITY it waits without timeuot.
##    
##  =Return value
##    Returns 1 if mutex is locked succesfully, 0 if mutex can't
##    be locked, -1 on error.
##
*/
ZCEXPORT int     zmt_lock(zmutex m,unsigned long usec);
#define ZMT_INFINITY ((unsigned long)-1)

/*
############################################
## =Function zmt_try_lock
##	try to lock mutex.
## =Synopsis
##  |	#include "zthread.h"
##  |	int	zmt_try_lock(zmutex m);
##
##  =Description
##
##    Try to lock mutex, but if it is owned by another thread
##    return immediately.
##    
##  =Return value
##    Returns 1 if the mutex was locked, 0 if it is locked by 
##    another thread, -1 on error.
*/
ZCEXPORT int    zmt_try_lock(zmutex m);


/*
############################################
## =Function zmt_unlock
##	unlock/release mutex.
## =Synopsis
##  |	#include "zthread.h"
##  |	int	zmt_unlock(zmutex m);
##
##  =Description
##
##	Unlock mutex.
##	Mutex must be locked by calling thread. If not something 
##	unspecified may occurr.	
##  =Return value
##
##    Try to lock mutex, but if it is owned by another thread
##    return immediately.
*/
ZCEXPORT int    zmt_unlock(zmutex m);

#endif

#ifdef __cplusplus
}
#endif

#endif /* force single include of file */

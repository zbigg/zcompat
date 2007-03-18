/*
* File: zthread.c
*
* Description:
*	Impelemtation of zthread library - portable interface for
*	programming threads.
*
*	Currently supported platforms:
*	    * POSIX Threads - unix
*	    * Win32 - while building
*
* Author Zbigniew Zagórski <longmanz@polbox.com>
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "zcompat/zsystype.h"

#include "zcompat/zthread.h"

#ifdef ZTH_POSIX
#include <pthread.h>
#endif

#ifdef ZTH_WIN32
/** Initial stack size.
   * Win32 only.
   Defines initial size of stack in thread created by zth_begin.
*/
#define ZTH_THREAD_STACK_SIZE	1024
#include <process.h>
#endif


zthread zth_begin(
    void (* _thread_function)(void*),
    void* _thread_arg
    )
{
#if	defined ZTH_POSIX
    pthread_t t;
    if( pthread_create(&t,NULL,(void*)_thread_function,_thread_arg) != 0 )
	return 0;
    pthread_detach((pthread_t)t);
    return (zthread)t;

#elif	defined ZTH_WIN32
    return (zthread) _beginthread(_thread_function,ZTH_THREAD_STACK_SIZE,_thread_arg);
#else
    return 0;
#endif
}

zthread zth_self(void)
{
#if	defined ZTH_POSIX
    return (zthread) pthread_self();
#elif	defined ZTH_WIN32
    return (zthread)0;
#else
    return (zthread)0;
#endif
}

void	zth_exit(void* retval)
{
#if	defined ZTH_POSIX
    pthread_exit(retval);
#elif	defined ZTH_WIN32
    _endthread();
#else
    return;
#endif
}

unsigned long zth_get_sys_handle(zthread th)
{
    return ( unsigned long) th;
}

int zmt_init(zmutex a)
{
    if( a == NULL ) {
	errno = EINVAL;
	return -1;
    }
#ifdef ZTH_POSIX
    memset(a,0,sizeof(zmutex_t));
    pthread_mutex_init(a,0);
    zmt_unlock(a);
#endif
#ifdef ZTH_WIN32
    *a = CreateMutex(NULL,TRUE,NULL);
    if( *a == NULL )
	return -1;
#endif
    errno = ENODEV;
    return -1;
}

zmutex	zmt_new(void)
{
#ifdef ZTH_OK
    zmutex m = (zmutex)malloc( sizeof( zmutex_t) );
    if( m == NULL )
	return NULL;
    zmt_init(m);

#ifdef ZTH_POSIX
    zmt_unlock(m);
#endif
    return m;
#else /* no threads */
    errno = ENODEV;
    return (zmutex)0;
#endif
}

int	zmt_free(zmutex m)
{
    zmt_frees(m);
    free(m);
    return 0;
}
int	zmt_frees(zmutex m)
{
#if	defined ZTH_POSIX
    pthread_mutex_destroy(m);
    return 0;
#elif	defined ZTH_WIN32
    CloseHandle(*m);
    return 0;
#else
    errno = ENODEV;
    return 0;
#endif
}

int	zmt_lock(zmutex m,unsigned long usec)
{
#if	defined ZTH_POSIX
#ifdef HAVE_PTHREAD_MUTEX_TIMEDLOCK
    struct timespec tm;
    int r;
    if( usec == ZMT_INFINITY ) 
        r = pthread_mutex_lock(m);
    else {
        tm.tv_sec = usec / 1000;
        tm.tv_nsec = (usec*1000)%1000000;
        r = pthread_mutex_timedlock(m,&tm);
    }
    return r;
#else
    #warning "zmt_lock() desn't support timeout argument"
    return pthread_mutex_lock(m);
#endif
#elif	defined ZTH_WIN32
    if( m == NULL )
	return -1;
    switch( WaitForSingleObject(*m,usec) ) {
    case WAIT_ABANDONED:
	return -1;
    case WAIT_OBJECT_0:
	return 1;
    case WAIT_TIMEOUT:
	return 0;
    default:
	return -1;
    }
#else
    errno = ENODEV;
    return -1;
#endif
}
#define ZMT_INFINITY ((unsigned long)-1)

int	zmt_try_lock(zmutex m)
{
    if( m == NULL )
	return -1;
#if	defined ZTH_POSIX
    return pthread_mutex_trylock(m);
#elif	defined ZTH_WIN32
    switch( WaitForSingleObject(*m,0) ) {
    case WAIT_FAILED:
	return -1;
    case WAIT_ABANDONED:
    case WAIT_OBJECT_0:
	return 1;
    case WAIT_TIMEOUT:
	return 0;
    default:
	return -1;
    }
#else
    errno = ENODEV;
    return -1;
#endif
}

int	zmt_unlock(zmutex m)
{
    if( m == NULL )
	return -1;
#if	defined ZTH_POSIX
    return pthread_mutex_unlock(m);
#elif	defined ZTH_WIN32
    if( ReleaseMutex(*m) == TRUE )
	return 1;
    return 0;
#else
    errno = ENODEV;
    return -1;
#endif
}


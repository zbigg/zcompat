/*
* File: win32f.c
*
* Id:           $Id: win32f.c,v 1.7 2003/06/28 11:57:45 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  Win32 Files routines
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zsystype.h"

#ifdef ZSYS_WIN
/*
#define ZPIOWIN_USE_INT_EVENT
*/
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

#include <errno.h>
#include <windows.h>

#include "wutil.h"
extern DWORD zwin32_last_error;
extern const char* win32_strerror(DWORD er);

#ifdef ZPIOWIN_USE_INT_EVENT
int zpiowin_waithandle(HANDLE f) 
#endif


/* CreateFile .. */
ZSTREAM	zwin32file_open	(ZSTREAM,const char*,int);
int	zwin32file_close(ZSTREAM);
int	zwin32file_read	(ZSTREAM,void*,size_t);
int	zwin32file_write(ZSTREAM,const void*,size_t);
zoff_t	zwin32file_seek	(ZSTREAM,zoff_t,int);
long	zwin32file_ctl	(ZSTREAM,int,void*);

zstream_vtable_t zvt_win32file = {
    5,			/*__ id */
    1,			/*__ data_size */
    zwin32file_open,	/*__ open */
    zwin32file_close,	/*__ close */
    zwin32file_read,	/*__ read */
    zwin32file_write,	/*__ write */
    zwin32file_seek,	/*__ seek */
    zwin32file_ctl,	/*__ ctl */
};

/* ------------------
             PUBLIC
            -----------------*/

/* ----------------------====*****      WIN32FILE
*/
ZSTREAM	zwin32hopen	(ZWIN_HANDLE h)
{
        return zcreat(&zvt_win32file,NULL,(int)h);
}

#define win_handle ((HANDLE)(f->data[0]))
ZSTREAM	zwin32file_open	(ZSTREAM f,const char* name,int mode)
{
        if( name == NULL ) {
                win_handle = (HANDLE)mode;
	        ZFL_SET(f->flags, ZSF_PUBLIC);
        } else {
    	    DWORD dwDesiredAccess =
            	(ZFL_ISSET(mode,ZO_READ)
            	    ? GENERIC_READ
                    : 0) |
            	(ZFL_ISSET(mode,ZO_WRITE)
            	    ? GENERIC_WRITE
                    : 0);
    	    DWORD dwCreationDistribution;
    	    /* convert open flags ZO_CREAT, ZO_EXCL, ZO_TRUNC to be compatible
    	       dwCreationRepresentation parameter of CreateFile */
	    {
            	register int fcreat = ZFL_ISSET(mode,ZO_CREAT);
            	register int fexcl = ZFL_ISSET(mode,ZO_EXCL);
            	register int ftrunc = ZFL_ISSET(mode,ZO_TRUNC);
            	if( fcreat ) {
            	    if( fexcl )
                        if( ftrunc )    /* fcreat | fexcl | ftrunc */
                            dwCreationDistribution = CREATE_NEW;
                        else           /* fcreat | fexcl ----  BAD */
                            dwCreationDistribution = 0;
                    else
                        if( ftrunc )    /* fcreat | ftrunc */
                            dwCreationDistribution = CREATE_ALWAYS;
                        else            /* fcreat */
                            dwCreationDistribution = OPEN_ALWAYS;
                } else if( ftrunc ) {
                    if( fexcl )             /* ftrunc | fexcl  ---- BAD */
                        dwCreationDistribution = 0;
                    else                    /* ftrunc */
                        dwCreationDistribution = TRUNCATE_EXISTING;
                } else if( fexcl ) {            /* fexcl ----  BAD*/
                    dwCreationDistribution = 0;
        	} else
            	    dwCreationDistribution = OPEN_EXISTING;
    	    }
            win_handle = CreateFile(name,
                        dwDesiredAccess,
                        0,
                        NULL,
                        dwCreationDistribution,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
	}
	if( win_handle == INVALID_HANDLE_VALUE || win_handle == NULL ) {
	    errno = zpiowin_get_last_errno();
	    return NULL;
	} else {
	    DWORD ft = GetFileType(win_handle);
	    switch( ft ) {
	    case FILE_TYPE_CHAR:
	    case FILE_TYPE_PIPE:
		ZFL_UNSET(f->flags, ZSF_FILE);
		break;
	    case FILE_TYPE_DISK:
	    case FILE_TYPE_UNKNOWN:
	    default:
		ZFL_SET(f->flags, ZSF_FILE);
		break;
	    }
	    f->name = zcompat_strdup(name ? name : "preset handle");
	    return f;
	}
}
int	zwin32file_close(ZSTREAM f)
{
    BOOL r;
    if( ZFL_ISSET(f->flags,ZSF_PUBLIC) )
	return 0;
    r = CloseHandle(win_handle);
    if( r == TRUE )
        return 0;
    else {
        errno = zpiowin_get_last_errno();
        return ZR_ERROR;
    }
}
int	zwin32file_read	(ZSTREAM f,void* buf,size_t size)
{
        DWORD readed;
#ifdef ZPIOWIN_USE_INT_EVENT
        int wr;
        if( (wr = wait_for_desc(win_handle)) != 0 ) {
            f->error = errno;
            return wr;
        }
#endif
        if( ReadFile(win_handle,
                (LPVOID)buf,
                (DWORD) size,
                &readed,
                NULL)
        ) {
                if( readed == 0 )
                        f->eof = 1;
                return (int)readed;
        } else switch ( GetLastError() ) {
        case ERROR_BROKEN_PIPE:
        	f->eof = 1;
                return 0;
        default:
                f->error = errno = zpiowin_get_last_errno();
                return ZR_ERROR;
        }
}
int	zwin32file_write(ZSTREAM f,const void* buf,size_t size)
{
        DWORD written;
        if( WriteFile(win_handle,
                (LPCVOID)buf,
                (DWORD)  size,
                &written,
                NULL)
        ) {
                return written;
        } else {
                f->error = errno = zpiowin_get_last_errno();
                return ZR_ERROR;
        }
}
zoff_t	zwin32file_seek	(ZSTREAM f,zoff_t offset,int whence)
{
        DWORD mm = 0;
        DWORD r;
        switch( whence ) {
        case ZSEEK_SET:
                mm = FILE_BEGIN;
                break;
        case ZSEEK_CUR:
                mm = FILE_CURRENT;
                break;
        case ZSEEK_END:
                mm = FILE_END;
                break;
        }
        if( (r=SetFilePointer(win_handle,
                (LONG) offset,
                NULL,
                mm)) != 0xffffffff )
        {
                return (zoff_t)r;
        } else {
                f->error = errno = zpiowin_get_last_errno();
                return ZR_ERROR;
        }

}
long	zwin32file_ctl	(ZSTREAM f,int what,void* data)
{
    switch( what ) {
    case ZFCTL_DUP:
        {
            HANDLE new_handle;
            if( ! DuplicateHandle(
                GetCurrentProcess(),
                win_handle,
                GetCurrentProcess(),
                &new_handle,
                0,
                TRUE,
                DUPLICATE_SAME_ACCESS)) {
                return -1;
            }
            return (long)zwin32hopen(new_handle);
        }
    case ZFCTL_GET_READ_DESC:
	return ZFL_ISSET(f->mode,ZO_READ)
	    ? (long)win_handle
	    : (long)NULL;
    case ZFCTL_GET_WRITE_DESC:
	return ZFL_ISSET(f->mode,ZO_WRITE)
	    ? (long)win_handle
	    : (long)NULL;
    case ZFCTL_GET_READ_STREAM:
	return ZFL_ISSET(f->mode,ZO_READ)
	    ? (int)f
	    : (int)NULL;
    case ZFCTL_GET_WRITE_STREAM:
	return ZFL_ISSET(f->mode,ZO_WRITE)
	    ? (int)f
	    : (int)NULL;
    case ZFCTL_GET_BLOCKING:
        return 0;
    case ZFCTL_SET_BLOCKING:
        return -1;
    case ZFCTL_EOF:
	return -1;
    }
    errno = EINVAL;
    return -1;
}
#ifdef ZPIOWIN_USE_INT_EVENT
static HANDLE interrupt_event = NULL;

static int init_int_ev(void) 
{
    if( interrupt_event ) return 1;

    interrupt_event = CreateEvent(NULL,TRUE,FALSE,NULL);
    zc_atexit(done_int_ev);
    return 1;
}
static void done_int_ev(void) 
{
    if( interrupt_event )
        CloseHandle(interrupt_event);
    interrupt_event = NULL;
}
static int wait_for_desc(HANDLE f) {
    HANDLE ht[2];

    if( ! interrupt_event && ! init_int_ev() ) return ZR_ERROR;
    ht[0] = interrupt_event;
    ht[1] = f;
    switch( WaitForMultipleObjects(2,ht,FALSE,INFINITE) ) {
    case WAIT_OBJECT_0:     /* interrupt ! */
        errno = EINTR;
        return ZR_INTR;
    case WAIT_OBJECT_0+1:   /* ready handle */
        return 0;
    default:
        errno = zpiowin_get_last_errno();
        return ZR_ERROR;
    }
}
int zpiowin_waithandle(HANDLE f) 
{
    return wait_for_desc(f);
}
void    zpiowin_raise_io(void)
{
    if( !interrupt_event ) 
        /* no event, then nobody surely waits for them */
        return;  
    PulseEvent(interrupt_event);
    Sleep(1);
    ResetEvent(interrupt_event);
}
#endif

#endif /* #ifdef ZSYS_WIN */

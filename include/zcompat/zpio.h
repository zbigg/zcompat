/*
* File:		zpio.h
*
* Project:	ZCompat Portable Input/Output library.
*
* Decription:	Library main header
*
* Author:	Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
* To do:
    * add ztruncate(ZSTREAM f, ?pos or current?)
*/

#ifndef __zpio_h_
#define __zpio_h_

#define INC_ZPIO

#include "zcompat/zcompat.h"
#include <stddef.h>
#include <fcntl.h>
#include <stdarg.h>

#if defined ZSYS_UNIX || defined ZDEV_DJGPP || defined ZDEV_MINGW
#include <sys/param.h>
#define HAVE_OFF_T
#define HAVE_SSIZE_T
#define HAVE_MODE_T

#else	/* unices */

/*#include <io.h>*/
#include <sys/types.h>
#define HAVE_OFF_T

#endif	/* dos/windows */

/*
#define ZPIO_WITH_POSIX
#define ZPIO_WITH_ANSI
#define ZPIO_WITH_WIN32F
#define ZPIO_WITH_BSDSOCK
#define ZPIO_WITH_MEMBUF
#define ZPIO_WITH_PIPE
#define ZPIO_WITH_FILTER
*/
#ifndef WANT_STDIO
#define WANT_STDIO 1
#endif


#if WANT_STDIO
#include <stdio.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_SSIZE_T
typedef unsigned long int ssize_t;
#endif

#ifndef HAVE_OFF_T
typedef unsigned long int off_t;
#endif

#ifndef HAVE_MODE_T
typedef unsigned int mode_t;
#endif

typedef off_t	zoff_t;
typedef size_t	zsize_t;
typedef mode_t	zmode_t;

typedef unsigned long    ptr_int;

/* 
struct	zstream_s;
typedef struct zstream_s zstream_t;

typedef zstream_t	ZFILE;
typedef zstream_t*	ZSTREAM;
	-- defined in zcompat.h
*/
struct	zstream_vtable;
typedef struct zstream_vtable	zstream_vtable_t;

struct zstream_vtable {
    int		vt_id;
    int		vt_data_size;

    ZSTREAM	(*vt_open)	(ZSTREAM,const char*,int);
    int		(*vt_close)	(ZSTREAM);
    int		(*vt_read)	(ZSTREAM,void*,size_t);
    int		(*vt_write)	(ZSTREAM,const void*,size_t);
    zoff_t	(*vt_seek)	(ZSTREAM,zoff_t,int);
    long	(*vt_ctl)	(ZSTREAM,int,void*);

/* FUTURE EXTENSIONS
    int		(*vt_putc)	(ZSTREAM,int);
    int		(*vt_getc)	(ZSTREAM,int);
    int		(*vt_puts)	(ZSTREAM,const char*);
    int		(*vt_gets)	(ZSTREAM,char*,int);
    int		(*vt_vprintf)	(ZSTREAM,const char*,void*);

    int		(*vt_putw)	(ZSTREAM,const char*);
    int		(*vt_getw)	(ZSTREAM,const char*,int);

    int		(*vt_pending)	(ZSTREAM);
*/
};

struct zstream_buf {
    /** buffer address */
    char*	buffer;
    /** buffer capacity */
    int		buffer_size;
    /** next sequence */
    char*	start;
    /** pointer to first invalid */
    char*	end;
    /** offset of buffer in file */
    zoff_t	start_offset;
    zoff_t	end_offset;
};

/** ZSTREAM descriptor.
*/
struct zstream_s {
    /** Reference count
    */
    int         ref_count;

    int		flags;
    /** Open mode.
	Combination of ZO_xxx constants.
    */
    int		mode;

    /** Error flag.
	errno value of last operation, 0 on success.
    */
    int		error;

    /** Value of last ungetc.
    */
    int*	ungetc_top;
    int*	ungetc_start;
    unsigned	ungetc_capacity;

    /** EOF indicator.
    */
    int		eof;

    /** Name of the stream.
	Name of file, or name of host where socket is connected or
	name of command linked to pipe etc.
    */
    char*	name;

    union {
	struct {
	    struct zstream_buf	in;
	    int			in_enabled;
	    struct zstream_buf	out;
	    int			out_enabled;
	} pipe;
	struct {
	    struct zstream_buf	io;
	    int			enabled;
	} file;
    } buf;
    zoff_t	real_offset;
    zoff_t	offset;
    ZSTREAM	parent;
    ZSTREAM	chain;

    zstream_vtable_t* vtable;

    unsigned long data[1];
};

/**
*/
/**@name ZPIO open mode flags

    Used as mode argument in functions that open stream.
    Use combination of them.
*/
/*@{ */
/** Open for reading. */
#define ZO_READ		0x0001
/** Open for writing. */
#define ZO_WRITE	0x0002
/** Append to existing file. */
#define ZO_APPEND	0x0004
/** Don't open if exists. */
#define ZO_EXCL		0x0008
/** Truncate if exists. */
#define ZO_TRUNC	0x0010
/** Set asynchrous mode. */
#define ZO_NODELAY	0x0020
/** Create if doesn't exists. */
#define ZO_CREAT	0x0040

#define ZO_BINARY	0x0100
#define ZO_TEXT		0x0200

#define ZO_RD		ZO_READ
#define ZO_WR		ZO_WRITE
/** Open for reading and writing. */
#define ZO_RDWR		( ZO_READ | ZO_WRITE )
#define ZO_NEW		( ZO_WRITE | ZO_CREAT | ZO_TRUNC )

/*@} */
/** */

/** Can anyone describe it ?
*/
#define ZSF_CHAIN	0x01

/** Medium in stream is public.

    Set if stream is associated with some public medium
    (like posix descriptors 0,1,2).

    Stream method vt_close can't free this medium.
*/
#define ZSF_PUBLIC	0x02

/** Buffer is done in file style.

    Buffer is done in file style, else in pipe style.
*/
#define ZSF_FILE	0x04

/** Input buffer is enabled.
*/
#define ZSF_IBUF	0x08

/** Output buffer is enabled.
*/
#define ZSF_OBUF	0x10
/**@name I/O routines return values

    Currently those modes are not supported.
*/
/*@{ */
/** End-of-file reached.
    Returned when there is no more data to read.
*/
#define ZR_EOF		0

/** Error ocurred.
    Some error ocurred while doing I/O operation
    Type of error (errno-style) is in f->error
*/
#define ZR_ERROR	-1
#define ZR_ERR		ZR_ERROR
/** Operation would have wait for done.
    If you selected non-blocking I/O all I/O
    functions returns immediately even if
    requested operation isn't done yet.
*/
#define ZR_AGAIN	-2
/** Interrupted system call.
    Call was interrupted before finished.

    Probably signal has ocurred.
*/
#define ZR_INTR		-3

#define Z_ERRNO_2_ZR(_eno)	(	\
	_eno == EAGAIN ? ZR_AGAIN	\
    :	_eno == EINTR  ? ZR_INTR	\
    :   		 ZR_ERROR )
/*@} */



   /*********************************
  /       I/O Basics               /
 *********************************/

/**@name I/O Basics
*/
/*@{ */

/** Create stream.

    Create stream using desired zstream_vtable.

    @param	vt	pointer to vtable
    @param	name	name parameter passed to open function
    @param	mode	mode paramater passed to open function
*/
ZCEXPORT ZSTREAM    zcreat(zstream_vtable_t* vt,const char* name,int mode);

/** Close stream.

    This function performs everything what is nedded to close stream:
    close descriptors,free buffers and so on.

    It calls free on all chained streams.

    @param s		stream
*/

ZCEXPORT ZSTREAM    zstream_ref(ZSTREAM s);

ZCEXPORT int    zclose(ZSTREAM s);

/** Close sub-stream.

    Close sub-stream for reading (mode = O_READ )
    or for writing (mode = O_WRITE) of stream.

    Use only for duplex pipes or dstreams.

    @param s		stream
*/

ZCEXPORT int    zclosesub(ZSTREAM s,int mode);

/** Get/set stream options.

    @param s		stream
    @param func		operation number (see ZFCTL_xxx macros)
    @param data		optional data for operation
*/
ZCEXPORT int    zfctl(ZSTREAM s,int func,void* data);

/*
## =Function zdup
##
## =Synopsis
##  |   ZSTREAM zdup(ZSTREAM s);
##
##  =Description
##
##
##
##  =Return value
##
##
*/
ZCEXPORT ZSTREAM    zdup(ZSTREAM s);

/** Read from stream.
    Works as read(2) - on error returns ZR_ERR , on EOF
    returns 0, else returns number of bytes read.

    If there is no data available, and stream is set to
    non-blocking mode it returns -1 and errno is set to EAGAIN.

    @param s		stream to read from
    @param db		data buffer pointer
    @param dbsize	requested size of data read
*/
ZCEXPORT int    zread(ZSTREAM s,void* db,int dbsize);

/** Write to stream.

    Works as write(2) - on error returns -1 , on EOF
    returns 0, else returns number of bytes written.

    @param s		stream to write to
    @param db		data buffer pointer
    @param dbsize	requested size of data to write
*/
ZCEXPORT int    zwrite(ZSTREAM s,const void* db,int dbsize);

/** Changes position in file.

    For normal fdesc works as lseek (2)

    @param s		stream
    @param offset	offset to set
    @param whence	origin (ZSEEK_xxx)
*/
ZCEXPORT zoff_t    zseek(ZSTREAM s,zoff_t offset,int whence);


/** Send bytes from one stream to other.

    Sends bytes from src to dest. If max > 0 it sends no
    more than max. If max <= 0 it continues until reached
    eof in src or error occured.

    Flags currently must be 0.

    @param	dest	destination stream
    @param	src	source stream

    @return	Number of bytes transferred.
*/
ZCEXPORT int    zpio_send(ZSTREAM dest,ZSTREAM src,int max,int flags);
/*@} */

/** Enable/disable/get/set buffer size.

    If size == 0 buffer disabled.
    If size == -1, nothing is done.
    else buffer size is set to requested size (and enabled).

    Always is returned size of buffer after call, 0 if is disabled.

    Not tested.
*/
ZCEXPORT int    zsetbuf(ZSTREAM f,int isize,int osize);
ZCEXPORT int    zsetibuf(ZSTREAM f,int isize,int osize);

/** Truncate file at current position.

    NOT IMPLEMENTED.
*/
ZCEXPORT int    ztruncate(ZSTREAM s);
/**@name Seek origins */
/*@{*/
/** Seek from start of file */
#define ZSEEK_SET        SEEK_SET
/** Seek from end of file */
#define ZSEEK_END        SEEK_END
/** Seek from current position in file */
#define ZSEEK_CUR        SEEK_CUR
/*@}*/


ZCEXPORT ZSTREAM    zfilter_creat(
    ZSTREAM 	f,
    int (*	f_read)(ZSTREAM,void*,size_t),
    int (*	f_write)(ZSTREAM,const void*,size_t),
    int data_size
    );

/** Push chain.

    Push <next> to top of <base>'s chain.
*/
ZCEXPORT int    zf_push(ZSTREAM base,ZSTREAM next);
/** Pop chain.

    Delete topmost stream from <base>'s chain and return its pointer.
*/
ZCEXPORT ZSTREAM    zf_pop(ZSTREAM base);
/** Duplicate vtable

    Creates exact copy of <vt> in newly allocated place ( zpio_malloc ).
*/
ZCEXPORT    zstream_vtable_t*     zpio_vt_dup(const zstream_vtable_t* vt);

   /*********************************
  /       Opening streams          /
 *********************************/

/**@name Opening streams */
/*@{*/

/** Opens a stream.

    Opens stream for given name.

    If name starts with http:// then it uses zhttpopen - opens
    file from HTTP server.

    If name is socket://[host_name | ip_address]:port then it
    tries to connect there.

    If name is pipe://command then it creates a pipe connected

    to output and/or input of command( depending on mode ).

    Other names (and file://name) are treated as files.

    @param name		name of stream resource
    @param mode		mode of open, combination of ZO_xxx constants
*/

typedef	ZSTREAM (* zpio_user_open_f)(const char*,int,int*);

#define ZPIO_USER_OPEN_MAX 10
/**
*/
ZCEXPORT int    zpio_register_user_open_func(zpio_user_open_f func);

ZCEXPORT ZSTREAM    zopen(const char* name,int mode);

/** Opens a file.

    Opens  local filesystem file given by name.

    @param name		name of stream resource
    @param mode		mode of open, combination of ZO_xxx constants

    @return	on success: stream
		on error: NULL, errno is set
*/

ZCEXPORT ZSTREAM    zfopen(const char* filename,int mode);

/** Creates ZFILE structure for ANSI stream.

    Parameter file is returned from fopen,fdopen or freopen function.
    ANSI C I/O functions are usually buffered and faster than zpio
    library ( especially with large number of small reads or writes).

    @param stream	ANSI C stream object
*/
#if WANT_STDIO
ZCEXPORT ZSTREAM    zansiopen(FILE* stream,const char* name);
#else
/* returns _always_ NULL, errno = ENODEV */

ZCEXPORT ZSTREAM    zansiopen(void* stream,const char* name);

#endif

/** Create ZFILE struct for previously opened POSIX descriptor.

    @param fd		descriptor of file,pipe or socket or whatever you wan't
    @param filename	name of the stream
*/
ZCEXPORT ZSTREAM    zdopen(int fd,const char* filename);
/** Create ZFILE struct for previously opened POSIX descriptor.

    @param fd		descriptor of file,pipe or socket or whatever you wan't
    @param filename	name of the stream
*/
ZCEXPORT ZSTREAM    zposixopen(int fd,const char* filename);

#define ZWIN_HANDLE void*
ZCEXPORT ZSTREAM    zwin32hopen	(ZWIN_HANDLE h);

/** Open TCP/IP listen socket.

    ... not description yet ...

   @param baddress	a name or IP adress to bind ( currently must be NULL)
   @param bport		port number to bind to

   @see		zdopen
*/
ZCEXPORT ZSTREAM    zsopen_tcp_server(const char* baddress, short bport);

/** Accept connection on listening socket.

    ... not description yet ...

   @param slisten	stream with listening socket

   @see		zdopen
*/
ZCEXPORT ZSTREAM    zaccept(ZSTREAM slisten);

/** Open socket, and connects to remote host.

    This functions performs looking for ip addres of host, and converts
    port from host short to net short ( htons ). Then opens an Internet
    (TCP/IP) stream full duplex communicaton channel.

   @param rhost		a name or IP adress of remote host
   @param rport		port number to connext to

   @see		zdopen
*/
ZCEXPORT ZSTREAM    zsopen(const char* rhost,short rport);

/** Create stream for socket.

    Create stream for selected sock_fd.

    Caller is resonsible for closing socket.
*/
ZCEXPORT ZSTREAM    zsreopen(int sock_fd);

/** Open a pipe.

    Opens a one way stream to shell command ( executed by system(3) ).
    If you wan't to read from pipe set mode to ZO_RDONLY.
    If you wan't to write to pipe set mode to ZO_WRONLY. Other values of
    mode are invalid.
    
    On success returned stream contains information about PID of
    created process. To obtain it use
        pid = zfcnlt(s, ZFCTL_GET_PID, 0)
    Note that this pid is handle to process on Win32 and PID on unices.

    @param program	shell command to execute
    @param mode		ZO_RD or ZO_WR or ZO_RDWR
*/

ZCEXPORT ZSTREAM    zpopen(const char* program,int mode);

/*   =Function zpio_mfifo_new 
	create a memory fifo stream
     
     =Synopsis
     |ZSTREAM zpio_mfifo_new(size_t size);
     
     =Description
     TODO: Add description of <zpio_mfifo_new> function.
     
*/

/*## =Function zpio_mfifo_new
        short description of zpio_mfifo_new
            =Synopsis
            |int     zpio_mfifo_new(size_t size,ZSTREAM* reader, ZSTREAM* writer);
            
            =Description
            short description ...
       */
ZCEXPORT int        zpio_mfifo_new(size_t size,ZSTREAM* reader, ZSTREAM* writer);

#define ZFCTL_GET_PID	    0x0020

/** Open a connection to SMTP server.

    If server is NULL it opens pipe ( see zpopen ) to ``sendmail -bs''.
    Else if server is ``local:command'' then opens pipe into shell
    command. This command should behave like SMTP server. If server is
    ``remote:[host_name|ip_address]'' function opens TCP/IP
    connection to SMTP server on this host with port 25.

    If everything is OK it writes MAIL FROM: and RCPT TO: to server with
    specified sender and recipient(separated with commas) values.
    Then sends DATA\r\n, and rest of connection is for user.

    To finish connection use zmailclose.

    @param fd		returned stream descriptors
    @param server	name or IP adress of server host
    @param sender	e-mail adress of sender
    @param recipient	e-mail adress of recipient

*/
ZCEXPORT ZSTREAM    zmailopen(const char* server,const char* sender,const char* recipient);

ZCEXPORT int    zmailclose(ZSTREAM f);

/** Open stream for reading file from http server.

    If options == 1 then it reads HTTP response header and returns
    result in *result integer, else it leaves header for program read.
    This function may be unreliable, because it blocks process when
    calling gethostbyname, and connect routines.

    @param httpfilename	name of file to retrieve full name with http:// on start
    @param options	currently only 0 or 1
    @param result	http transaction result
*/

ZCEXPORT ZSTREAM    zhttpopen(const char* httpfilename,int options,int* result);

/** Create a stream with different input & output streams.

    All read/write calls to returned stream will be equal
    to call read/write on in/out streams.

    Now, seek is impossible.

    @param in		input stream for zread
    @param out		output stream for zwrite
*/
ZCEXPORT ZSTREAM    zdscreat(ZSTREAM in,ZSTREAM out);

/** Create stream for memory buffer.

    Creates empty buffer with specified size.

    This stream is like normal file descriptor, you can use
    zread,zwrite,zseek ( if it's permitted by open mode ).

    @param capacity	initial size of buffer
    @param datasize	size of data in buffer, if previously allocated else 0
    @param buffer	pointer to buffer or 0 if have to be malloced
    @param mode		mode of opening
    @param flags	flags of memory buffer
*/
ZCEXPORT ZSTREAM    zmbopen(
    int capacity,
    int datasize,
    void* buffer,
    int mode,
    int flags);

/** Opens stream for io.

    Creates memory buffer data containing information about string
    and permissions, its size.

    If you want to read from string S as it is a stream call:
       s = zstropen((char*)S,0,ZO_READ,ZMB_STATIC)

    If you want create a string using file IO in buffer S
    (which is size L) call:
	s = zstropen(S,L,ZO_WRITE,ZMB_STATIC).

    @param str		string
    @param size		size of buffer pointed by str
    @param mode		IO mode
    @param flags	flags of memory buffer ( ZMB_xxx )
*/


ZCEXPORT ZSTREAM    zstropen(
    char* str,
    int size,
    int mode,
    int flags);
/*@}*/

#define ZCON_IN	    0
#define ZCON_OUT    1
#define ZCON_ERR    2

ZCEXPORT ZSTREAM    zpio_getcon(int stream_id);
ZCEXPORT void    zpio_freecon();

#define ZFOP_POSIX	0
#define ZFOP_ANSI	1
#define ZFOP_WIN32	2


ZCEXPORT    int*	    zpio_get_zfopen_preffered(void) /* default = ZFOP_POSIX */;
#define zfopen_preffered	(* zpio_get_zfopen_preffered () )


ZCEXPORT    int	    zmbtrunc(ZSTREAM s);

/**@name Memory buffer open mode

    These constants may be used to define behavior of memory buffer.

    The are used as flags argument for zmbopen and zstropen.

    @see zmbopen
    @see zstropen
*/
/*@{ */
/** Memory buffer can't be reallocated or freed.

    mean that buffer specified in zmbopen call is static, and
    free or realloc can't be called on it
    @see zmbopen
 */
#define ZMB_STATIC     0x0100

/** Memory buffer can be reallocated or freed.

    Mean that library can use functions free,malloc or realloc on buffer.

    If the buffer argument in zmbopen is NULL, then buffer is automaticaly allocated.
    @see zmbopen
*/
#define ZMB_DYNAMIC    0x0200

/** Memory buffer grows as needed.

    Mean that buffer is growable, and when write pointer exceed size,
    buffer will grow (fmb_size*=2) to get enough of space
    @see zmbopen
*/
#define ZMB_GROWABLE   0x0400
/*@} */


/**@name zfctl operation codes */
/*@{ */
/** Get read descriptor.
    If there is no file descriptor for reading
    associated with stream returns -1.

*/
#define ZFCTL_GET_READ_DESC	0x0001

/** Get write descriptor.
    If there is no file descriptor for writing
    associated with stream returns -1.
*/
#define ZFCTL_GET_WRITE_DESC	0x0002

/** Get read stream.
    Returns ZSTREAM for reading.
    Usage:
	(ZSTREAM)zfctl(stream,ZFCTL_GET_READ_STREAM,0);

*/
#define ZFCTL_GET_READ_STREAM	0x0003
/** Get write stream.
    Returns ZSTREAM for writing.
    Usage:

	(ZSTREAM)zfctl(stream,ZFCTL_GET_WRITE_STREAM,0);
*/
#define ZFCTL_GET_WRITE_STREAM	0x0004

/** Get blocking mode of stream.

    Returns 1 if stream is in non-blocking mode.
*/
#define ZFCTL_GET_BLOCKING	0x0005

/** Set blocking mode of stream.

    If data is 1 sets stream into non-blocking mode, else to blocking.
*/
#define ZFCTL_SET_BLOCKING	0x0006

/** Test end of file.

    If EOF occured zfctl returns 1 else 0, -1 when an error
    ocurred.
*/
#define ZFCTL_EOF		0x0007

/** Set read stream.
    Returns nothing
    Usage:
	zfctl(stream,ZFCTL_SET_READ_STREAM,new_stream);

*/
#define ZFCTL_SET_READ_STREAM	0x0008

/** Set write stream.
    Returns nothing
    Usage:

	zfctl(stream,ZFCTL_SET_WRITE_STREAM,new_stream);
*/
#define ZFCTL_SET_WRITE_STREAM	0x0009

/**
*/
#define ZFCTL_DUP               0x000a
/*@} */
/**




*/
ZCEXPORT ZSTREAM    zcipher_xor(ZSTREAM f,unsigned long key);

/** Returns descriptor if possible.

    Note: if two descriptors are used for stream
    (eg one for writing, one for reading) it returns
    one for reading.
 */
ZCEXPORT int    zfdesc(ZSTREAM f);
#define zfdesc(a) zfdesc_r((a))

/** Returns name of stream.
*/
ZCEXPORT const char*    zfname(ZSTREAM f);

/** Sets name of stream.
*/
ZCEXPORT int    zfname_set(ZSTREAM f,const char* name);

/** Returns type of stream. OBSOLETE !.
    Always returns 0.
*/
ZCEXPORT int        zftype(ZSTREAM f);

/** Returns read descriptor.
    If stream is for writing only, it returns -1.
*/
ZCEXPORT int    zfdesc_r(ZSTREAM f);

/** Returns write descriptor.
    If stream is for reading only, it returns -1.
*/

ZCEXPORT int    zfdesc_w(ZSTREAM f);

#define zfdesc_r(a) zfctl((a), ZFCTL_GET_READ_DESC, 0 )
#define zfdesc_w(a) zfctl((a), ZFCTL_GET_WRITE_DESC, 0 )

/** Formatted output.
    Prints a formatted output to stream.

    @param fmt		printf-like format string
    @param fdesc	stream
*/
ZCEXPORT int    zfprintf(ZSTREAM fdesc,const char* fmt,...);

/** Formatted output to zstdout.
    Works like zfprintf(zstdout,fmt,...)

    @param fmt		printf-like format string
*/
ZCEXPORT int    zprintf(const char* fmt,...);

/** Formatted output to zstdout.

    @param fmt		printf-like format string
*/
ZCEXPORT int    zvfprintf(ZSTREAM fdesc,const char* fmt,va_list vl);

#define ZUMODE_OUT   0
#define ZUMODE_ERR   1
#define ZUMODE_WARN  2
#define ZUMODE_DBG   3  /* on
                            unix    it's equal to stderr
                            win32   it sends string into the debugger
                        */
/** Formatted output to user.

    @param  umode one of ZUMODE_OUT | ZUMODE_ERR | ZUMODE_WARN
    @param fmt    printf-like format string
*/
ZCEXPORT void       zuprintf(int umode,const char* fmt,...);

/** Formatted output to user.

    @param umode one of ZUMODE_OUT | ZUMODE_ERR | ZUMODE_WARN
    @param fmt	format
    @param ap	printf-like format string
*/

ZCEXPORT void       zvuprintf(int umode,const char* fmt,va_list ap);

/** Read line from stream.

    Reads character to buffer until end of line or EOF reached.
    '\n' character is not put into the buffer.

    @param fdesc	stream
    @param buf		adress of buffer
    @param len		maximum length of readed line eg. size of buffer

    @return
	on error ZR_ERR , errno is set
	if ok returns number of characters read counting  '\n' or '\r'.
*/
ZCEXPORT int    zfgets(ZSTREAM fdesc,char* buf,int len);

/** Read line from zstdin.

    Reads character to buffer until end of line or EOF reached.
    '\n' character is not put into the buffer.

    @param fdesc	stream
    @param buf		adress of buffer
    @param len		maximum length of readed line eg. size of buffer

    @return
	on error ZR_ERR , errno is set
	if ok returns number of characters read counting  '\n' or '\r'.
*/
ZCEXPORT int    zgets(char* buf,int len);

/** Read a char.
    Function reads one char ( 1 byte ) from stream, and returns it casted
    to (int)(unsigned char). On EOF returns -1.

    @param fdesc	stream
    @return
	on error EOF , errno is set
*/
ZCEXPORT int    zfgetc(ZSTREAM fdesc);

/** Read a char from zstdin.
    Function reads one char  ( 1 byte ) from zstdin, and returns it casted
    to (int)(unsigned char). On EOF returns -1.

    @param fdesc	stream
    @return
	on error ZR_ERR , errno is set
*/
ZCEXPORT int    zgetc(void);
#define zgetc() zfgetc(zstdin)

/** Return char to stream.

    Return char to stream.

    EOF shouldn't be passed to ungetc.

    ungetc shouldn't be mixed with zseek.

    @param fdesc	stream
    @param c		char returned
*/
ZCEXPORT int    zungetc(ZSTREAM f,int c);

ZCEXPORT int    zputc(int c);
ZCEXPORT int    zfputc(ZSTREAM s,int c);


ZCEXPORT int    zputs(const char* buf);
ZCEXPORT int    zfputs(ZSTREAM s,const char* buf);


/** Clears error flag.
    Set f_error value to 0.

    @param fdesc	stream
    @return always 0
*/
ZCEXPORT int         zclerr(ZSTREAM fdesc);
#define  zclerr(a) (a ? (((ZSTREAM)(a))->error = 0), (0) : errno=EINVAL,(-1) )

/**
    Sets blocking mode.

    Sets blocking mode of stream. If block_state <> 0 sets to blocking mode
    else sets to non-blocking mode. If stream doesn't support
    this option returns EBADF.

    @param f		stream
    @param block_state	requested state
    @return 0 on succes, ZR_ERR on error (errno is set)

*/
ZCEXPORT int    zset_blocking(ZSTREAM f,int block_state);

#define zset_blocking(a,b) ( zfctl((a),ZFCTL_SET_BLOCKING,(b)) )

/**
    Sets blocking mode for descriptor.

    Sets blocking mode of descriptor. If mode_state <> 0 sets to
    blocking else sets to non-blocking mode. If stream doesn't
    support this option returns EBADF.

    @param f		stream
    @param block_state	requested state
    @return 0 on succes, ZR_ERR on error (errno is set)
*/
ZCEXPORT int    zset_blocking_desc(int desc,int block_state);

/* Note that functions defined as macros, don't have to be
   macros in future */

/** Check EOF.

    Returns 1 if last byte was read from stream.

    NOT TESTED,probably buggy.

    @param fdesc	stream
*/
ZCEXPORT int    zeof(ZSTREAM fdesc);

/** Print error.

    Prints a error message depening on errno value to zstderr.

    Equal to zfprintf(zstderr,"%s: %s\n",msg,zstrerror(errno));

    @param msg		name of message
*/
ZCEXPORT void    zperror(const char* msg);

/** Get error name.

    Returns static name of error defined for this code.

    Error code can be value of errno after unsuccesfull operation.

    @param errcode	code of error
*/
ZCEXPORT const char*    zstrerror(int errcode);

ZCEXPORT ZSTREAM    zpio_get_std_stream(int i);

/** Standard input stream.

Default set do descriptor 0. Functions zgetc,zgets, etc reads from it.
*/
#define zstdin (zpio_get_std_stream(0))
/** Standard output stream.

Default set do descriptor 1. Functions zprintf, etc writes to it.
*/
#define zstdout (zpio_get_std_stream(1))
/** Diagnostic output stream.

Default set do descriptor 2. zperror writes to it.
*/
#define zstderr (zpio_get_std_stream(2))

/** Those below are not ready */

/**@name NEW, not tested functions.
    Those below are not testes.
*/

/*@{ */
/** Find file sarching in more athan one saech paths.

  @param path	vector of `path' strings, terminatet with NULL
  @param file	name of file
  @param type	type of search Zx_OK
  @param buf	buffer where file name will be stored
  @param max	length of buf
*/

ZCEXPORT int    zpio_find_file_by_paths(
    const char* path[],
    const char* file,
    int		type,
    char* 	buf,
    size_t 	max);

/** Find file searching path.

  @param path	path string
  @param file	name of file
  @param type	type of search Zx_OK
  @param buf	buffer where file name will be stored
  @param max	length of buf
*/

ZCEXPORT int    zpio_find_file_by_path(
    const char* path,
    const char* file,
    int		type,
    char* 	buf,
    size_t 	max);

/** Check if path is absolute.
    Return 1 if path is absolute, else 0.
*/
ZCEXPORT int    zpio_path_is_abs(const char* path);

/** Check if name has extension.
    Return 1 if has, else 0.
*/
ZCEXPORT int    zpio_file_has_ext(const char* name);

/** Set extension in filename.

    Set extension in filename, and store whole filename in buf.
*/
ZCEXPORT int    zpio_file_set_ext(const char* name,const char* ext,char* buf,int max);

/** Convert path to current system format.
*/
ZCEXPORT void    zpio_path_to_sys(char* name,int len);

/** Convert path to portable format.
*/
ZCEXPORT void    zpio_path_to_portable(char* name,int len);

/** Test if file is accessible.

    @param path	    path to file
    @param type	    requested access using Zx_OK constants
    @return 0 if file is accessible, -1 on error, errno is set
*/
ZCEXPORT int    zpio_access(const char* path,int type);

/* Create directory.
*/
ZCEXPORT int    zpio_mkdir(const char* name);

/* Create directory structure.
*/
ZCEXPORT int    zpio_mkpath(const char* name);

ZCEXPORT int    zpio_rmdir(const char* name,int recursive);

ZCEXPORT int    zpio_rm(const char* name);

#if !defined R_OK
#if defined ZDEV_BORLAND || defined ZDEV_VC
        /* borland and MS have these settings */
#define ZR_OK	2
#define ZW_OK	4
#define ZX_OK	1
#define ZF_OK	0
#else
        /* default settings */
#define ZR_OK	2
#define ZW_OK	4
#define ZX_OK	1
#define ZF_OK	0
#endif /* now it defaults to those from MSC & BC */
#else /* haven't got x_OK constants */
#define ZR_OK	R_OK
#define ZW_OK	W_OK
#define ZX_OK	X_OK
#define ZF_OK	F_OK
#endif /* have got x_OK constants */


#define ZPIO_ALLOW	1
#define ZPIO_DENY	0

#define Z_PERSON_OWNER	"owner"
#define Z_PERSON_GROUP	"group"
#define Z_PERSON_OTHER	"other"
#define Z_PERSON_ALL	"all"

#define Z_ACCESS_READ	04
#define Z_ACCESS_WRITE	02
#define Z_ACCESS_EXEC	01

#define Z_ACCESS_SUID	0x10
#define Z_ACCESS_SGID	0x20

#define Z_ACCESS_ALL	Z_ACCESS_READ | Z_ACCESS_WRITE | Z_ACCESS_EXEC
/**
*/
ZCEXPORT int    zpio_chmod_ex(const char* path,int permit,char* who,int what);


/*
    Binary byteorder unsafe  input/output.
*/

ZCEXPORT int    zbwrite_l(ZSTREAM f,long* l);
ZCEXPORT int    zbwrite_lv(ZSTREAM f,const long* v,size_t count);

ZCEXPORT int    zbwrite_i(ZSTREAM f,int* i);
ZCEXPORT int    zbwrite_iv(ZSTREAM f,const int* v,size_t count);

ZCEXPORT int    zbwrite_h(ZSTREAM f,short* h);
ZCEXPORT int    zbwrite_hv(ZSTREAM f,const short* v,size_t count);

ZCEXPORT int    zbwrite_c(ZSTREAM f,char* c);
ZCEXPORT int    zbwrite_cv(ZSTREAM f,const char* s,size_t count);

ZCEXPORT int    zbwrite_s(ZSTREAM f,const char** c);

ZCEXPORT int    zbread_l(ZSTREAM f,long* l);
ZCEXPORT int    zbread_lv(ZSTREAM f,long* v,size_t count);

ZCEXPORT int    zbread_i(ZSTREAM f,int* i);
ZCEXPORT int    zbread_iv(ZSTREAM f,int* v,size_t count);

ZCEXPORT int    zbread_h(ZSTREAM f,short* h);
ZCEXPORT int    zbread_hv(ZSTREAM f,short* v,size_t count);

ZCEXPORT int    zbread_c(ZSTREAM f,char* c);
ZCEXPORT int    zbread_cv(ZSTREAM f,char* s,size_t count);

ZCEXPORT int    zbread_s(ZSTREAM f,char** c);

/*
    Binary  byteorder safe input/output.
*/
ZCEXPORT int    znwrite_l(ZSTREAM f,unsigned long* l);
ZCEXPORT int    znwrite_lv(ZSTREAM f,const unsigned long* v,size_t count);

ZCEXPORT int    znwrite_h(ZSTREAM f,unsigned short* h);
ZCEXPORT int    znwrite_hv(ZSTREAM f,const unsigned short* v,size_t count);

ZCEXPORT int    znwrite_s(ZSTREAM f,const char** c);

ZCEXPORT int    znread_l(ZSTREAM f,unsigned long* l);
ZCEXPORT int    znread_lv(ZSTREAM f,unsigned long* v,size_t count);

ZCEXPORT int    znread_h(ZSTREAM f,unsigned short* h);
ZCEXPORT int    znread_hv(ZSTREAM f,unsigned short* v,size_t count);

ZCEXPORT int    znread_s(ZSTREAM f,char** c);

ZCEXPORT unsigned long     zntohl(unsigned long i);
ZCEXPORT unsigned short    zntohs(unsigned short i);
ZCEXPORT unsigned long     zhtonl(unsigned long i);
ZCEXPORT unsigned short    zhtons(unsigned short i);

typedef struct zcconv_table_byte {
    char    c[256];
} zcconv_table_byte;

typedef unsigned short zwchar;
typedef unsigned char  zchar;

typedef struct zcconv_unicode {
    zwchar u;
    zchar  b;
} zcconv_unicode;


#define ZCC_ANSI            0
#define ZCC_UNICODE         1
#define ZCC_UCS2	    1
#define ZCC_UTF             2
#define ZCC_UTF8            2

typedef struct zcconv_t {
    char*       enc_src;
    char*       enc_dest;
    int         type_src;
    int             type_dest;
    size_t          len;
    zchar*          conv_byte;
    zwchar*         conv_byte2unicode;
    zcconv_unicode* conv_unicode;
    size_t          conv_unicode_size;
} zcconv_t;

ZCEXPORT zcconv_t*      zcconv_new(const char* from, const char* to);
ZCEXPORT void           zcconv_free(zcconv_t* cc);

ZCEXPORT size_t         zcconv_dest_length(zcconv_t* cc, size_t src_len);
/*
    gets size of requested buffer
*/
ZCEXPORT size_t         zcconv_dest_size(zcconv_t* cc, const void* src, size_t src_len);

/*
    do a conversion
    Treat src as type of source in zcconv_t and so dest.
*/
ZCEXPORT int            zcconv(zcconv_t* cc, const void* src, void* dest,size_t length);

/*
* zcharconv
*
*	This is an old style char conversion "library". Which was internally coded
*       in chconv.c
*
*	These definitions are taken from "chconv.h" which is now deprecated.
*/
typedef unsigned char	zcharconv_table[256];

#define ZCHC_PLAIN	0
#define ZCHC_WIN_1250	1
#define ZCHC_ISO_8859_2	2
#define ZCHC_DOS_852	3

ZCEXPORT ZSTREAM    zcharconv(ZSTREAM f,zcharconv_table input,zcharconv_table output);
ZCEXPORT void    zcharconv_table_init(zcharconv_table t);

ZCEXPORT int    zcharconv_get(zcharconv_table t,int src_code,int dest_code);

#if 0
#include "zlinkdll.h"

void zpio_set_alloc_vtable(zld_alloc_vtable* vt);
void zpio_get_alloc_vtable(zld_alloc_vtable* vt);
#endif

/* errno codes for zpio & zcompat library*/
#define ZE_FIRST 1000

#define ZEINHOST	ZE_FIRST+1
#define ZENOUSER	ZE_FIRST+2
#define ZEHTTP_NOENT	ZE_FIRST+3
#define ZEHTTP_PERM	ZE_FIRST+4
#define ZEHTTP_INTERNAL	ZE_FIRST+5
#define ZBADADDR	ZE_FIRST+6
#define ZE_FROM_WINDOWS ZE_FIRST+7

#ifdef ZSYS_WIN
#ifndef ZST_WSA_ERROR_COUNT
#define ZST_WSA_ERROR_COUNT 1100
#endif
#endif /* ZSYS_WIN */

#ifndef NOT_READY
#define NOT_READY	{ zfprintf(zstderr,"Called unready function at %s:%i,(%s)\n",__FILE__,__LINE__,"f:");exit(1); }
#endif

ZCEXPORT extern void    zpio_free(void*);

#define ZRDT_NONE	0
#define ZRDT_CHAR	1
#define ZRDT_SPN	2
#define ZRDT_CSPN	3

/*
    This thre real read function.

    et:
	one of ZRDT_xxx
	    ZRDT_NONE	nothing is done for testing
	    ZRDT_CHAR   end_test = (void*) ending_character
	    ZRDT_SPN    end_test = (void*) "accepted characters"
    ZRDT_CSPN   end_test = (void*) "rejected characters"
*/
ZCEXPORT int        _zread(ZSTREAM f,char* db,int requested_size,int et,void* end_test);

#ifdef __cplusplus
};
#endif

#endif /* __zpio_h_ */












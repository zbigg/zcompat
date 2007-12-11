/*
* File: ansi.c
*
* Id:           $Id: ansi.c,v 1.5 2003/06/28 11:57:44 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  ANSI I/O (ISO C) routines
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fcntl.h>
#include <string.h>
#include <errno.h>

#if( defined ZSYS_WIN || defined ZSYS_MSDOS) && !defined ZDEV_DJGPP
#include <io.h>     /* for dup */
#include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>

#include "zcompat/zpio.h"
#include "zpio_i.h"


#define ZPIO_ANSI 1

/* ANSI streams - fopen ... */
ZSTREAM	zansi_open	(ZSTREAM,const char*,int);
int	zansi_close	(ZSTREAM);
int	zansi_read	(ZSTREAM,void*,size_t);
int	zansi_write	(ZSTREAM,const void*,size_t);
zoff_t	zansi_seek	(ZSTREAM,zoff_t,int);
long	zansi_ctl	(ZSTREAM,int,void*);

zstream_vtable_t zvt_ansi = {
    3,			/*__ id */
    1,			/*__ data_size */
    zansi_open,		/*__ open */
    zansi_close,	/*__ close */
    zansi_read,		/*__ read */
    zansi_write,	/*__ write */
    zansi_seek,		/*__ seek */
    zansi_ctl,		/*__ ctl */
};

static const char*	zpio_openflags_2_ansi_openflags(int zf);

/* ------------------
             PUBLIC
            -----------------*/
/* ----------------------====*****      ANSI
*/
#define ansi_f (f->data[0])

ZSTREAM	zansi_open	(ZSTREAM f,const char* name,int mode)
{
#if ZPIO_ANSI
    f->mode = mode;
    if( name != NULL ) {
	ansi_f = (long)fopen(name,zpio_openflags_2_ansi_openflags(mode));
	if( ansi_f == 0 )
	    return NULL;
	f->name = (char*)zcompat_strdup(name);
    } else {
	ansi_f = mode;
	f->name = NULL;
    }
    ZFL_SET(f->flags,ZSF_FILE);
    return f;
#else
    errno = ENODEV;
    return NULL;
#endif
}
int	zansi_close(ZSTREAM f)
{
#if ZPIO_ANSI
    if( ZFL_ISSET(f->flags,ZSF_PUBLIC) )
	return 0;
    return ( fclose((FILE*) ansi_f ) == EOF )
	? -1
	: 0;
#else
    errno = ENODEV;
    return ZR_ERROR;
#endif
}
int	zansi_read	(ZSTREAM f,void* buf,size_t size)
{
#if ZPIO_ANSI
    size_t r;
    if( (r = fread( buf,1,size, (FILE*) ansi_f )) < size ) {
	if( feof( (FILE*)ansi_f ) )
	    f->eof = 1;
	else  {
	    f->error = errno;
	    clearerr( (FILE*)ansi_f);
	    return ZR_ERROR;
	}
	return (int)r;
    }
    return (int)r;
#else
    errno = ENODEV;
    return -1;
#endif
}
int	zansi_write(ZSTREAM f,const void* buf,size_t size)
{
#if ZPIO_ANSI
    size_t r;
    if( (r = fwrite( buf,1,size, (FILE*) ansi_f )) < size ) {
	if( ferror( (FILE*) ansi_f) ) {
	    f->error = errno;
	    clearerr((FILE*) ansi_f);
	    return ZR_ERROR;
	}
	return 0;
    }
    return (int)r;
#else
    errno = ENODEV;
    return -1;
#endif
}
zoff_t	zansi_seek	(ZSTREAM f,zoff_t offset,int whence)
{
#if ZPIO_ANSI
    if( fseek( (FILE*) ansi_f, offset, whence ) != 0 ) {
	f->error = errno;
	clearerr((FILE*) ansi_f);
	return (zoff_t)-1;
    }
    return (zoff_t)ftell((FILE*) ansi_f);
#else
    errno = ENODEV;
    return (zoff_t)-1;
#endif
}
#ifdef ZDEV_MSC
extern int dup(int);
#endif

long	zansi_ctl	(ZSTREAM f,int what,void* data)
{
#if ZPIO_ANSI
    int d;
    d = fileno( (FILE*) ansi_f );

    switch( what ) {
    case ZFCTL_DUP:
        {
            int fd = dup(d);
            FILE* af;
            if( fd < 0 ) return -1;
            af = fdopen(d,zpio_openflags_2_ansi_openflags(f->mode));
            return (long)zansiopen(af,f->name);
        }

    case ZFCTL_GET_READ_DESC:
	return ZFL_ISSET(f->mode,ZO_READ)
	    ? d
	    : -1;

    case ZFCTL_GET_WRITE_DESC:
	return ZFL_ISSET(f->mode,ZO_WRITE)
	    ? d
	    : -1;

    case ZFCTL_GET_READ_STREAM:
	return ZFL_ISSET(f->mode,ZO_READ)
	    ? (int)f
	    : (int)NULL;

    case ZFCTL_GET_WRITE_STREAM:
	return ZFL_ISSET(f->mode,ZO_WRITE)
	    ? (int)f
	    : (int)NULL;

    case ZFCTL_GET_BLOCKING:
	{
#ifdef O_NONBLOCK
	    int fl = fcntl( d, F_GETFL );
	    if( fl < 0 )
		return -1;
	    return (( fl & O_NONBLOCK ) == O_NONBLOCK) ? 1 : 0;
#else
	    errno = EINVAL;
	    return -1;
#endif
	}
    case ZFCTL_SET_BLOCKING:
	{
#ifdef O_NONBLOCK
	    int fl = fcntl( d, F_GETFL );
	    if( fl < 0 )
		return -1;
	    if( (int)data )
		fl |= O_NONBLOCK;
	    else
		fl &= ~(O_NONBLOCK);
	    if( fcntl( d, F_SETFL ,fl ) < 0 )
		return -1;
	    return 0;
#else
	    errno = EINVAL;
	    return -1;
#endif
	}

    case ZFCTL_EOF:
	return f->eof = feof( (FILE*) ansi_f );
    }
    errno = EINVAL;
    return -1;
#else /* ZPIO_ANSI */
    errno = ENODEV;
    return -1;
#endif /* ZPIO_ANSI */
}

static char buf[20] = "";
static const char*	zpio_openflags_2_ansi_openflags(int zf)
{
    int i = 0;
    buf[0] = 0;

#define cadd(a) { buf[i++] = (a); buf[i] = 0; }
#define cset(a) { strcpy(buf,(a)); i = strlen(buf); }
#define fset(b) ( ((zf) & (b)) == (b) )

    if( fset(ZO_READ) && fset( ZO_WRITE ) ) {
	if     ( fset( ZO_TRUNC ) && fset( ZO_CREAT) )
	    cset("w+")
	else if( fset( ZO_APPEND ) )
	    cset("a+")
	else
	    cset("r+");
    } else if ( fset( ZO_READ ) ) {
	cset("r")
    } else if ( fset( ZO_WRITE ) ) {

	if ( fset( ZO_TRUNC ) && fset( ZO_CREAT) )
	    cset("w")
	else if( fset( ZO_APPEND ) && fset( ZO_CREAT) )
	    cset("a")
	else
	    cset("r+")
    }
    if( fset( ZO_BINARY ) )
	cadd('b')
    if( fset( ZO_TEXT ) )
	cadd('t')
#undef  fset
#undef  cadd
#undef  cset
    return buf;
}
/*
    ansi compatibility layer

    if 
*/

ZSTREAM zpioc_fopen(const char * fileName, const char *mode);
ZSTREAM zpioc_fdopen(int fileNo, const char *mode);
ZSTREAM zpioc_freopen(const char *fileName, const char *mode, ZSTREAM f);
ZSTREAM zpioc_popen(const char *command, const char *mode);
ZSTREAM zpioc_tmpfile(void);

int	zpioc_fclose(ZSTREAM f);
int	zpioc_pclose(ZSTREAM f);

size_t	zpioc_fread(void *buffer, size_t size, size_t count, ZSTREAM f);
size_t	zpioc_fwrite(const void *buffer, size_t size, size_t count, ZSTREAM f);

void	zpioc_clearerr(ZSTREAM f);
int	zpioc_feof(ZSTREAM f);
int	zpioc_ferror(ZSTREAM f);
int	zpioc_fflush(ZSTREAM f);
int	zpioc_fileno(ZSTREAM f);

int	zpioc_fgetpos(ZSTREAM f, fpos_t * pos);
int	zpioc_fsetpos(ZSTREAM f, const fpos_t * pos);
int	zpioc_fseek(ZSTREAM f, long offset, int whence);
long	zpioc_ftell(ZSTREAM f);
void	zpioc_rewind(ZSTREAM f);

void	zpioc_setbuf(ZSTREAM f, char *buffer);
int	zpioc_setvbuf(ZSTREAM f, char *buf, int mof, size_t size);

int	zpioc_getc(ZSTREAM f);
int	zpioc_getchar(void);
int	zpioc_fgetc(ZSTREAM f);
int	zpioc_ungetc(int c, ZSTREAM f);
int	zpioc_fputc(int c, ZSTREAM f);
int	zpioc_putc(int c, ZSTREAM f);
int	zpioc_putchar(int c);

int	zpioc_puts(const char * s);
char*	zpioc_gets(char *s);
char*	zpioc_fgets(char *s, int max, ZSTREAM f);
int	zpioc_fputs(const char * s, ZSTREAM f);

int	zpioc_fprintf(ZSTREAM f, const char * fmt, ...);
int	zpioc_fscanf(ZSTREAM f, const char * fmt, ...);
void	zpioc_perror(const char *message);
int	zpioc_printf(const char *fmt, ...);
int	zpioc_vfprintf(ZSTREAM f, const char *fmt, va_list ap);
int	zpioc_vprintf(const char * fmt, va_list ap);

int	zpioc_scanf(const char *fmt, ...);


ZSTREAM zpioc_fopen(const char * fileName, const char *mode)
{
    return NULL;
}

ZSTREAM zpioc_fdopen(int fileNo, const char *mode)
{
    return NULL;
}

ZSTREAM zpioc_freopen(const char *fileName, const char *mode, ZSTREAM f)
{
    return NULL;
}

ZSTREAM zpioc_popen(const char *command, const char *mode)
{
    return NULL;
}

ZSTREAM zpioc_tmpfile(void)
{
    return NULL;
}


int	zpioc_fclose(ZSTREAM f)
{
    return -1;
}
int	zpioc_pclose(ZSTREAM f)
{
    return -1;
}

size_t	zpioc_fread(void *buffer, size_t size, size_t count, ZSTREAM f)
{
    int rsize = size*count;
    int result = zread(f,buffer, size*count);
    return result >= 0 
	? result / size
	: 0;
}
size_t	zpioc_fwrite(const void *buffer, size_t size, size_t count, ZSTREAM f)
{
    int rsize = size*count;
    int result = zwrite(f,buffer, size*count);
    return result >= 0 
	? result / size
	: 0;
}

void	zpioc_clearerr(ZSTREAM f)
{
    f->error = 0;
}
int	zpioc_feof(ZSTREAM f)
{
    return zeof(f);
}
int	zpioc_ferror(ZSTREAM f)
{
    return f->error;
}
int	zpioc_fflush(ZSTREAM f)
{
    return -1; //zflush(f);
}
int	zpioc_fileno(ZSTREAM f)
{
    return zfctl(f,ZFCTL_GET_READ_DESC,NULL);
}

int	zpioc_fgetpos(ZSTREAM f, fpos_t * pos)
{
    zoff_t off = zseek(f,0,ZSEEK_CUR);
    if( off == (zoff_t)-1 )
	return -1;
    *pos = (fpos_t)pos;
    return 0;
}

int	zpioc_fsetpos(ZSTREAM f, const fpos_t * pos)
{
    zoff_t opos = (zoff_t)*pos;
    return zseek(f,opos, ZSEEK_SET);
}

int	zpioc_fseek(ZSTREAM f, long offset, int whence)
{
    return zseek(f,offset,whence);
}

long	zpioc_ftell(ZSTREAM f)
{
    return zseek(f,0,ZSEEK_CUR);
}

void	zpioc_rewind(ZSTREAM f)
{
    zseek(f,0,ZSEEK_SET);
}

void	zpioc_setbuf(ZSTREAM f, char *buffer)
{
}

int	zpioc_setvbuf(ZSTREAM f, char *buf, int mof, size_t size)
{
    return -1;
}

int	zpioc_getc(ZSTREAM f)
{
    return zfgetc(f);
}

int	zpioc_getchar(void)
{
    return zfgetc(zstdin);
}

int	zpioc_fgetc(ZSTREAM f)
{
    return zfgetc(f);
}

int	zpioc_ungetc(int c, ZSTREAM f)
{
    return zungetc(f,c);
}

int	zpioc_fputc(int c, ZSTREAM f)
{
    return zfputc(f,c);
}

int	zpioc_putc(int c, ZSTREAM f)
{
    return zfputc(f,c);
}

int	zpioc_putchar(int c)
{
    return zfputc(zstdout,c);;
}

int	zpioc_puts(const char * s)
{
    return zputs(s);
}

char*	zpioc_gets(char *s)
{
    zfgets(zstdin,s,INT_MAX);
    return s;
}

char*	zpioc_fgets(char *s, int max, ZSTREAM f)
{
    zfgets(f,s,max);
    return s;
}
int	zpioc_fputs(const char * s, ZSTREAM f)
{
    return zfputs(f,s);
}

int	zpioc_fprintf(ZSTREAM f, const char * fmt, ...)
{
    int result;
    va_list ap;
    va_start(ap, fmt);
    result = zvfprintf(f, fmt, ap);
    va_end(ap);
    return result;
}

int	zpioc_fscanf(ZSTREAM f, const char * fmt, ...)
{
    return -1;
}

void	zpioc_perror(const char *message)
{
    zperror(message);     
}

int	zpioc_printf(const char *fmt, ...)
{
    int result;
    va_list ap;
    va_start(ap, fmt);
    result = zvfprintf(zstdout, fmt, ap);
    va_end(ap);
    return result;
}

int	zpioc_vfprintf(ZSTREAM f, const char *fmt, va_list ap)
{
    return zvfprintf(f, fmt, ap);
}

int	zpioc_vprintf(const char * fmt, va_list ap)
{
    return zvfprintf(zstdout, fmt, ap);
}

int	zpioc_scanf(const char *fmt, ...)
{
    return -1;
}

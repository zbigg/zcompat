/*
* File: bsdsock.c
*
* Id:           $Id: bsdsock.c,v 1.7 2003/06/27 20:21:45 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  BSD Sockets I/O routines
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
* To do:
 	(WIN)*	zbsdsocket_ctl - check in win32.hlp how to get
		    blocking state of socket
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zsystype.h"



#include <fcntl.h>
#include <string.h>
#include <errno.h>
#ifdef ZSYS_WIN
#include <winsock.h>
#else
#if defined(ZSYS_UNIX) || defined(ZSYS_NET_SUPPORT)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#endif

#include "zcompat/zcompat.h"
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"
/*
    i've read somewhere that these flags are equal
    library uses O_NONBLOCK
*/

#if defined O_NDELAY && !defined O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

/* UNIX + Windows sockets */
static ZSTREAM	zbsdsocket_open_connect(ZSTREAM,const char*,int);
static ZSTREAM	zbsdsocket_open_tcp_server(ZSTREAM f,const char* rhost,int rport);
static ZSTREAM	zbsdsocket_open_accept(ZSTREAM f,const char* rhost,int rport);

static int	zbsdsocket_close(ZSTREAM);
static int	zbsdsocket_read	(ZSTREAM,void*,size_t);
static int	zbsdsocket_write(ZSTREAM,const void*,size_t);
static long	zbsdsocket_ctl	(ZSTREAM,int,void*);

#if defined ZSYS_NET_SUPPORT
static int in_get_addr(const char* address,int rport, struct sockaddr_in* sa);
static int _close_sock(int sock);
#endif
zstream_vtable_t zvt_bsdsocket_connect = {
    4,			/*__ id */
    1,			/*__ data_size */
    zbsdsocket_open_connect,	/*__ open */
    zbsdsocket_close,	/*__ close */
    zbsdsocket_read,	/*__ read */
    zbsdsocket_write,	/*__ write */
    NULL,		/*__ seek */
    zbsdsocket_ctl,	/*__ ctl */
};
zstream_vtable_t zvt_bsdsocket_tcp_server = {
    4,			/*__ id */
    1,			/*__ data_size */
    zbsdsocket_open_tcp_server,	/*__ open */
    zbsdsocket_close,	/*__ close */
    zbsdsocket_read,	/*__ read */
    zbsdsocket_write,	/*__ write */
    NULL,		/*__ seek */
    zbsdsocket_ctl,	/*__ ctl */
};
zstream_vtable_t zvt_bsdsocket_accept= {
    4,			/*__ id */
    1,			/*__ data_size */
    zbsdsocket_open_accept,	/*__ open */
    zbsdsocket_close,	/*__ close */
    zbsdsocket_read,	/*__ read */
    zbsdsocket_write,	/*__ write */
    NULL,		/*__ seek */
    zbsdsocket_ctl,	/*__ ctl */
};
/* ------------------
             PUBLIC
            -----------------*/

ZSTREAM zsopen(const char* rhost,short rport)
{
    return zcreat(& zvt_bsdsocket_connect, rhost, rport );
}

ZSTREAM zsreopen(int sock_fd)
{
    return zcreat(& zvt_bsdsocket_connect,(const char*)sock_fd, -1 );
}

ZSTREAM zsopen_tcp_server(const char* address,short port)
{
    return zcreat(& zvt_bsdsocket_tcp_server,NULL, port );
}
ZSTREAM zaccept(ZSTREAM server_socket)
{
    return zcreat(& zvt_bsdsocket_accept,(const char*) server_socket, 0 );
}

/* ----------------------====*****      bsdsocket
*/

#ifdef WE_LOVE_BILL
int zst_WSA_ready  = 0;
#endif

static void	zst_set_socket_error(ZSTREAM f);

#if defined ZSYS_NET_SUPPORT

#ifdef ZSYS_WIN
#define ZST_WSA_INIT (  !zst_WSA_ready ? (int)zst_WSA_init() : (int)1 )
static void	zst_WSA_done(void);
static int	zst_WSA_init(void);

int	WSAError2errno(int WSAError);
#else
#define ZST_WSA_INIT ( 1 )
#endif

#endif

#define socket_fd data[0]

static void zst_set_socket_error(ZSTREAM f)
{
#ifdef ZSYS_WIN
    int a = WSAGetLastError();
    errno = WSAError2errno(a);
#endif
    if( f != NULL )
	f->error = errno;
}

static ZSTREAM	zbsdsocket_open_connect	(ZSTREAM f,const char* rhost,int rport)
{
#if defined ZSYS_NET_SUPPORT  || 0

    struct sockaddr_in sa;
    int                sock;

#ifdef ZSYS_WIN
    if( ! ZST_WSA_INIT ) {
	errno = ENODEV;
	return NULL;
    }
#endif

    if( rport != -1 && rhost == NULL ) {
	errno = EINVAL;
	return NULL;
    }
    if( rport == -1 )
	sock = (int)rhost;
    else
	sock = socket(AF_INET,SOCK_STREAM,0);
    if (sock < 0) {
	zst_set_socket_error(NULL);
	return NULL;
    }
    f->mode = ZO_RDWR;
    f->socket_fd = sock;

    if( rport == -1 ) {
	ZFL_SET(f->flags,ZSF_PUBLIC);
	f->name = (char*)zpio_strdup("socket://previously-opened socket");
	return f;
    }
    if( in_get_addr(rhost,rport,&sa) < 0 ) {
        zst_set_socket_error(NULL);
        _close_sock(sock);
        return NULL;
    }

    if( connect(sock,(struct sockaddr*)&sa,sizeof(sa)) < 0 ) {
	zst_set_socket_error(NULL);
        _close_sock(sock);
	return NULL;
    }

    f->name = zsaprintf(NULL,0,"socket://%s:%i",rhost,rport);
    ZFL_UNSET(f->flags,ZSF_FILE);
    return f;
#else /* ZSYS_NET_SUPPORT */
    errno = ENODEV;
    return 0;
#endif
}

static ZSTREAM	zbsdsocket_open_tcp_server(ZSTREAM f,const char* rhost,int rport)
{
    /*
        open a server socket !
            rhost is NULL or in future ASCIIZ name of bind address
            rport is rport
    */
#if defined ZSYS_NET_SUPPORT  || 0

    struct sockaddr_in sa;
    int                sock;

#ifdef ZSYS_WIN
    if( ! ZST_WSA_INIT ) {
	errno = ENODEV;
	return NULL;
    }
#endif

    /* Get the socket */
    if( rport != -1 && rhost != NULL ) {
	errno = EINVAL;
	return NULL;
    }
    if( rport == -1 )
	sock = (int)rhost;
    else
	sock = socket(AF_INET,SOCK_STREAM,0);
    if (sock < 0) {
	zst_set_socket_error(NULL);
	return NULL;
    }
    f->mode = ZO_RDWR;
    f->socket_fd = sock;

    if( rport == -1 ) {
	ZFL_SET(f->flags,ZSF_PUBLIC);
	f->name = zsaprintf(NULL,0,"socket_tcp_server://%i",rport);
    }
    /* Get the bind address */
    memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET;
    if( rhost ) {
        if( in_get_addr(rhost,rport,&sa) < 0 ) {
            /* ERROR: you've passed bad bind address ! */
            zst_set_socket_error(NULL);
            return NULL;
        }
    } else
        sa.sin_port = htons((short)rport);
    
    /* Do a bind */
    if( bind(sock,(struct sockaddr*)&sa,sizeof(sa)) < 0 ) {
        /* ERROR: can't bind, maybe busy ? */
	zst_set_socket_error(NULL);
        _close_sock(sock);
	return NULL;
    }

    /* Do a listen */
    if( listen(sock,5) < 0 ) {
        /* ERROR: very bad error, almost never occurs */
	zst_set_socket_error(NULL);
        _close_sock(sock);
	return NULL;
    }
    f->name = zsaprintf(NULL,0,"socket_tcp_server://%i",rport);
    ZFL_UNSET(f->flags,ZSF_FILE);
    return f;
#else /* ZSYS_NET_SUPPORT */
    errno = ENODEV;
    return 0;
#endif
}


static ZSTREAM	zbsdsocket_open_accept(ZSTREAM f,const char* rhost,int rport)
{
#if defined ZSYS_NET_SUPPORT  || 0

    
    /*struct sockaddr_in sa;*/      /* TODO:*/
    ZSTREAM listen_stream = (ZSTREAM)rhost;
    int     listen_sock = -1;
    int     accept_sock = -1;

#ifdef ZSYS_WIN
    if( ! ZST_WSA_INIT ) {
	errno = ENODEV;
	return NULL;
    }
#endif
    if( !listen_stream ) {
        errno = EINVAL;
        return NULL;
    }
    listen_sock = zfdesc_r(listen_stream);
    if( listen_sock < 0 ) {
        errno = EBADF;
        return NULL;
    }
    accept_sock = accept(listen_sock, NULL, NULL );
    if( accept_sock < 0 ) {
        zst_set_socket_error(NULL);
        return NULL;
    }
    f->mode = ZO_RDWR;
    f->socket_fd = accept_sock;
    
    ZFL_UNSET(f->flags,ZSF_FILE);
    return f;
#else /* ZSYS_NET_SUPPORT */
    errno = ENODEV;
    return 0;
#endif
}


static int	zbsdsocket_close(ZSTREAM f)
{
#if defined ZSYS_NET_SUPPORT
    int e = 0;

    if( ZFL_ISSET(f->flags,ZSF_PUBLIC) )
	return 0;

#ifdef ZSYS_WIN
    e = closesocket(f->socket_fd);
    if( e < 0 )
	zst_set_socket_error(f);
#else /* here UNIX */
    e =  close(f->socket_fd);
    if( e < 0 ) {
	zst_set_socket_error(f);
    }
#endif
    return e;

#else /* no network */
    errno = ENODEV;
    return -1;
#endif
}


static int	zbsdsocket_read	(ZSTREAM f,void* buf,size_t size)
{
#if defined ZSYS_NET_SUPPORT
    int e = recv(f->socket_fd,buf,size,0);
    if( e < 0 ) {
	zst_set_socket_error(f);
	e = Z_ERRNO_2_ZR(f->error);
    }
    if ( e == 0 )
	f->eof = 1;
    return e;
#else /* no network */
    errno = ENODEV;
    return -1;
#endif
}
static int	zbsdsocket_write(ZSTREAM f,const void* buf,size_t size)
{
#if defined ZSYS_NET_SUPPORT
    int e = send(f->socket_fd,buf,size,0);
    if( e < 0 ) {
	zst_set_socket_error(f);
	if( f->error == EAGAIN )
	    e = ZR_AGAIN;
    }
    if ( e == 0 )
	f->eof = 1;
    return e;
#else /* no network */
    errno = ENODEV;
    return -1;
#endif
}
static long	zbsdsocket_ctl	(ZSTREAM f,int what,void* data)
{
#if defined ZSYS_NET_SUPPORT
    switch( what ) {
    case ZFCTL_DUP:
#ifdef ZSYS_WIN
        {

#ifdef USING_WINSOCK_2_BUT_WE_NEVER_USE_IT
            /*
                code for duplicating socket handle on win32
                needs winsock2, and we're using old winsock 1.1
                interface, so for now we don't do anything but report
                that socket can't be duplicated because it doesn't
                support it
            */
            SOCKET s;
            struct WSAPROTOCOL_INFO wsapi;
            if( !WSADuplicateSocket(f->socket_fd,GetCurrentProcessId(),&wsapi))
                return -1;

            s = WSASocket(FROM_PROTOCOL_INFO,
                FROM_PROTOCOL_INFO,FROM_PROTOCOL_INFO,&wsapi,0,0);
            if( s == INVALID_SOCKET ) 
                return -1;
            return zsreopen(s);
#endif
            errno = EBADF;
            return -1;
        }
            
#else       /* assume others are unix-like */
        {
            int fd = dup(f->socket_fd);
            if( fd < 0 ) return -1;
            return (int)zsreopen(fd);
        }
#endif
    case ZFCTL_GET_READ_DESC:
	return f->socket_fd;
    case ZFCTL_GET_WRITE_DESC:
	return f->socket_fd;
    case ZFCTL_GET_READ_STREAM:
	return (int)f;
    case ZFCTL_GET_WRITE_STREAM:
	return (int)f;
    case ZFCTL_GET_BLOCKING:
	{
#ifndef ZSYS_WIN
#ifdef O_NONBLOCK
	    int fl = fcntl( f->socket_fd, F_GETFL );
	    if( fl < 0 )
		return -1;
	    return ( fl & O_NONBLOCK ) == O_NONBLOCK;
#else /* haven't got O_NONBLOCK */
	    errno = EINVAL;
	    return -1;
#endif
#else /* WIN */
	    errno = EINVAL;
	    return -1;
#endif
	}

#ifndef ZSYS_WIN
    case ZFCTL_SET_BLOCKING:
#ifdef O_NONBLOCK
	{
	    int fl = fcntl( f->socket_fd, F_GETFL );
	    if( fl < 0 )
		return -1;
	    if( (int)data )
		fl |= O_NONBLOCK;
	    else
		fl &= ~(O_NONBLOCK);
	    if( fcntl( f->socket_fd, F_SETFL ,fl ) < 0 )
		return -1;
	    return 0;
	}
#else /* haven't got O_NONBLOCK */
	errno = EINVAL;
	return -1;
#endif
#else /* WIN */
    case ZFCTL_SET_BLOCKING:
	{
	    unsigned long _block = !(int)data;
	    return ioctlsocket(f->socket_fd,FIONBIO,&_block);
	}
#endif
    case ZFCTL_EOF:
	return f->eof;
    }
    errno = EINVAL;
    return -1;

#else /* no network */
    errno = ENODEV;
    return -1;
#endif
}

#undef socket_fd

#if defined ZSYS_WIN
#if 0
#ifndef ZST_WSA_ERROR_COUNT
#define ZST_WSA_ERROR_COUNT 1100
#endif

static int wsa2errno[ZST_WSA_ERROR_COUNT] = { 0 };
int	wsa2errno_set = 0;

int	WSAError2errno(int WSAError)
{
    if( !wsa2errno_set ) {

#define _s(errno_v,wsock_v) wsa2errno[ (wsock_v) - WSABASEERR ] = (errno_v)
#ifdef EAGAIN
        _s(EAGAIN		   ,WSAEWOULDBLOCK);
#endif
#ifdef EINPROGRESS
        _s(EINPROGRESS             ,WSAEINPROGRESS);
#endif
#ifdef   EALREADY
        _s(EALREADY                ,WSAEALREADY);
#endif
#ifdef     ENOTSOCK
        _s(ENOTSOCK                ,WSAENOTSOCK);
#endif
#ifdef     EDESTADDRREQ
        _s(EDESTADDRREQ            ,WSAEDESTADDRREQ);
#endif
#ifdef     EMSGSIZE
        _s(EMSGSIZE                ,WSAEMSGSIZE);
#endif
#ifdef     EPROTOTYPE
        _s(EPROTOTYPE              ,WSAEPROTOTYPE);
#endif
#ifdef     ENOPROTOOPT
        _s(ENOPROTOOPT             ,WSAENOPROTOOPT);
#endif
#ifdef     EPROTONOSUPPORT
        _s(EPROTONOSUPPORT         ,WSAEPROTONOSUPPORT);
#endif
#ifdef     ESOCKTNOSUPPORT
        _s(ESOCKTNOSUPPORT         ,WSAESOCKTNOSUPPORT);
#endif
#ifdef     EOPNOTSUPP
        _s(EOPNOTSUPP              ,WSAEOPNOTSUPP);
#endif
#ifdef     EPFNOSUPPORT
        _s(EPFNOSUPPORT            ,WSAEPFNOSUPPORT);
#endif
#ifdef     EAFNOSUPPORT
        _s(EAFNOSUPPORT            ,WSAEAFNOSUPPORT);
#endif
#ifdef     EADDRINUSE
        _s(EADDRINUSE              ,WSAEADDRINUSE);
#endif
#ifdef     EADDRNOTAVAIL
        _s(EADDRNOTAVAIL           ,WSAEADDRNOTAVAIL);
#endif
#ifdef     ENETDOWN
        _s(ENETDOWN                ,WSAENETDOWN);
#endif
#ifdef      ENETUNREACH
        _s(ENETUNREACH             ,WSAENETUNREACH);
#endif
#ifdef     ENETRESET
        _s(ENETRESET               ,WSAENETRESET);
#endif
#ifdef      ECONNABORTED
        _s(ECONNABORTED            ,WSAECONNABORTED);
#endif
#ifdef      ECONNRESET
        _s(ECONNRESET              ,WSAECONNRESET);
#endif
#ifdef      ENOBUFS
        _s(ENOBUFS                 ,WSAENOBUFS);
#endif
#ifdef      EISCONN
        _s(EISCONN                 ,WSAEISCONN);
#endif
#ifdef      ENOTCONN
        _s(ENOTCONN                ,WSAENOTCONN);
#endif
#ifdef     ESHUTDOWN
        _s(ESHUTDOWN               ,WSAESHUTDOWN);
#endif
#ifdef    ETOOMANYREFS
        _s(ETOOMANYREFS            ,WSAETOOMANYREFS);
#endif
#ifdef     ETIMEDOUT
        _s(ETIMEDOUT               ,WSAETIMEDOUT);
#endif
#ifdef     ECONNREFUSED
        _s(ECONNREFUSED            ,WSAECONNREFUSED);
#endif
#ifdef      ELOOP
        _s(ELOOP                   ,WSAELOOP);
#endif
#ifdef       ENAMETOOLONG
        _s(ENAMETOOLONG            ,WSAENAMETOOLONG);
#endif
#ifdef       EHOSTDOWN
        _s(EHOSTDOWN               ,WSAEHOSTDOWN);
#endif
#ifdef     EHOSTUNREACH
        _s(EHOSTUNREACH            ,WSAEHOSTUNREACH);
#endif
#ifdef     ENOTEMPTY
        _s(ENOTEMPTY               ,WSAENOTEMPTY);
#endif
#ifdef     EPROCLIM
        _s(EPROCLIM                ,WSAEPROCLIM);
#endif
#ifdef      EUSERS
        _s(EUSERS                  ,WSAEUSERS);
#endif
#ifdef     EDQUOT
        _s(EDQUOT                  ,WSAEDQUOT);
#endif
#ifdef      ESTALE
        _s(ESTALE                  ,WSAESTALE);
#endif
#ifdef       EREMOTE
        _s(EREMOTE                 ,WSAEREMOTE);
#endif
#undef s_
	wsa2errno_set = 1;
    }
    return wsa2errno[WSAError - WSABASEERR];
}
#else
int	WSAError2errno(int WSAError)
{
    return WSAError;
}
#endif
#endif

#if defined(WE_LOVE_BILL) && defined(ZSYS_NET_SUPPORT)
int zpio_sockets_init()
{
    return zst_WSA_init();
}
static int	zst_WSA_init(void)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    if ( zst_WSA_ready )
	return 1;
    wVersionRequested = MAKEWORD(1, 1);

    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0)
	return 0;

    if ( LOBYTE( wsaData.wVersion ) != 1 ||
	   HIBYTE( wsaData.wVersion ) != 1 ) {
        WSACleanup();
        errno = ENODEV;
        zst_WSA_ready  = 0;
        return 0;
    }
    zst_WSA_ready  = 1;
    zc_atexit(zst_WSA_done);
    return 1;
}

static void    zst_WSA_done(void)
{
    if( zst_WSA_ready )
        zst_WSA_ready = 0,WSACleanup();
}
#endif

/*
    some static functions that are implemented anly if we have net
*/
#if defined ZSYS_NET_SUPPORT
/*
#ifndef HAVE_INET_ATON
int	inet_aton(const char* adr, struct in_addr& ia)
{
    
}
#endif
#ifndef HAVE_GETHOSTBYNAME
struct hostent* gethostbyname(const char* address)
{
    return NULL;
}
#endif
*/
static int in_get_addr(const char* address,int rport, struct sockaddr_in* sa)
{
    struct hostent*    ha;
    struct in_addr     ia;
#ifdef ZSYS_WIN
    unsigned long      ian;
#endif
    memset(sa,0,sizeof(*sa));
    sa->sin_family = AF_INET;
    sa->sin_port = htons((short)rport);

#ifdef ZSYS_WIN
    ian =  inet_addr(address);
    ia.s_addr = ian;
    if( ian == INADDR_NONE ) {
#else /* here UNIX */
    if( !inet_aton(address,&ia) ) {
#endif
        ha = gethostbyname(address);
        if( ha == NULL )
	    return -1;
	    /* found with gethostbyname */
    	memcpy(&sa->sin_addr, ha->h_addr, ha->h_length);
    } else {
        /* found with inet_addr or inet_aton */
        memcpy(&sa->sin_addr,&ia,sizeof(ia));
    }
    return 0;
}

static int _close_sock(int sock) {
#ifdef ZSYS_WIN
    return closesocket(sock);
#else /* here UNIX */
    return close(sock);
#endif
}
int zpio_close_socket(int sock) {
    return _close_sock(sock);
}

#endif





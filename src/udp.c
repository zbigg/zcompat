/*
* File:		udp.c
*
* Id:           $Id$
*
* Project:	zcompat/zpio
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*   This program is Copyright(c) Zbigniew Zagorski 2004,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zpio.h"

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



int zpio_udp_create()
{
#if defined(WE_LOVE_BILL) && defined(ZSYS_NET_SUPPORT)
    if( !zpio_sockets_init() ) {
	errno = ENODEV;
	return -1;
    }
#endif
}

int zpio_udp_close(int sock) {
    return zpio_close_sock(sock);
}

int zpio_udp_create_serve(int port)
{
    int sock = zpio_udp_create();
    struct sockaddr_in sa;
    memset(&sa,0,sizeof(sa));
    /* address is set to 0.0.0.0 == INNADDR_ANY */
    sa.sin_port = htons((short)port);
    sa.sin_family = AF_INET;

    if( bind(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0 ) {
	zst_set_socket_error(NULL);
	zpio_close_sock(sock);
	return -1;
    }
    return sock;
}
int zpio_udp_create_connect(const char* name, int port)
{   
    int sock = zpio_udp_create();

    return sock;
}

int zpio_setsockopt_bool(int sock, int opt, int value)
{
    
}

int zpio_udp_sendto()
{
}
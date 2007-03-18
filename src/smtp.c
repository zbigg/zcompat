/*
* File: error.c
*
* Id:           $Id: smtp.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  error routines
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* std */
#include <stdlib.h>

#include "zcompat/zsystype.h"
#include <errno.h>
#include <string.h>
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

#if 0
/* Different input & output stream */
ZSTREAM	zmail_open	(ZSTREAM,const char*,int);
int	zmail_read	(ZSTREAM,void*,size_t);
int	zmail_write	(ZSTREAM,const void*,size_t);
int	zmail_close	(ZSTREAM);

zstream_vtable_t zvt_mail = {
    1,			/*__ id */
    1,			/*__ data_size */
    zmail_open,		/*__ open */
    zmail_close,	/*__ close */
    zmail_read,		/*__ read */
    zmail_write,	/*__ write */
    NULL,		/*__ seek */
    NULL,		/*__ ctl */
};

#define	mail_stream	(ZSTREAM)	f->data[0]
#define mail_error	(char*)		f->data[1]

ZSTREAM	zmail_open	(ZSTREAM f,const char* name,int mode)
{
    return f;
}
int	zmail_read	(ZSTREAM f,void* buf,size_t size)
{
    return zread(mail_stream,buf,size);
}
int	zmail_write	(ZSTREAM f,const void* buf,size_t size)
{
    return zwrite(mail_stream,buf,size);
}
int	zmail_close	(ZSTREAM f)
{
    zmailclose(mail_stream);
}

#endif

static  int hear(ZSTREAM a)
{
    int h;
    char buf[1024];
    buf[0] = 0;
    h = zfgets(a,buf,sizeof(buf));
    return h;
}

ZSTREAM	zmailopen(const char* server,const char* sender,const char* recipient)
{
    int a;
    const char* sendmailname = "sendmail -bs";
    const char* servername = server;
    ZSTREAM mf;

    if( sender == NULL || recipient == NULL ) {
	errno = EINVAL;
	return NULL;
    }
    if( server != NULL ) {
	if( !strncmp(server,"local:",6) ) {
	    sendmailname = &server[6];
	    server = NULL;
	} else if( !strncmp(server,"remote:",7) ) {
	    servername = &server[7];
	}
    }
    if ( server == NULL ) {
	mf = zpopen(sendmailname,ZO_RDWR);
	if( mf == NULL )
	    return NULL;
    } else {
	mf = zsopen(servername,25);
	if( mf == NULL )
	    return NULL;
    }

    hear(mf);	/* SMTP returns: 220: introduction text */
    a = strcspn(sender,"@\0");
    zfputs(mf,"helo ");
    if( sender[a] != 0 ) {
	zwrite(mf,&sender[a+1],strlen(&sender[a+1]));
    }
    zfputs(mf,"\r\n");
    hear(mf); /* SMTP returns: 250 hello text */

    zfprintf(mf,"mail from: %s\r\n",sender);
    hear(mf); /* SMTP returns: 250 sender ok */

    {
	char *x,*y,*z;
	y = x = z = zpio_strdup(recipient);

	while ( (x = strchr(z,',')) != 0 ) {
	    *x++ = 0;
	    zfprintf(mf,"rcpt to: %s\r\n",z);
	    hear(mf); /* SMTP returns: 250 recipient ok */
	    z = x;
	}
	if( *z )  {
	    zfprintf(mf,"rcpt to: %s\r\n",z);
	    hear(mf); /* SMTP returns: 250 recipient ok */
	}
	zpio_free(y);
    }

    zfputs(mf,"data\r\n");
    hear(mf);	/* SMTP returns: 354 enter mail, end with "." on a line by itself*/

    return 0;
}

int	zmailclose(ZSTREAM f)
{
    int a;
    if( f == NULL )
	return -1;
    zfputs(f,
	"\r\n.\r\n"
	"quit\r\n"
	);
    hear(f);
    a = zclose(f);
    return a;
}

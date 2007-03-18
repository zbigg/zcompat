/*
* File:		chconv.c
*
* Id:           $Id: chconv.c,v 1.6 2003/06/28 11:50:19 trurl Exp $
*
* Project:	ZBIG Portable Input/Output library.
*
* Description:  Char Converter Filter
*
* Author:	Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2003,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zcompat.h"
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

#include <stdlib.h>
#include <string.h>

static int zcharconv_read(ZSTREAM f, void *buf, size_t size);
static int zcharconv_write(ZSTREAM f, const void *buf, size_t size);

#define input_table ( (unsigned char*) f->data[0] )
#define output_table ( (unsigned char*) f->data[1] )

ZSTREAM zcharconv(ZSTREAM parent, zcharconv_table input, zcharconv_table output)
{
    ZSTREAM f = zfilter_creat(parent, zcharconv_read, zcharconv_write, 2);
    if (f == NULL)
	return NULL;
    f->data[0] = (long)input;
    f->data[1] = (long)output;
    return f;
}

static int zcharconv_read(ZSTREAM f, void *buf, size_t size)
{
    int e = zread(f->parent, buf, size);
    if (e <= 0)
	return e;
    else if (input_table) {
	register int m = e;
	register unsigned char *i = (unsigned char *) buf;
	while (m-- > 0) {
	    *i = input_table[*i];
	    i++;
	}
    }
    return e;
}
static int zcharconv_write(ZSTREAM f, const void *buf, size_t size)
{
    if (output_table) {
	size_t n = 0;
	while (n < size) {
	    char szbuf[200];
	    int now = size - n < sizeof(szbuf) ? size - n : sizeof(szbuf);
	    memcpy(szbuf, (char *) buf + n, now);
	    {
		register int m = now;
		register unsigned char *i = (unsigned char *) szbuf;
		while (m-- > 0) {
		    *i = output_table[*i];
		    i++;
		}
	    }
	    {
		int r = zwrite(f->parent, szbuf, now);
		if (r <= 0)
		    return n;
		n += now;
	    }
	}
	return n;
    } else
	return zwrite(f->parent, buf, size);
}

void zcharconv_table_init(zcharconv_table t)
{
    unsigned int i;
    for (i = 0; i < 256; i++)
	t[i] = (unsigned char)i;
}

int zcharconv_get(zcharconv_table t, int src_code, int dest_code)
{
    zcharconv_table_init(t);
    switch (dest_code) {
    case ZCHC_PLAIN:
	switch (src_code) {
	case ZCHC_WIN_1250:	/* win  --> plain */
	    t[185] = 'a';
	    t[230] = 'c';
	    t[234] = 'e';
	    t[179] = 'l';
	    t[241] = 'n';
	    t[243] = 'o';
	    t[156] = 's';
	    t[159] = 'z';
	    t[191] = 'z';

	    t[165] = 'A';
	    t[198] = 'C';
	    t[202] = 'E';
	    t[163] = 'L';
	    t[209] = 'N';
	    t[211] = 'O';
	    t[140] = 'S';
	    t[143] = 'Z';
	    t[175] = 'Z';
	    return 1;
	case ZCHC_ISO_8859_2:	/* iso  --> plain */
	    t[177] = 'a';
	    t[230] = 'c';
	    t[234] = 'e';
	    t[179] = 'l';
	    t[241] = 'n';
	    t[243] = 'o';
	    t[182] = 's';
	    t[188] = 'z';
	    t[191] = 'z';

	    t[161] = 'A';
	    t[198] = 'C';
	    t[202] = 'E';
	    t[163] = 'L';
	    t[209] = 'N';
	    t[211] = 'O';
	    t[166] = 'S';
	    t[172] = 'Z';
	    t[175] = 'Z';
	    return 1;
	}
	break;
    case ZCHC_ISO_8859_2:
	switch (src_code) {
	case ZCHC_DOS_852:
	    t[165] = 177;
	    t[134] = 230;
	    t[169] = 234;
	    t[136] = 179;
	    t[228] = 241;
	    t[162] = 243;
	    t[152] = 182;
	    t[190] = 191;
	    t[171] = 188;

	    t[164] = 161;
	    t[143] = 198;
	    t[168] = 202;
	    t[157] = 163;
	    t[227] = 209;
	    t[224] = 211;
	    t[151] = 166;
	    t[189] = 175;
	    t[141] = 172;
	    return 1;
	case ZCHC_WIN_1250:	/* win  --> iso */
	    t[185] = 177;
	    t[156] = 182;
	    t[159] = 188;
	    t[165] = 161;
	    t[140] = 166;
	    t[143] = 172;
	    return 1;
	case ZCHC_PLAIN:	/* plain -> iso */
	    return 0;
	};
	break;
    case ZCHC_WIN_1250:
	switch (src_code) {
	case ZCHC_PLAIN:	/* plain --> win */
	    return 0;
	case ZCHC_ISO_8859_2:	/* iso  --> win */
	    t[177] = 185;
	    t[182] = 156;
	    t[188] = 159;
	    t[161] = 165;
	    t[166] = 140;
	    t[172] = 143;
	    return 1;
	};
	break;
    case ZCHC_DOS_852:
	switch(src_code) {
	case ZCHC_PLAIN:	/* plain --> dos */
	    return 0;
	case ZCHC_ISO_8859_2:	/* iso  --> dos */
	    t[177] = 165;
	    t[230] = 134;
	    t[234] = 169;
	    t[179] = 136;
	    t[241] = 228;
	    t[243] = 162;
	    t[182] = 152;
	    t[191] = 190;
	    t[188] = 171;

	    t[161] = 164;
	    t[198] = 143;
	    t[202] = 168;
	    t[163] = 157;
	    t[209] = 227;
	    t[211] = 224;
	    t[166] = 151;
	    t[175] = 189;
	    t[172] = 141;
	    return 1;
	case ZCHC_WIN_1250:	/* win  --> dos */
	    t[185] = 165;
	    t[230] = 134;
	    t[234] = 169;
	    t[179] = 136;
	    t[241] = 228;
	    t[243] = 162;
	    t[156] = 152;
	    t[159] = 190;
	    t[191] = 171;

	    t[165] = 164;
	    t[198] = 143;
	    t[202] = 168;
	    t[163] = 157;
	    t[209] = 227;
	    t[211] = 224;
	    t[140] = 151;
	    t[143] = 189;
	    t[175] = 141;
	    return 1;
	}
    }
    return -1;
}

/* TODO 
    * move it to zpio
    * remove chconv.h header, because project must have one header
*/


#include "convtabl.c"

/*static int utf8_getc(const char *s);*/
static int utf8_getx(char **s);
static int utf8_setc(char *s, int c);
/*static int utf8_width(const char *s);*/
static int utf8_cwidth(int c);
/*static int utf8_isok(int c);*/

/*
static struct {
    const char* encoding; // encoding identifier
    unsigned short *table;         // 8bit to unicode table
} encodings_list[] = {
    ...
    NULL,NULL
}

*/
static int conv_bytes(unsigned char* table, const char* src, char* dest, size_t length);
static int conv_bytes2utf8(zwchar* table, const char* src, char* dest, size_t length);
static int conv_bytes2unicode(zwchar* table, const char* src, zwchar* dest, size_t length);
static int conv_unicode2bytes(zcconv_unicode* table, size_t table_size, const zwchar* src, char* dest, size_t length);
static int conv_unicode2utf8(const zwchar* src, char* dest, size_t length);
static int conv_utf82bytes(zcconv_unicode* table, size_t table_size, const char* src, char* dest, size_t length);
static int conv_utf82unicode(const char* src, zwchar* dest, size_t length);

static int get_encoding_type(const char* name, zwchar** byte2unicode_ptr);

static unsigned char unicode_search(zcconv_unicode* table, size_t table_size,zwchar x);
static unsigned char unicode2byte(zcconv_unicode* table, size_t table_size,zwchar x);

static int create_unicode2byte_table(zwchar* byte2unicode, zcconv_unicode** res, size_t size);
static int create_byte2byte_table(zwchar* src_table,zwchar* dest_table, zchar** res, size_t size);

zcconv_t*   zcconv_new(const char* from, const char* to)
{
    zcconv_t* cc = zpio_calloc(1, sizeof( zcconv_t));
    zwchar* src_conv,*dest_conv;
    /*	search for encoding types and conversin tables */
    cc->type_src = get_encoding_type(from,&src_conv);
    cc->type_dest = get_encoding_type(to,&dest_conv);
    
    cc->enc_src = (char*)zpio_strdup(from);
    cc->enc_dest = (char*)zpio_strdup(to);
    
    if( cc->type_src == cc->type_dest ) {	/* both are the same ! */
	if( cc->type_src == ZCC_ANSI ) {
	    create_byte2byte_table(src_conv,dest_conv,&cc->conv_byte,128);
	} else {
	    /* do nothing, because zcconv knows what to do if ucs/utf types are equal */
	}
    } else
    if( cc->type_src == ZCC_ANSI ) {
	/* cc->type_dest must be UTF or UNICODE, both need conv_byte2unicode table */
	cc->conv_byte2unicode = src_conv;
    } else 
    if( cc->type_dest == ZCC_ANSI ) {
	/* cc->type_src must be UTF or UNICODE, both need conv_unicode table and its size*/
	create_unicode2byte_table(dest_conv, &cc->conv_unicode,128);
	cc->conv_unicode_size = 128;
    } else 
    if( cc->type_src != -1 && cc->type_dest != -1 ) {
	/* unknown types, so do nothing ! */
	/* anything we know, they don't need conversion tables */
    } else {
	/* one of types is unknown, so fail */
	zpio_free(cc);
	return NULL;
    }

    /* merge tables if both are bytes */
    /* is src = unicode || utf then */
        /* create reverse table sorted by unicode values */
    return cc;
}
void        zcconv_free(zcconv_t* cc)
{
    /*if( cc->conv_byte2unicode ) 
	zpio_free(cc->conv_byte2unicode);*/
    if( cc->conv_byte )
        zpio_free(cc->conv_byte);
    if( cc->conv_unicode ) {
	zpio_free(cc->conv_unicode);
    }
    zpio_free(cc->enc_src);
    zpio_free(cc->enc_dest);
    zpio_free(cc);
}

/*
    gets size in bytes of requested buffer    
*/
size_t      zcconv_dest_size(zcconv_t* cc, const void* src, size_t src_len)
{
    switch( cc->type_dest ) {
    case ZCC_ANSI:
        return src_len+1;
    case ZCC_UTF8:
	switch( cc->type_src ) {
	case ZCC_ANSI:
	    {
		const unsigned char* x = (const unsigned char*)src;
		size_t i = 0,l = 0;
		while( *x && i < src_len ) {
		    if( *x < 128 )
			l++;
		    else
			l += 8;
		    x++;
		    i++;
		}
		return l+1;
	    }
	case ZCC_UCS2:
	    {
		const zwchar* x = (const zwchar*)src;
		size_t i = 0,l = 0;
		while( *x && i < src_len ) {
		    l += utf8_cwidth(*x++);
		    i++;
		}
		return l+1;
	    }
	    break;
	case ZCC_UTF8:
	    return src_len+1;
	    break;
	default:
	    return 0;
	}
    case ZCC_UCS2:
        return (src_len+1)*sizeof(zwchar);
    default: 
        return 0;
    }
}

size_t      zcconv_dest_length(zcconv_t* cc, size_t src_len)
{
    return src_len;
}

/* 
    do a conversion
    Treat src as type of source in zcconv_t and so dest.
*/

int         zcconv(zcconv_t* cc, const void* src, void* dest,size_t length)
{
    switch( cc->type_src ) {
    case ZCC_ANSI:
        switch( cc->type_dest ) {
        case ZCC_ANSI:
            return conv_bytes(cc->conv_byte,(const char*)src, (char*)dest,length);
        case ZCC_UTF8:
	    return conv_bytes2utf8(cc->conv_byte2unicode,
                (const char*)src, (char*)dest,length);
        case ZCC_UCS2:
            return conv_bytes2unicode(cc->conv_byte2unicode,
                (const char*)src, (zwchar*)dest,length);
        default: return -1;
        }
    case ZCC_UCS2:
        switch( cc->type_dest ) {
        case ZCC_ANSI:
            return conv_unicode2bytes(cc->conv_unicode,cc->conv_unicode_size,
                        (const zwchar*)src, (char*)dest, length);
        case ZCC_UTF8:
            return conv_unicode2utf8((const zwchar*)src, (char*)dest, length);
        case ZCC_UCS2:
            memcpy(dest,src, length*sizeof(zwchar));
            return length;
        default: return -1;
        }
    case ZCC_UTF8:
        switch( cc->type_dest ) {
        case ZCC_ANSI:
            return conv_utf82bytes(cc->conv_unicode,cc->conv_unicode_size,
                        (const char*)src, (char*)dest, length);
        case ZCC_UTF8:
            memcpy(dest,src, length*sizeof(unsigned char));
            return length;
        case ZCC_UCS2:
            return conv_utf82unicode((const char*)src, (zwchar*)dest, length);
        default: return -1;
        }
    }
    return -1;
}

static int create_byte2byte_table(zwchar* src_table,zwchar* dest_table, zchar** res, size_t size)
{
    size_t i;
    zwchar *is;
    zchar x;
    zchar* d;
    zcconv_unicode* u2d_table;
    size_t	    u2d_size;
    if( !res || size == 0 || !src_table || !dest_table ) return 0;
    *res = d = zpio_calloc(size,sizeof(zchar));
    u2d_size = create_unicode2byte_table(dest_table,&u2d_table,size);
    for( is = src_table, i = 0; i < size; i++, is++,d++ ) {
	x = unicode2byte(u2d_table,u2d_size,*is);
	if( x == 0 )
	    *d = i+128; /* TODO: or '?' */
	else
	    *d = x;
    }
    return size;
}

static int zcconv_unicode_cmp(const void* p1,const void* p2) {
    zcconv_unicode* i1 = (zcconv_unicode*)p1;
    zcconv_unicode* i2 = (zcconv_unicode*)p2;
    return i1->u - i2->u;
}
static int create_unicode2byte_table(zwchar* b2u, zcconv_unicode** res, size_t size)
{
    zcconv_unicode* r,*ri;
    size_t i;
    if( !res ) return -1;
    if( size == 0 ) return 0;
    r = ri = zpio_calloc(size,sizeof(zcconv_unicode));
    for( i = 0; i < size; i++,ri++ ) {
	ri->b = i + 128;
	ri->u = b2u[i];
    }
    qsort(r,size,sizeof(zcconv_unicode), zcconv_unicode_cmp);
    *res = r;
    return size;
}


static 	    int		get_encoding_type(const char* name, zwchar** pct) {
    zwchar* ct = NULL;
    int type = -1;
    if( zc_strcasecmp(name,"unicode") == 0 ||
	zc_strcasecmp(name,"ucs2") == 0 )
	type = ZCC_UCS2;
    else if( 
	zc_strcasecmp(name,"utf-8") == 0 ||
	zc_strcasecmp(name,"utf8") == 0 ) 
	type = ZCC_UTF8;
    else if( 
	zc_strcasecmp(name,"ansi") == 0 )
	type = ZCC_ANSI;
    else {
	int i;
	for( i = 0; i < countof(encodings_list); i++ ) {
	    if( !encodings_list[i].encoding ) break;
	    if( zc_strcasecmp(encodings_list[i].encoding,name) == 0 ) {
		ct = encodings_list[i].table;
		type = ZCC_ANSI;
	    }
	}
    }
    if( pct )
	*pct = ct;
    return type;
}

static unsigned char unicode_search(zcconv_unicode* table, size_t table_size,zwchar x) 
{
    if( x < 128 )
	return (unsigned char)x;
    else {
#if 0
	register size_t m,h;
        unsigned char y;
	register zwchar xf;
#endif	
#if 0
        m = table_size/2;               /* do a binary search of character */
        h = m > 0 && m < table_size;
        xf = table[m].u;
        y = table[m].b;
        while( xf != x && h ) {
            if( xf > x )
                m = m / 2;
            else
                m = (m+table_size)/2;
            xf = table[m].u;
            y = table[m].b;
            h = m > 0 && m < table_size;
        }
#endif
#if 0
        h = table_size/2;
        m = h;               /* do a binary search of character */
        xf = table[m].u;
        y = table[m].b;
        while( xf != x && h > 0) {
            h = h/2;
            if( xf > x )
                m -= h ;
            else
                m += h;
            xf = table[m].u;
            y = table[m].b;
        }
        if( xf != x )
            return 0;
        else
            return y;
#endif
#if 1
	register size_t m;
	for( m =0; m < table_size; m++ )
	    if( table[m].u == x )
		return table[m].b;
	return '?';
#endif
    }
}
static unsigned char  unicode2byte(zcconv_unicode* table, size_t table_size,zwchar x)
{
    unsigned char r;
    if( !table || ((r = unicode_search(table,table_size,x)) == 0) )
	if( (r = unicode_search(encoding_unicode_fallback,countof(encoding_unicode_fallback),x)) == 0 )
	    return 0;
    return r;
}

/*
    this table must be 128 chars length - 128 unsigned chars
*/
static int conv_bytes(unsigned char* table, const char* src, char* dest, size_t length)
{
    size_t i = 0;
    unsigned char x;
    while( i < length ) {
        x = *src++;
        *dest++ = x > 127 ? table[ x-128 ] : x;
        i++;
    }
    return i;
}

/*
    this table must be 256 bytes length - 128 unsigned shorts
*/
static int conv_bytes2utf8(zwchar* table, const char* src, char* dest, size_t length)
{
    size_t i = 0;
    int l;
    unsigned char x;
    unsigned short ux;
    while( *src && i < length ) {
	x = *src++;
	if( x < 128 ) {
	    i++;
	    *dest++ = x;
	} else {
	    ux = table[ x-128 ];
	    l = utf8_setc(dest,ux);
	    i+=l;
	    dest+=l;
	}
    }
    *dest++ = '\0';
    return i;
}
static int conv_bytes2unicode(zwchar* table, const char* src, zwchar* dest, size_t length)
{
    size_t i = 0;
    unsigned char x;
    while( *src && i < length-1 ) {
        x = *src++;
        *dest++ = x > 127 ? table[ x-128 ] : x;
        i++;
    }
    *dest++ = 0;
    return i;
}

/*
    this table is variable in length it should be sorted by unicode 
    value, lowest unicde at start
*/
static int conv_unicode2bytes(zcconv_unicode* table, size_t table_size, const zwchar* src, char* dest, size_t length)
{
    size_t i = 0;
    register size_t m;
    /*register size_t h;*/
    unsigned char y,lb;
    zwchar lx=0xffff;
    register zwchar x;
    /*register zwchar xf;*/
    while( i < length ) {
        /* calc x */
        x = *src++;
        if( x == lx ) /* maybe it's last of the unicode found char ? */
            y = lb;
        else if( x > 127 ) {
#if 1
	    y = '?';
	    for( m =0; m < table_size; m++ )
		if( table[m].u == x ) {
		    y = table[m].b; 
		    lx = x; 
		    lb = y; break; 
		}
#endif
#if 0
            h = table_size/2;
            m = h;               /* do a binary search of character */
            xf = table[m].u;
            y = table[m].b;
            while( xf != x && h > 0) {
                h = h/2;
                if( xf > x )
                    m -= h ;
                else
                    m += h;
                xf = table[m].u;
                y = table[m].b;
            }
            if( xf != x )
                y = '?';
            else {
                lx = x;
                lb = y;
            }
#endif
        } else
            y = (unsigned char)x;
        *dest++ = y;
        i++;
    }
    return i;
}
static int conv_unicode2utf8(const zwchar* src, char* dest, size_t length)
{
    size_t i = 0;
    int l;
    unsigned short x;
    while( i < length ) {
	x = *src++;
	if( x < 128 ) {
	    i++;
	    *dest++ = (unsigned char)x;
	} else {
	    l = utf8_setc(dest,x);
	    i+=l;
	    dest+=l;
	}
    }
    *dest++ = '\0';
    return i;
}

static int conv_utf82bytes(zcconv_unicode* table, size_t table_size, const char* src, char* dest, size_t length)
{
    size_t i = 0;
    unsigned short x;
    while( *src && i < length-1 ) {
	x = utf8_getx((char**)&src);
	*dest++ = unicode2byte(table, table_size, x);
	i++;
    }
    *dest++ = '\0';
    return i;
}

static int conv_utf82unicode(const char* src, zwchar* dest, size_t length)
{
    size_t i = 0;
    while( *src && i < length ) {
	*dest++ = utf8_getx((char**)&src);
	i++;
    }
    *dest++ = 0;
    return i;
}
/*
static int utf8_getc(const char *s)
{
   int c = *((unsigned char *)(s++));
   int n, t;

   if (c & 0x80) {
      n = 1;
      while (c & (0x80>>n))
	 n++;

      c &= (1<<(8-n))-1;

      while (--n > 0) {
	 t = *((unsigned char *)(s++));

	 if ((!(t&0x80)) || (t&0x40))
	    return '^';

	 c = (c<<6) | (t&0x3F);
      }
   }

   return c;
}
*/
static int utf8_getx(char **s)
{
   int c = *((unsigned char *)((*s)++));
   int n, t;

   if (c & 0x80) {
      n = 1;
      while (c & (0x80>>n))
	 n++;

      c &= (1<<(8-n))-1;

      while (--n > 0) {
	 t = *((unsigned char *)((*s)++));

	 if ((!(t&0x80)) || (t&0x40)) {
	    (*s)--;
	    return '^';
	 }

	 c = (c<<6) | (t&0x3F);
      }
   }

   return c;
}

static int utf8_setc(char *s, int c)
{
   int size, bits, b, i;

   if (c < 128) {
      *s = c;
      return 1;
   }

   bits = 7;
   while (c >= (1<<bits))
      bits++;

   size = 2;
   b = 11;

   while (b < bits) {
      size++;
      b += 5;
   }

   b -= (7-size);
   s[0] = c>>b;

   for (i=0; i<size; i++)
      s[0] |= (0x80>>i);

   for (i=1; i<size; i++) {
      b -= 6;
      s[i] = 0x80 | ((c>>b)&0x3F);
   }

   return size;
}
/*
static int utf8_width(const char *s)
{
   int c = *((unsigned char *)s);
   int n = 1;

   if (c & 0x80) {
      while (c & (0x80>>n))
	 n++;
   }

   return n;
}
*/


static int utf8_cwidth(int c)
{
   int size, bits, b;

   if (c < 128)
      return 1;

   bits = 7;
   while (c >= (1<<bits))
      bits++;

   size = 2;
   b = 11;

   while (b < bits) {
      size++;
      b += 5;
   }

   return size;
}
/*
static int utf8_isok(int c)
{
   return 1;
}
*/

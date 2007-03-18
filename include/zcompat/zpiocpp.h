/*
* File:		zpiocpp.h
*
* Project:	ZCompat Portable Input/Output library C++ Interface.
*
* Decription:	ZPIO Library C++ Interface header.
*
* Author:	Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
* To do:
    * add ztruncate(ZSTREAM f, ?pos or current?)
*/


#ifndef __zpiocpp_h_
#define __zpiocpp_h_
#define INC_ZPIOCPP

#ifndef INC_ZPIO
#include "zcompat/zpio.h"
#endif
#include <ctype.h>
#include <math.h>
#ifdef __cplusplus

class zstream;

class zstream {
private:
    ZSTREAM s;
public:
    enum {
	open_detect = 0,
	open_file,
	open_pipe,
	open_socket
    };
    enum {
	mode_read   = ZO_READ,
	mode_write  = ZO_WRITE,
	mode_append = ZO_APPEND,
	mode_excl   = ZO_EXCL,
	mode_creat  = ZO_CREAT,
	mode_trunc  = ZO_TRUNC,
	mode_nodelay= ZO_NODELAY,
	mode_binary = ZO_BINARY,
	mode_text   = ZO_TEXT,
	mode_new    = ZO_NEW,
	mode_rdwr   = ZO_RDWR
    };
    enum {
	seek_set = ZSEEK_SET,
	seek_cur = ZSEEK_CUR,
	seek_end = ZSEEK_END
    };
    /*
	Constructors.
    */
    inline zstream(const char* name, int mode, int as = open_detect);
    inline zstream(ZSTREAM stream);
    inline zstream(ZSTREAM in,ZSTREAM out);
    inline zstream(int fd, const char* name = 0);
    inline zstream(FILE* f, const char* name = 0);
    inline zstream();
#ifdef ZSYS_WIN
    inline zstream(ZWIN_HANDLE h);
#endif
    inline ~zstream();

    /*
	Basic operations
    */
    inline int close();
    inline int closesub(int mode);
    inline void detach();


    inline int read(void* buffer,int read_size);
    inline int write(const void* buffer,int write_size);
    inline int seek(zoff_t offset, int whence = seek_set);

    inline int send(ZSTREAM dest,int max = -1,int flags = 0);
    inline int eof() { return zeof(s); }
    /* Opening */
    inline int open(const char* name, int mode, int as = open_detect);
    inline int open(ZSTREAM stream);
    inline int open(ZSTREAM in,ZSTREAM out);
    inline int open(int fd, const char* name = 0);
    inline int open(FILE* f, const char* name = 0);
#ifdef ZSYS_WIN
    inline int open(ZWIN_HANDLE h);
#endif

    inline int fopen(const char* name,int mode);
    inline int sopen(const char* host,short port);
    inline int popen(const char* command,int mode);
    inline int httpopen(const char* url,int options,int* result);
#ifdef ZSYS_WIN
    inline int win32hopen(ZWIN_HANDLE h);
#endif

    /*
	String operations
    */
	/*
	    Watch out <stdio.h> frequently defines
	    getc & putc as macros
	*/
    inline int		(getc)() { return zfgetc(s); }
    inline void		(putc)(int c) { zfputc(s,c); }
    inline void		ungetc(int c) { zungetc(s,c); }

    /*
	Output operators
    */
    inline zstream& operator << (zstream& src);
    inline zstream& operator << (char i);
    inline zstream& operator << (signed char i);
    inline zstream& operator << (unsigned char i);
    inline zstream& operator << (short i);
    inline zstream& operator << (unsigned short i);
    inline zstream& operator << (int i);
    inline zstream& operator << (unsigned int i);
    inline zstream& operator << (long i);
    inline zstream& operator << (unsigned long i);
    inline zstream& operator << (float i);
    inline zstream& operator << (double i);
    inline zstream& operator << (long double i);
    inline zstream& operator << (const char* i);
    inline zstream& operator << (zstream& (*manip)(zstream& s));

    /*
	Input operators
    */
    inline zstream& operator >> (zstream& dest);
    inline zstream& operator >> (char& i);
    inline zstream& operator >> (signed char& i);
    inline zstream& operator >> (unsigned char& i);
    inline zstream& operator >> (short& i);
    inline zstream& operator >> (unsigned short& i);
    inline zstream& operator >> (int& i);
    inline zstream& operator >> (unsigned int& i);
    inline zstream& operator >> (long& i);
    inline zstream& operator >> (unsigned long& i);
    inline zstream& operator >> (float& i);
    inline zstream& operator >> (double& i);
    inline zstream& operator >> (long double& i);

    inline 	operator ZSTREAM() { return s; }

    /*
	Misc
    */
    inline zstream& eatwhite();
private:
    int _close();
    int _os;
};

inline zstream::zstream(const char* name, int mode, int as) : s(0),_os(0)
{
    open(name,mode,as);
}
inline zstream::zstream(ZSTREAM stream) : s(0),_os(0)
{
    open(stream);
}
inline zstream::zstream(ZSTREAM in,ZSTREAM out) : s(0),_os(0)
{
    open(in,out);
}
inline zstream::zstream(int fd, const char* name) : s(0),_os(0)
{
    open(fd,name);
}
inline zstream::zstream(FILE* f, const char* name) : s(0),_os(0)
{
    open(f,name);
}
inline zstream::zstream() : s(0),_os(0)
{
}
#ifdef ZSYS_WIN
inline zstream::zstream(ZWIN_HANDLE h) : s(0),_os(0)
{
    open(h);
}
#endif
inline zstream::~zstream()
{
    _close();
}

/* Basic operations */
inline int zstream::close()
{
    return _close();
}
inline int zstream::_close()
{
    if( s && _os ) return zclose(s);
    s = 0;
    return -1;
}
inline void zstream::detach()
{
    s = 0;
}
inline int zstream::closesub(int mode)
{
    if( s ) return zclosesub(s,mode);
    return -1;
}

inline int zstream::read(void* buffer,int read_size)
{
    return zread(s,buffer,read_size);
}
inline int zstream::write(const void* buffer,int write_size)
{
    return zwrite(s,buffer,write_size);
}
inline int zstream::seek(zoff_t offset, int whence)
{
    return zseek(s,offset,whence);
}

inline int zstream::send(ZSTREAM dest,int max,int flags)
{
    return zpio_send(dest,s,max,flags);
}

/* Opening */
inline int zstream::open(const char* name, int mode, int as)
{
    _close();
    switch( as ) {
    case open_file:
	return fopen(name,mode);
    case open_pipe:
	return popen(name,mode);
    case open_socket:
	return sopen(name,mode);
    default:
	s = zopen(name,mode);
	break;
    }
    _os = 1;
    return s ? 0 : -1;
}
inline int zstream::open(ZSTREAM _s)
{
    _close();
    s = _s;
    _os = 0;
    return s ? 0 : -1;
}
inline int zstream::open(ZSTREAM in,ZSTREAM out)
{
    _close();
    s = zdscreat(in,out);
    _os = 1;
    return s ? 0 : -1;
}
inline int zstream::open(int fd, const char* name)
{
    _close();
    s = zdopen(fd,name);
    _os = 1;
    return s ? 0 : -1;
}
inline int zstream::open(FILE* f, const char* name )
{
    _close();
    s = zansiopen(f,name);
    _os = 1;
    return s ? 0 : -1;
}
#ifdef ZSYS_WIN
inline int zstream::open(ZWIN_HANDLE h)
{
    _close();
    s = zwin32hopen(h);
    _os = 1;
    return s ? 0 : -1;
}
#endif
inline int zstream::fopen(const char* name,int mode)
{
    _close();
    s = zfopen(name,mode);
    _os = 1;
    return s ? 0 : -1;
}
inline int zstream::sopen(const char* host,short port)
{
    _close();
    s = zsopen(host,port);
    _os = 1;
    return s ? 0 : -1;
}
inline int zstream::popen(const char* command,int mode)
{
    _close();
    s = zpopen(command,mode);
    _os = 1;
    return s ? 0 : -1;
}

inline int zstream::httpopen(const char* url,int options,int* result)
{
    _close();
    s = zhttpopen(url,options,result);
    _os = 1;
    return s ? 0 : -1;
}
#ifdef ZSYS_WIN
inline int zstream::win32hopen(ZWIN_HANDLE h)
{
    _close();
    s = zwin32hopen(h);
    _os = 1;
    return s ? 0 : -1;
}
#endif

/* output operators */
inline zstream& zstream::operator << (zstream& src)
{
    src.send(s,-1,0);
    return *this;
}

inline zstream& zstream::operator << (char i)
{
    zfprintf(s,"%c",i);
    return *this;
}

inline zstream& zstream::operator << (signed char i)
{
    zfprintf(s,"%c",i);
    return *this;
}

inline zstream& zstream::operator << (unsigned char i)
{
    zfprintf(s,"%u",i);
    return *this;
}

inline zstream& zstream::operator << (short i)
{
    zfprintf(s,"%h",i);
    return *this;
}

inline zstream& zstream::operator << (unsigned short i)
{
    zfprintf(s,"%uh",i);
    return *this;
}

inline zstream& zstream::operator << (int i)
{
    zfprintf(s,"%i",i);
    return *this;
}

inline zstream& zstream::operator << (unsigned int i)
{
    zfprintf(s,"%u",i);
    return *this;
}

inline zstream& zstream::operator << (long i)
{
    zfprintf(s,"%li",i);
    return *this;
}

inline zstream& zstream::operator << (unsigned long i)
{
    zfprintf(s,"%lu",i);
    return *this;
}

inline zstream& zstream::operator << (float i)
{
    zfprintf(s,"%f",i);
    return *this;
}

inline zstream& zstream::operator << (double i)
{
    zfprintf(s,"%f",i);
    return *this;
}

inline zstream& zstream::operator << (long double i)
{
    zfprintf(s,"%Lf",i);
    return *this;
}

inline zstream& zstream::operator << (const char* i)
{
    zfprintf(s,"%s",i);
    return *this;
}

inline zstream& zstream::operator << (zstream& (*manip)(zstream& s))
{
    return manip(*this);
}


inline zstream& zstream::operator >> (zstream& dest)
{
    send(dest.s,-1,0);
    return *this;
}

inline int _NUM(char a)
{
    return a - '0';
}

template <class X>
    void zpio_read_integer(ZSTREAM s,X& r)
    {
	int c;
	bool sign = true;
	r = 0;
	c = zfgetc(s);
	if( c == '-' ) {
	    sign = false;
	    c = zfgetc(s);
	}
	if( !isdigit(c) ) return;
	do {
	    r *= 10;
	    r += _NUM(c);
	} while( isdigit(c = zfgetc(s)) );
	if( c > 0 ) zungetc(s,c);
	if( !sign) r = -r;
    }

template <class X>
    void zpio_read_integer_u(ZSTREAM s,X& r)
    {
	int c;
	r = 0;
	c = zfgetc(s);
	if( c == '-' )
	    c = zfgetc(s);
	if( !isdigit(c) ) return;
	do {
	    r *= 10;
	    r += _NUM(c);
	} while( isdigit(c = zfgetc(s)) );
	if( c > 0 ) zungetc(s,c);
    }

template <class X>
    void zpio_read_float(ZSTREAM s,X& r)
    {
	int c;
	int e = 0;
	int dc,	// digit count
	    dp; // dot position
	bool sign = true;
	r = 0;
	c = zfgetc(s);
	if( c == '-' ) {
	    sign = false;
	    c = zfgetc(s);
	}
	if( c != '.' && !isdigit(c) ) return;
	dp = -1;
	dc = 0;
	do {
	    if( c == '.' ) {
		dp = dc;
	    } else {
		r *= 10.0;
		r += _NUM(c);
		++dc;
	    }
	} while( isdigit(c = zfgetc(s)) || c == '.' );
	if( c == 'e' || c == 'E' ) {
	    zpio_read_integer<int>(s,e);
	} else
	    if( c > 0 ) zungetc(s,c);
	c = 0;
	if( dp >= 0 )
	    c += dp - dc;
	if( e )
	    c += e;
	if( c )
	    r *= (X)pow(10.0, c);
	if( !sign) r = -r;
    }

inline zstream& zstream::operator >> (char& i)
{
    eatwhite();
    zpio_read_integer<char>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (signed char& i)
{
    eatwhite();
    zpio_read_integer<signed char>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (unsigned char& i)
{
    eatwhite();
    zpio_read_integer_u<unsigned char>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (short& i)
{
    eatwhite();
    zpio_read_integer<short>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (unsigned short& i)
{
    eatwhite();
    zpio_read_integer_u<unsigned short>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (int& i)
{
    eatwhite();
    zpio_read_integer<int>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (unsigned int& i)
{
    eatwhite();
    zpio_read_integer_u<unsigned int>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (long& i)
{
    eatwhite();
    zpio_read_integer<long>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (unsigned long& i)
{
    eatwhite();
    zpio_read_integer_u<unsigned long>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (float& i)
{
    eatwhite();
    zpio_read_float<float>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (double& i)
{
    eatwhite();
    zpio_read_float<double>(s,i);
    return *this;
}

inline zstream& zstream::operator >> (long double& i)
{
    eatwhite();
    zpio_read_float<long double>(s,i);
    return *this;
}

inline zstream& zstream::eatwhite()
{
    int c = zfgetc(s);
    while( isspace(c) ) c = zfgetc(s);
    zungetc(s,c);
    return *this;
}

inline zstream& endl(zstream& s)
{
#if defined(ZSYS_WIN) || defined( ZSYS_MSDOS)
    return s << "\r\n";
#else
    return s << "\n";
#endif
}

#endif /* __cplusplus */
#endif /* __zpiocpp_h_ */

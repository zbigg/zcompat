/* 
* File:		vform.c
*
* Id:           $Id: vform.c,v 1.4 2003/06/27 19:04:34 trurl Exp $

* printf-like text formatting functions.

  `Formatter' is taken from Apache Server source modified for
  general use.

* Functions:
    Formatter
	*	zvformatter

    Limited buffer length versions of sprintf and vsprintf
	*	zsnprintf
	*	zvsnprintf

    Allocating formatted output
	*	zsaprintf


* Modification for general use
   Zbigniew Zagorski <zzbigg@o2.pl>

* Copyright:

*   This program is Copyright(c) Zbigniew Zagorski 1999-2000,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.

       **************************************************
  I've took those formatting functions from Apache Server sources.
       **************************************************

  ====================================================================
 * Copyright (c) 1995-1999 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 * This code is based on, and used with the permission of, the
 * SIO stdio-replacement strx_* functions by Panos Tsirigotis
 * <panos@alumni.cs.colorado.edu> for xinetd.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdio.h>

#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#ifndef WITHOUT_ZPIO
#include "zcompat/zsystype.h"
#include "zcompat/zpio.h"
#include "zpio_i.h"

#define malloc	zpio_malloc
#define free	zpio_free
#define realloc zpio_realloc
#endif

#include "zcompat/zprintf.h"

typedef enum {
    NO = 0, YES = 1
} boolean_e;

#ifndef FALSE
#define FALSE			0
#endif
#ifndef TRUE
#define TRUE			1
#endif

#ifndef ZLONG_LONG
#if defined(ZSYS_MSDOS) && !defined(ZDEV_DJGPP)
#define ZLONG_LONG long
#elif defined(ZDEV_MSC) || defined(ZDEV_BORLAND)
#define ZLONG_LONG __int64
#else
#define ZLONG_LONG long long
#endif
#endif

#define NUL			'\0'
#define INT_NULL		((int *)0)
#define WIDE_INT		long
#define WIDEST_INT		ZLONG_LONG

typedef WIDE_INT wide_int;
typedef unsigned WIDE_INT u_wide_int;
typedef WIDEST_INT widest_int;
#ifdef __TANDEM
/* Although Tandem supports "long long" there is no unsigned variant. */
typedef unsigned long	    u_widest_int;
#else
typedef unsigned WIDEST_INT u_widest_int;
#endif
typedef int bool_int;

#define S_NULL			"(null)"
#define S_NULL_LEN		6

#define FLOAT_DIGITS		6
#define EXPONENT_LENGTH 	10

/*
 * NUM_BUF_SIZE is the size of the buffer used for arithmetic conversions
 *
 * XXX: this is a magic number; do not decrease it
 */
#define NUM_BUF_SIZE		512

/*
 * cvt.c - IEEE floating point formatting routines for FreeBSD
 * from GNU libc-4.6.27.  Modified to be thread safe.
 */

/*
 *    zpio_ecvt converts to decimal
 *	the number of digits is specified by ndigit
 *	decpt is set to the position of the decimal point
 *	sign is set to 0 for positive, 1 for negative
 */

#define NDIG	80

/* buf must have at least NDIG bytes */
static char *cvt(double arg, int ndigits, int *decpt, int *sign, int eflag, char *buf)
{
    register int r2;
    double fi, fj;
    register char *p, *p1;

    if (ndigits >= NDIG - 1)
	ndigits = NDIG - 2;
    r2 = 0;
    *sign = 0;
    p = &buf[0];
    if (arg < 0) {
	*sign = 1;
	arg = -arg;
    }
    arg = modf(arg, &fi);
    p1 = &buf[NDIG];
    /*
     * Do integer part
     */
    if (fi != 0) {
	p1 = &buf[NDIG];
	while (fi != 0) {
	    fj = modf(fi / 10, &fi);
	    *--p1 = (int) ((fj + .03) * 10) + '0';
	    r2++;
	}
	while (p1 < &buf[NDIG])
	    *p++ = *p1++;
    }
    else if (arg > 0) {
	while ((fj = arg * 10) < 1) {
	    arg = fj;
	    r2--;
	}
    }
    p1 = &buf[ndigits];
    if (eflag == 0)
	p1 += r2;
    *decpt = r2;
    if (p1 < &buf[0]) {
	buf[0] = '\0';
	return (buf);
    }
    while (p <= p1 && p < &buf[NDIG]) {
	arg *= 10;
	arg = modf(arg, &fj);
	*p++ = (int) fj + '0';
    }
    if (p1 >= &buf[NDIG]) {
	buf[NDIG - 1] = '\0';
	return (buf);
    }
    p = p1;
    *p1 += 5;
    while (*p1 > '9') {
	*p1 = '0';
	if (p1 > buf)
	    ++ * --p1;
	else {
	    *p1 = '1';
	    (*decpt)++;
	    if (eflag == 0) {
		if (p > buf)
		    *p = '0';
		p++;
	    }
	}
    }
    *p = '\0';
    return (buf);
}

static char *zpio_ecvt(double arg, int ndigits, int *decpt, int *sign, char *buf)
{
    return (cvt(arg, ndigits, decpt, sign, 1, buf));
}

static char *zpio_fcvt(double arg, int ndigits, int *decpt, int *sign, char *buf)
{
    return (cvt(arg, ndigits, decpt, sign, 0, buf));
}

/*
 * zpio_gcvt  - Floating output conversion to
 * minimal length string
 */

static char *zpio_gcvt(double number, int ndigit, char *buf, boolean_e altform)
{
    int sign, decpt;
    register char *p1, *p2;
    register int i;
    char buf1[NDIG];

    p1 = zpio_ecvt(number, ndigit, &decpt, &sign, buf1);
    p2 = buf;
    if (sign)
	*p2++ = '-';
    for (i = ndigit - 1; i > 0 && p1[i] == '0'; i--)
	ndigit--;
    if ((decpt >= 0 && decpt - ndigit > 4)
	|| (decpt < 0 && decpt < -3)) { 	/* use E-style */
	decpt--;
	*p2++ = *p1++;
	*p2++ = '.';
	for (i = 1; i < ndigit; i++)
	    *p2++ = *p1++;
	*p2++ = 'e';
	if (decpt < 0) {
	    decpt = -decpt;
	    *p2++ = '-';
	}
	else
	    *p2++ = '+';
	if (decpt / 100 > 0)
	    *p2++ = decpt / 100 + '0';
	if (decpt / 10 > 0)
	    *p2++ = (decpt % 100) / 10 + '0';
	*p2++ = decpt % 10 + '0';
    }
    else {
	if (decpt <= 0) {
	    if (*p1 != '0')
		*p2++ = '.';
	    while (decpt < 0) {
		decpt++;
		*p2++ = '0';
	    }
	}
	for (i = 1; i <= ndigit; i++) {
	    *p2++ = *p1++;
	    if (i == decpt)
		*p2++ = '.';
	}
	if (ndigit < decpt) {
	    while (ndigit++ < decpt)
		*p2++ = '0';
	    *p2++ = '.';
	}
    }
    if (p2[-1] == '.' && !altform)
	p2--;
    *p2 = '\0';
    return (buf);
}

/*
 * The INS_CHAR macro inserts a character in the buffer and writes
 * the buffer back to disk if necessary
 * It uses the char pointers sp and bep:
 *	sp points to the next available character in the buffer
 *	bep points to the end-of-buffer+1
 * While using this macro, note that the nextb pointer is NOT updated.
 *
 * NOTE: Evaluation of the c argument should not have any side-effects
 */
#define INS_CHAR(c, sp, bep, cc)				\
	    {							\
		if (sp >= bep) {				\
		    vbuff->curpos = sp; 			\
		    vbuff->buflen = sp - vbuff->bufpos; 	\
		    if (flush_func(vbuff))			\
			return -1;				\
		    sp = vbuff->curpos; 			\
		    bep = vbuff->endpos;			\
		    vbuff->bufpos = sp; 			\
		}						\
		*sp++ = (c);					\
		cc++;						\
	    }

#define NUM( c )			( c - '0' )

#define STR_TO_DEC( str, num )		\
    num = NUM( *str++ ) ;		\
    while ( isdigit( *str ) )		\
    {					\
	num *= 10 ;			\
	num += NUM( *str++ ) ;		\
    }

/*
 * This macro does zero padding so that the precision
 * requirement is satisfied. The padding is done by
 * adding '0's to the left of the string that is going
 * to be printed.
 */
#define FIX_PRECISION( adjust, precision, s, s_len )	\
    if ( adjust )					\
	while ( s_len < precision )			\
	{						\
	    *--s = '0' ;				\
	    s_len++ ;					\
	}

/*
 * Macro that does padding. The padding is done by printing
 * the character ch.
 */
#define PAD( width, len, ch )	do		\
	{					\
	    INS_CHAR( ch, sp, bep, cc ) ;	\
	    width-- ;				\
	}					\
	while ( width > len )

/*
 * Prefix the character ch to the string str
 * Increase length
 * Set the has_prefix flag
 */
#define PREFIX( str, length, ch )	 *--str = ch ; length++ ; has_prefix = YES


/*
 * Convert num to its decimal format.
 * Return value:
 *   - a pointer to a string containing the number (no sign)
 *   - len contains the length of the string
 *   - is_negative is set to TRUE or FALSE depending on the sign
 *     of the number (always set to FALSE if is_unsigned is TRUE)
 *
 * The caller provides a buffer for the string: that is the buf_end argument
 * which is a pointer to the END of the buffer + 1 (i.e. if the buffer
 * is declared as buf[ 100 ], buf_end should be &buf[ 100 ])
 *
 * Note: we have 2 versions. One is used when we need to use quads
 * (conv_10_quad), the other when we don't (conv_10). We're assuming the
 * latter is faster.
 */
static char *conv_10(register wide_int num, register bool_int is_unsigned,
		     register bool_int *is_negative, char *buf_end,
		     register int *len)
{
    register char *p = buf_end;
    register u_wide_int magnitude;

    if (is_unsigned) {
	magnitude = (u_wide_int) num;
	*is_negative = FALSE;
    }
    else {
	*is_negative = (num < 0);

	/*
	 * On a 2's complement machine, negating the most negative integer
	 * results in a number that cannot be represented as a signed integer.
	 * Here is what we do to obtain the number's magnitude:
	 *	a. add 1 to the number
	 *	b. negate it (becomes positive)
	 *	c. convert it to unsigned
	 *	d. add 1
	 */
	if (*is_negative) {
	    wide_int t = num + 1;

	    magnitude = ((u_wide_int) -t) + 1;
	}
	else
	    magnitude = (u_wide_int) num;
    }

    /*
     * We use a do-while loop so that we write at least 1 digit
     */
    do {
	register u_wide_int new_magnitude = magnitude / 10;

	*--p = (char) (magnitude - new_magnitude * 10 + '0');
	magnitude = new_magnitude;
    }
    while (magnitude);

    *len = buf_end - p;
    return (p);
}

static char *conv_10_quad(register widest_int num, register bool_int is_unsigned,
		     register bool_int *is_negative, char *buf_end,
		     register int *len)
{
    register char *p = buf_end;
    register u_widest_int magnitude;

    /*
     * If the value is less than the maximum unsigned long value,
     * then we know we aren't using quads, so use the faster function
     */
    if ( is_unsigned
	    ? ((u_widest_int)num <= (u_widest_int)ULONG_MAX )
	    :  num >= 0 && num <= (widest_int)ULONG_MAX )
	return(conv_10( (wide_int)num, is_unsigned, is_negative,
	       buf_end, len));

    if (is_unsigned) {
	magnitude = (u_widest_int) num;
	*is_negative = FALSE;
    }
    else {
	*is_negative = (num < 0);

	/*
	 * On a 2's complement machine, negating the most negative integer
	 * results in a number that cannot be represented as a signed integer.
	 * Here is what we do to obtain the number's magnitude:
	 *	a. add 1 to the number
	 *	b. negate it (becomes positive)
	 *	c. convert it to unsigned
	 *	d. add 1
	 */
	if (*is_negative) {
	    widest_int t = num + 1;

	    magnitude = ((u_widest_int) -t) + 1;
	}
	else
	    magnitude = (u_widest_int) num;
    }

    /*
     * We use a do-while loop so that we write at least 1 digit
     */
    do {
	register u_widest_int new_magnitude = magnitude / 10;

	*--p = (char) (magnitude - new_magnitude * 10 + '0');
	magnitude = new_magnitude;
    }
    while (magnitude);

    *len = buf_end - p;
    return (p);
}

/*
 * Convert a floating point number to a string formats 'f', 'e' or 'E'.
 * The result is placed in buf, and len denotes the length of the string
 * The sign is returned in the is_negative argument (and is not placed
 * in buf).
 */
static char *conv_fp(register char format, register double num,
    boolean_e add_dp, int precision, bool_int *is_negative,
    char *buf, int *len)
{
    register char *s = buf;
    register char *p;
    int decimal_point;
    char buf1[NDIG];

    if (format == 'f')
	p = zpio_fcvt(num, precision, &decimal_point, is_negative, buf1);
    else			/* either e or E format */
	p = zpio_ecvt(num, precision + 1, &decimal_point, is_negative, buf1);

    /*
     * Check for Infinity and NaN
     */
    if (isalpha(*p)) {
	*len = strlen(strcpy(buf, p));
	*is_negative = FALSE;
	return (buf);
    }

    if (format == 'f') {
	if (decimal_point <= 0) {
	    *s++ = '0';
	    if (precision > 0) {
		*s++ = '.';
		while (decimal_point++ < 0)
		    *s++ = '0';
	    }
	    else if (add_dp)
		*s++ = '.';
	}
	else {
	    while (decimal_point-- > 0)
		*s++ = *p++;
	    if (precision > 0 || add_dp)
		*s++ = '.';
	}
    }
    else {
	*s++ = *p++;
	if (precision > 0 || add_dp)
	    *s++ = '.';
    }

    /*
     * copy the rest of p, the NUL is NOT copied
     */
    while (*p)
	*s++ = *p++;

    if (format != 'f') {
	char temp[EXPONENT_LENGTH];	/* for exponent conversion */
	int t_len;
	bool_int exponent_is_negative;

	*s++ = format;		/* either e or E */
	decimal_point--;
	if (decimal_point != 0) {
	    p = conv_10((wide_int) decimal_point, FALSE, &exponent_is_negative,
			&temp[EXPONENT_LENGTH], &t_len);
	    *s++ = exponent_is_negative ? '-' : '+';

	    /*
	     * Make sure the exponent has at least 2 digits
	     */
	    if (t_len == 1)
		*s++ = '0';
	    while (t_len--)
		*s++ = *p++;
	}
	else {
	    *s++ = '+';
	    *s++ = '0';
	    *s++ = '0';
	}
    }

    *len = s - buf;
    return (buf);
}


/*
 * Convert num to a base X number where X is a power of 2. nbits determines X.
 * For example, if nbits is 3, we do base 8 conversion
 * Return value:
 *	a pointer to a string containing the number
 *
 * The caller provides a buffer for the string: that is the buf_end argument
 * which is a pointer to the END of the buffer + 1 (i.e. if the buffer
 * is declared as buf[ 100 ], buf_end should be &buf[ 100 ])
 *
 * As with conv_10, we have a faster version which is used when
 * the number isn't quad size.
 */
static char *conv_p2(register u_wide_int num, register int nbits,
		     char format, char *buf_end, register int *len)
{
    register int mask = (1 << nbits) - 1;
    register char *p = buf_end;
    static const char low_digits[] = "0123456789abcdef";
    static const char upper_digits[] = "0123456789ABCDEF";
    register const char *digits = (format == 'X') ? upper_digits : low_digits;

    do {
	*--p = digits[num & mask];
	num >>= nbits;
    }
    while (num);

    *len = buf_end - p;
    return (p);
}

static char *conv_p2_quad(register u_widest_int num, register int nbits,
		     char format, char *buf_end, register int *len)
{
    register int mask = (1 << nbits) - 1;
    register char *p = buf_end;
    static const char low_digits[] = "0123456789abcdef";
    static const char upper_digits[] = "0123456789ABCDEF";
    register const char *digits = (format == 'X') ? upper_digits : low_digits;

    if (num <= ULONG_MAX)
	return(conv_p2( (u_wide_int)num, nbits, format, buf_end, len));

    do {
	*--p = digits[num & mask];
	num >>= nbits;
    }
    while (num);

    *len = buf_end - p;
    return (p);
}


/*
 * Do format conversion placing the output in buffer
 */
int zvformatter(int (*flush_func)(zvformatter_buff *),
    zvformatter_buff *vbuff, const char *fmt, va_list ap)
{
    register char *sp;
    register char *bep;
    register int cc = 0;
    register int i;

    register char *s = NULL;
    char *q;
    int s_len;

    register int min_width = 0;
    int precision = 0;
    enum {
	LEFT, RIGHT
    } adjust;
    char pad_char;
    char prefix_char;

    double fp_num;
    long double fp_num_ld;
    widest_int i_quad = (widest_int) 0;
    u_widest_int ui_quad;
    wide_int i_num = (wide_int) 0;
    u_wide_int ui_num;
    int base;

    char num_buf[NUM_BUF_SIZE];
    char char_buf[2];		/* for printing %% and %<unknown> */

    enum var_type_enum {
	IS_QUAD,    /* int64: long long : ll | I64 | q	*/
	IS_LONG,    /* int32: long	: l		*/
	IS_INT,     /* int				*/
	IS_SHORT,   /* int16: short	: h		*/
	IS_CHAR,    /* int8 : char	: hh		*/
	IS_INTMAX,  /*	    : intmax_t	: j		*/
	IS_SIZE,    /*	    : size_t	: z		*/
	IS_PTRDIFF, /*	    : ptrdiff_t : t		*/
	IS_LONG_DBL /*	    : long double : L		*/
    };
    enum var_type_enum var_type = IS_INT;

    /*
     * Flag variables
     */
    boolean_e format_xml;
    boolean_e format_tabs;
    boolean_e alternate_form;
    boolean_e print_sign;
    boolean_e print_blank;
    boolean_e adjust_precision;
    boolean_e adjust_width;
    bool_int is_negative;

    sp = vbuff->curpos;
    bep = vbuff->endpos;

    while (*fmt) {
	if (*fmt != '%') {
	    INS_CHAR(*fmt, sp, bep, cc);
	}
	else {
	    /*
	     * Default variable settings
	     */
	    adjust = RIGHT;
	    alternate_form = print_sign = print_blank = NO;
	    pad_char = ' ';
	    prefix_char = NUL;
	    format_xml = format_tabs = NO;
	    fmt++;

	    /*
	     * Try to avoid checking for flags, width or precision
	     */
	    if( *fmt == 'T' ) {
		*fmt++;
		while( *fmt && isalpha(*fmt) && isupper(*fmt) ) {
		    switch(*fmt) {
			case 'T' : format_tabs = YES; break;
			case 'X' : format_xml = YES; break;
			default: break;
		    }
		    fmt++;
		}
	    }
	    if (!islower(*fmt)) {
		/*
		 * Recognize flags: -, #, BLANK, +
		 */
		for (;; fmt++) {
		    if (*fmt == '-')
			adjust = LEFT;
		    else if (*fmt == '+')
			print_sign = YES;
		    else if (*fmt == '#')
			alternate_form = YES;
		    else if (*fmt == ' ')
			print_blank = YES;
		    else if (*fmt == '0')
			pad_char = '0';
		    else
			break;
		}

		/*
		 * Check if a width was specified
		 */
		if (isdigit(*fmt)) {
		    STR_TO_DEC(fmt, min_width);
		    adjust_width = YES;
		}
		else if (*fmt == '*') {
		    min_width = va_arg(ap, int);
		    fmt++;
		    adjust_width = YES;
		    if (min_width < 0) {
			adjust = LEFT;
			min_width = -min_width;
		    }
		}
		else
		    adjust_width = NO;

		/*
		 * Check if a precision was specified
		 *
		 * XXX: an unreasonable amount of precision may be specified
		 * resulting in overflow of num_buf. Currently we
		 * ignore this possibility.
		 */
		if (*fmt == '.') {
		    adjust_precision = YES;
		    fmt++;
		    if (isdigit(*fmt)) {
			STR_TO_DEC(fmt, precision);
		    }
		    else if (*fmt == '*') {
			precision = va_arg(ap, int);
			fmt++;
			if (precision < 0)
			    precision = 0;
		    }
		    else
			precision = 0;
		}
		else
		    adjust_precision = NO;
	    }
	    else
		adjust_precision = adjust_width = NO;

	    /*
	     * Modifier check
	     */
	    if (*fmt == 'q') {
		var_type = IS_QUAD;
		fmt++;
	    }
	    else if (*fmt == 'l') {
		if( fmt[1] == 'l' ) {
		    var_type = IS_QUAD;
		    fmt+=2;
		} else {
		    var_type = IS_LONG;
		    fmt++;
		}
	    }
	    else if (*fmt == 'h') {
		if( fmt[1] == 'h' ) {
		    var_type = IS_CHAR;
		    fmt+=2;
		} else {
		    var_type = IS_SHORT;
		    fmt++;
		}
	    }
	    else if (*fmt == 'I' && fmt[1] == '6' && fmt[2] == '4') {
		var_type = IS_QUAD;
		fmt+=3;
	    } else if( *fmt == 'j' ) {
		var_type = IS_INTMAX;
		fmt++;
	    } else if( *fmt == 'z' ) {
		var_type = IS_SIZE;
		fmt++;
	    } else if( *fmt == 't' ) {
		var_type = IS_PTRDIFF;
		fmt++;
	    } else if( *fmt == 'L' ) {
		var_type = IS_LONG_DBL;
		fmt++;
	    }
	    else {
		var_type = IS_INT;
	    }

	    /*
	     * Argument extraction and printing.
	     * First we determine the argument type.
	     * Then, we convert the argument to a string.
	     * On exit from the switch, s points to the string that
	     * must be printed, s_len has the length of the string
	     * The precision requirements, if any, are reflected in s_len.
	     *
	     * NOTE: pad_char may be set to '0' because of the 0 flag.
	     *	 It is reset to ' ' by non-numeric formats
	     */
	    base = 0;
	    switch (*fmt) {
#if 0
	    case 'd':
	    case 'i':
		switch( var_type ) {
		case IS_QUAD:
		    i_quad = va_arg(ap, widest_int);
		    goto FMT_LONGLONG;
		case IS_LONG:
		    i_num = (wide_int) va_arg(ap, wide_int);
		    goto FMT_INT;
		case IS_SHORT:
		    i_num = (wide_int) (short) va_arg(ap, short);
		    goto FMT_INT;
		case IS_CHAR:
		    i_num = (wide_int) (char) va_arg(ap,char);
		    goto FMT_INT;
		case IS_SIZE:
		    i_num = (wide_int) (size_t) va_arg(ap,size_t);
		    goto FMT_INT;
		case IS_PTRDIFF:
		    i_num = (wide_int) (ptrdiff_t) va_arg(ap,ptrdiff_t);
		    goto FMT_INT;
		case IS_INTMAX:
		    i_num = (wide_int) (intmax_t) va_arg(ap,intmax_t);
		    goto FMT_LONGLONG;
		case IS_INT:
		default:
		    i_num = (wide_int) (int) va_arg(ap,int);
		    goto FMT_INT;
		}
		FMT_LONGLONG:
		s = conv_10_quad(i_quad, 0, &is_negative,
			&num_buf[NUM_BUF_SIZE], &s_len);
		FIX_PRECISION(adjust_precision, precision, s, s_len);
		if (is_negative)
		    prefix_char = '-';
		else if (print_sign)
		    prefix_char = '+';
		else if (print_blank)
		    prefix_char = ' ';
		break;

		FMT_INT:
		s = conv_10(i_num, 0, &is_negative,
			&num_buf[NUM_BUF_SIZE], &s_len);
		FIX_PRECISION(adjust_precision, precision, s, s_len);

		if (is_negative)
		    prefix_char = '-';
		else if (print_sign)
		    prefix_char = '+';
		else if (print_blank)
		    prefix_char = ' ';
		break;
	    case 'u':
		base = 10;
	    case 'o':
		base = base ? base : 8;
	    case 'x':
	    case 'X':
		base = base ? base : 16;
		switch( var_type ) {
		case IS_QUAD:
		    ui_quad = va_arg(ap, u_widest_int);
		    goto FMT_ULONGLONG;
		case IS_INTMAX:
		    ui_quad = (u_wide_int) (uintmax_t) va_arg(ap,uintmax_t);
		    goto FMT_ULONGLONG;
		case IS_LONG:
		    ui_num = (u_wide_int) va_arg(ap, u_wide_int);
		    goto FMT_UINT;
		case IS_SHORT:
		    ui_num = (u_wide_int) (unsigned short) va_arg(ap, unsigned short);
		    goto FMT_UINT;
		case IS_CHAR:
		    ui_num = (u_wide_int) (unsigned char) va_arg(ap,unsigned char);
		    goto FMT_UINT;
		case IS_SIZE:
		    ui_num = (u_wide_int) (size_t) va_arg(ap,size_t);
		    goto FMT_UINT;
		case IS_PTRDIFF:
		    ui_num = (u_wide_int) (ptrdiff_t) va_arg(ap,ptrdiff_t);
		    goto FMT_UINT;
		    goto FMT_UINT;
		case IS_INT:
		default:
		    ui_num = (u_wide_int) (unsigned int) va_arg(ap,unsigned int);
		    goto FMT_UINT;
		}
		FMT_ULONGLONG:

		if( base == 10 ) {
		    s = conv_10_quad(ui_quad, 1, &is_negative,
			&num_buf[NUM_BUF_SIZE], &s_len);
		    FIX_PRECISION(adjust_precision, precision, s, s_len);

		} else if( base == 16 ) {
		    s = conv_p2_quad(ui_quad, 4, *fmt,
			&num_buf[NUM_BUF_SIZE], &s_len);
		    FIX_PRECISION(adjust_precision, precision, s, s_len);
		    if (alternate_form && ui_num != 0) {
			*--s = *fmt;	/* 'x' or 'X' */
			*--s = '0';
			s_len += 2;
		    }

		} else if( base == 8 ) {
		    s = conv_p2_quad(ui_quad, 3, *fmt,
			&num_buf[NUM_BUF_SIZE], &s_len);
		    FIX_PRECISION(adjust_precision, precision, s, s_len);
		    if (alternate_form && *s != '0') {
			*--s = '0';
			s_len++;
		    }
		}
		break;

		FMT_UINT:
		if( base == 10 ) {
		    s = conv_10(ui_num, 1, &is_negative,
			&num_buf[NUM_BUF_SIZE], &s_len);
		    FIX_PRECISION(adjust_precision, precision, s, s_len);
		} else if( base == 16 ) {
		    s = conv_p2(ui_num, 4, *fmt,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		    FIX_PRECISION(adjust_precision, precision, s, s_len);
		    if (alternate_form && ui_num != 0) {
			*--s = *fmt;	/* 'x' or 'X' */
			*--s = '0';
			s_len += 2;
		    }

		} else if( base == 8 ) {
		    s = conv_p2(ui_num, 3, *fmt,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		    FIX_PRECISION(adjust_precision, precision, s, s_len);
		    if (alternate_form && *s != '0') {
			*--s = '0';
			s_len++;
		    }
		}
		break;
#else
	    case 'u':
		if (var_type == IS_QUAD) {
		    i_quad = va_arg(ap, u_widest_int);
		    s = conv_10_quad(i_quad, 1, &is_negative,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		}
		else {
		    if (var_type == IS_LONG)
			i_num = (wide_int) va_arg(ap, u_wide_int);
		    else if (var_type == IS_SHORT)
			i_num = (wide_int) (unsigned short) va_arg(ap, unsigned int);
		    else if (var_type == IS_CHAR )
			i_num = (wide_int) (unsigned char) va_arg(ap,unsigned int);
		    else
			i_num = (wide_int) va_arg(ap, unsigned int);
		    s = conv_10(i_num, 1, &is_negative,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		}
		FIX_PRECISION(adjust_precision, precision, s, s_len);
		break;

	    case 'd':
	    case 'i':
		if (var_type == IS_QUAD) {
		    i_quad = va_arg(ap, widest_int);
		    s = conv_10_quad(i_quad, 0, &is_negative,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		}
		else {
		    if (var_type == IS_LONG)
			i_num = (wide_int) va_arg(ap, wide_int);
		    else if (var_type == IS_SHORT)
			i_num = (wide_int) (short) va_arg(ap, int );
		    else if (var_type == IS_CHAR )
			i_num = (wide_int) (signed char) va_arg(ap,int );
		    else
			i_num = (wide_int) va_arg(ap, int);
		    s = conv_10(i_num, 0, &is_negative,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		}
		FIX_PRECISION(adjust_precision, precision, s, s_len);

		if (is_negative)
		    prefix_char = '-';
		else if (print_sign)
		    prefix_char = '+';
		else if (print_blank)
		    prefix_char = ' ';
		break;


	    case 'o':
		if (var_type == IS_QUAD) {
		    ui_quad = va_arg(ap, u_widest_int);
		    s = conv_p2_quad(ui_quad, 3, *fmt,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		}
		else {
		    if (var_type == IS_LONG)
			ui_num = (u_wide_int) va_arg(ap, u_wide_int);
		    else if (var_type == IS_SHORT)
			ui_num = (u_wide_int) (unsigned short) va_arg(ap, unsigned int);
		    else if (var_type == IS_CHAR )
			i_num = (wide_int) (unsigned char) va_arg(ap,unsigned int);
		    else
			ui_num = (u_wide_int) va_arg(ap, unsigned int);
		    s = conv_p2(ui_num, 3, *fmt,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		}
		FIX_PRECISION(adjust_precision, precision, s, s_len);
		if (alternate_form && *s != '0') {
		    *--s = '0';
		    s_len++;
		}
		break;


	    case 'x':
	    case 'X':
		if (var_type == IS_QUAD) {
		    ui_quad = va_arg(ap, u_widest_int);
		    s = conv_p2_quad(ui_quad, 4, *fmt,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		}
		else {
		    if (var_type == IS_LONG)
			ui_num = (u_wide_int) va_arg(ap, u_wide_int);
		    else if (var_type == IS_SHORT)
			ui_num = (u_wide_int) (unsigned short) va_arg(ap, unsigned int);
		    else if (var_type == IS_CHAR )
			i_num = (wide_int) (unsigned char) va_arg(ap,unsigned int);
		    else
			ui_num = (u_wide_int) va_arg(ap, unsigned int);
		    s = conv_p2(ui_num, 4, *fmt,
			    &num_buf[NUM_BUF_SIZE], &s_len);
		}
		FIX_PRECISION(adjust_precision, precision, s, s_len);
		if (alternate_form && ui_num != 0) {
		    *--s = *fmt;	/* 'x' or 'X' */
		    *--s = '0';
		    s_len += 2;
		}
		break;
#endif

	    case 's':
		s = va_arg(ap, char *);
		if (s != NULL) {
		    s_len = strlen(s);
		    if (adjust_precision && precision < s_len)
			s_len = precision;
		}
		else {
		    s = S_NULL;
		    s_len = S_NULL_LEN;
		}
		pad_char = ' ';
		break;


	    case 'f':
	    case 'e':
	    case 'E':
		if( var_type == IS_LONG_DBL ) {
		    fp_num_ld = va_arg(ap,long double);
		    fp_num = (double)fp_num_ld;
		} else
		    fp_num = va_arg(ap, double);
		/*
		 * * We use &num_buf[ 1 ], so that we have room for the sign
		 */
		s = conv_fp(*fmt, fp_num, alternate_form,
			(adjust_precision == NO) ? FLOAT_DIGITS : precision,
			    &is_negative, &num_buf[1], &s_len);
		if (is_negative)
		    prefix_char = '-';
		else if (print_sign)
		    prefix_char = '+';
		else if (print_blank)
		    prefix_char = ' ';
		break;


	    case 'g':
	    case 'G':
		if (adjust_precision == NO)
		    precision = FLOAT_DIGITS;
		else if (precision == 0)
		    precision = 1;
		/*
		 * * We use &num_buf[ 1 ], so that we have room for the sign
		 */
		s = zpio_gcvt(va_arg(ap, double), precision, &num_buf[1],
			    alternate_form);
		if (*s == '-')
		    prefix_char = *s++;
		else if (print_sign)
		    prefix_char = '+';
		else if (print_blank)
		    prefix_char = ' ';

		s_len = strlen(s);

		if (alternate_form && (q = strchr(s, '.')) == NULL) {
		    s[s_len++] = '.';
		    s[s_len] = '\0'; /* delimit for following strchr() */
		}
		if (*fmt == 'G' && (q = strchr(s, 'e')) != NULL)
		    *q = 'E';
		break;


	    case 'c':
		char_buf[0] = (char) (va_arg(ap, int));
		s = &char_buf[0];
		s_len = 1;
		pad_char = ' ';
		break;


	    case '%':
		char_buf[0] = '%';
		s = &char_buf[0];
		s_len = 1;
		pad_char = ' ';
		break;


	    case 'n':
		if (var_type == IS_QUAD)
		    *(va_arg(ap, widest_int *)) = cc;
		else if (var_type == IS_LONG)
		    *(va_arg(ap, long *)) = cc;
		else if (var_type == IS_SHORT)
		    *(va_arg(ap, short *)) = cc;
		else
		    *(va_arg(ap, int *)) = cc;
		break;

	    case NUL:
		/*
		 * The last character of the format string was %.
		 * We ignore it.
		 */
		continue;


		/*
		 * The default case is for unrecognized %'s.
		 * We print %<char> to help the user identify what
		 * option is not understood.
		 * This is also useful in case the user wants to pass
		 * the output of format_converter to another function
		 * that understands some other %<char> (like syslog).
		 * Note that we can't point s inside fmt because the
		 * unknown <char> could be preceded by width etc.
		 */
	    default:
		char_buf[0] = '%';
		char_buf[1] = *fmt;
		s = char_buf;
		s_len = 2;
		pad_char = ' ';
		break;
	    }

	    if (prefix_char != NUL && s != S_NULL && s != char_buf) {
		*--s = prefix_char;
		s_len++;
	    }

	    if (adjust_width && adjust == RIGHT && min_width > s_len) {
		if (pad_char == '0' && prefix_char != NUL) {
		    INS_CHAR(*s, sp, bep, cc);
		    s++;
		    s_len--;
		    min_width--;
		}
		PAD(min_width, s_len, pad_char);
	    }

	    /*
	     * Print the string s.
	     */
	    for (i = s_len; i != 0; i--) {
		register const char* trans_s=NULL;
		register int trans_l=0;
#define	TRANS(c,len,str) case c: trans_l = len;trans_s = str; break;
		if( format_xml )
		    switch( *s ) {
			TRANS('<',4,"&lt;");
			TRANS('>',4,"&gt;");
			TRANS('&',5,"&amp;");
		    }
		if( format_tabs )
		    switch( *s ) {
			TRANS('\t',8,"        ");
		    }
#undef TRANS
		if( trans_l == 0) {
		    INS_CHAR(*s, sp, bep, cc);
		} else
		    for( ; trans_l != 0; trans_l-- ) {
			INS_CHAR(*trans_s++, sp, bep, cc);
		    }
		s++;
	    }

	    if (adjust_width && adjust == LEFT && min_width > s_len)
		PAD(min_width, s_len, pad_char);
	}
	fmt++;
    }
    vbuff->curpos = sp;
    return cc;
}

static int zsnprintf_flush(zvformatter_buff *vbuff)
{
    /* if the buffer fills we have to abort immediately, there is no way
     * to "flush" an zsnprintf... there's nowhere to flush it to.
     */
    return -1;
}
int zsnprintf(char *buf, size_t len, const char *format,...)
{
    int cc;
    va_list ap;
    zvformatter_buff vbuff;

    if (len == 0)
	return 0;

    /* save one byte for nul terminator */
    vbuff.curpos = buf;
    vbuff.endpos = buf + len - 1;
    vbuff.bufpos = buf;
    vbuff.buflen = 0;
    va_start(ap, format);
    cc = zvformatter(zsnprintf_flush, &vbuff, format, ap);
    va_end(ap);
    *vbuff.curpos = '\0';
    return (cc == -1) ? len : cc;
}

int zvsnprintf(char *buf, size_t len, const char *format,
			     va_list ap)
{
    int cc;
    zvformatter_buff vbuff;

    if (len == 0)
	return 0;

    /* save one byte for nul terminator */
    vbuff.curpos = buf;
    vbuff.endpos = buf + len - 1;
    vbuff.bufpos = buf;
    vbuff.buflen = 0;
    cc = zvformatter(zsnprintf_flush, &vbuff, format, ap);
    *vbuff.curpos = '\0';
    return (cc == -1) ? len : cc;
}

static int zsaprintf_flush(zvformatter_buff *vbuff)
{
    /* _param_ is start of string, thing on which we call realloc,
    free etc */
    register int olen = vbuff->endpos - (char*)vbuff->param + 1;
    register char* tmp = zcompat_realloc(vbuff->param, olen * 2);
    if( tmp == NULL )
	return -1;
    vbuff->curpos = tmp + (vbuff->curpos - (char*)vbuff->param);
    vbuff->endpos = tmp + (olen*2) -1;
    vbuff->param = tmp;
    return 0;
}
char* zvsaprintf(char *buf, size_t len, const char* format,va_list ap);

char* zsaprintf(char *buf, size_t len, const char *format,...)
{
    char* r;
    va_list ap;
    va_start(ap, format);
    r = zvsaprintf(buf,len,format,ap);
    va_end(ap);
    return r;
}

char* zvsaprintf(char *buf, size_t len, const char* format,va_list ap)
{
    int cc;
    zvformatter_buff vbuff;

    if( buf == NULL || len == 0 ) {
	buf = zcompat_malloc(16);
	if( buf == NULL )
	    return NULL;
	len = 16;
    }
    vbuff.curpos = buf;
    vbuff.endpos = buf + len - 1;
    vbuff.param = buf;
    cc = zvformatter(zsaprintf_flush, &vbuff, format, ap);
    *(vbuff.curpos) = '\0';
    if( cc == -1 ) {
	zpio_free(vbuff.param);
	return NULL;
    } else
	return vbuff.param;
}

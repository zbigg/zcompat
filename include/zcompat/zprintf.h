/*
* Header:	zprintf.h
*
* Text formatting functions. 
  Taken from Apache Server source, 
  modified for general use.

* Functions:
    Limited buffer length versions of sprintf and vsprintf
	*	zsnprintf	- format to string
	*	zvsnprintf	- format to string (va_list version)
	*	zvformatter	- formatter function
	*	zsaprintf	- format to dynamically allocated string

* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2002, 
*   All rights reserved, and is distributed as free software under the 
*   license in the file "LICENSE", which is included in the distribution.
*/
#ifndef __zprintf_h_
#define __zprintf_h_

#define INC_ZPRINTF

#ifndef WITHOUT_ZSYSTYPE
#include "zcompat/zsystype.h"
#endif /* WITHOUT_ZSYSTYPE */

#include <stdarg.h>
#include <stddef.h>

#ifndef WE_HAVE_XNPRINTF
#define WE_HAVE_XNPRINTF 1
#endif

#define zpio_vsnprintf	zvsnprintf
#define zpio_snprintf	zsnprintf

#if !WE_HAVE_XNPRINTF
#define vsnprintf	zpio_vsnprintf
#define snprintf	zpio_snprintf
#endif	/* WE_HAVE_XNPRINTF */


typedef struct {
    /** position of next inserted char */
    char*	curpos;	
    /** first invalid position in buffer usally buffer_start+ buffer size */
    char*	endpos;
    /** position of buffer to flush */
    char*	bufpos; 
    /** length of buffer to flush */
    int		buflen; 
    /** user parameter for flush function */
    void*	param;  
} zvformatter_buff;

#ifdef __cplusplus
extern "C" {
#endif
    
/**
    #######################################################
    ##
    ## =Function zsnprintf
    ##
    ## =Synopsis
    ##
    ##	|#include "zcompat/zprintf.h"
    ##	|
    ##	|char* zsnprintf(
    ##  |	char *buffer,
    ##  |	size_t buffer_len,
    ##	|	const char *format,
    ##	|	...);
    ##
    ## =Description
    ##
    ##	Formatted output to buffer.
    ##
    ##	Format string using 'printf-like' syntax and output result to
    ##	place pointed by <buffer>. No more than <bufferl_len> chars
    ##	(including ending '\0' character) will be written.
    ##	
    ##    
    ##	Arguments:
    ##	    - <buffer>:	    desttination buffer pointer
    ##	    - <buffer_len>: length of buffer
    ##	    - <format>:	    format string
    ##	    - ...	    list of arguments for formatter
    ##
    ## =Returns
    ##
    ##	Number of characters printed or -1 on error.
    ##
    #######################################################
    ##
    ## =Function zvsnprintf
    ##
    ## =Synopsis
    ##
    ##	|#include "zcompat/zprintf.h"
    ##	|
    ##	|char* zvsnprintf(
    ##	|	char *buffer,
    ##  |	size_t buffer_len,
    ##	|	const char *format,
    ##	|	va_list ap);
    ##
    ## =Description
    ##
    ##	Formatted output to buffer. Using arguments
    ##	vector.
    ##
    ##	Format string using 'printf-like' syntax and output result to
    ##	place pointed by <buffer>. No more than <bufferl_len> chars
    ##	(including ending '\0' character) will be written.
    ##	
    ##    
    ##	Arguments:
    ##	    - <buffer>:	    buffer pointer
    ##	    - <buffer_len>: length of buffer
    ##	    - <format>:	    format string
    ##	    - <ap>	    pointer to vector of arguments
    ##
    ## =Returns
    ##
    ##	Number of characters printed or -1 on error.
    ##
    #######################################################
    ##
    ## =Function zsaprintf
    ##
    ## =Synopsis
    ##
    ##	|#include "zcompat/zprintf.h"
    ##	|
    ##	|char* zsaprintf(
    ##	|    char *buf,size_t len,
    ##	|    const char *format,
    ##	|    ...);
    ##
    ## =Description
    ##
    ##	Formatted output to allocated string string.
    ##
    ##	Format string using 'printf-like' syntax and return allocated
    ##	string pointer. <buf> points to buffer which is length <len>
    ##	and can be reallocated using realloc.
    ##    
    ##	If <buf> is NULL. New string is allocated.
    ##	Arguments:
    ##	    - <buf>:	buffer pointer
    ##	    - <len>:	length of buffer
    ##	    - <format>:	format string
    ##	    - ...:	list of arguments for formatter
    ##
    ## =Returns
    ##
    ##	Pointer to newly allocated buffer containing 
    ##	formatted output or NULL on error.
    ##
    #######################################################
    ##
    ## =Function zvsaprintf
    ##
    ## =Synopsis
    ##	|#include "zcompat/zprintf.h"
    ##	|
    ##	|char* zvsaprintf(
    ##	|    char *buf,size_t len,
    ##	|    const char *format,
    ##	|    va_list ap);
    ##
    ## =Description
    ##
    ##	Formatted output to allocated string. Using arguments
    ##	vector.
    ##
    ##	Format string using 'printf-like' syntax and return allocated
    ##	string pointer. <buf> points to buffer which is length <len>
    ##	and can be reallocated using realloc.
    ##    
    ##	If <buf> is NULL. New string is allocated.
    ##	Arguments:
    ##	    - buf	buffer pointer
    ##	    - len	length of buffer
    ##	    - format	format string
    ##	    - ap	pointer to vector of arguments
    ##
    ## =Returns
    ##
    ##	Pointer to newly allocated buffer containing 
    ##	formatted output or NULL on error.
    ##
    #######################################################
    ##
    ## =Function zvformatter
    ##
    ## =Synopsis
    ##	|#include "zcompat/zprintf.h"
    ##	|
    ##  |int	zvformatter(
    ##  |     int (*flush_func)(zvformatter_buff *),
    ##  |     zvformatter_buff *vbuff, 
    ##  |     const char*   fmt,
    ##  |     va_list ap);
    ##
    ## =Description
    ##
    ##  Formatter function.
    ##
    ##	<TODO>: Write documentation for this function.
    ##
    ##	Parameters
    ##
    ##	- <flush_func>:	flushing callback
    ##	- <vbuff>:	formatter buffer data
    ##	- <fmt>:	format string
    ##	- <ap>:		parameters pointer
    ##
    ##
    ## =Returns
    ##
    ## Number of characters printed or -1 on error.
    ##
    #######################################################
*/

ZCEXPORT  int    zsnprintf(
    char *buffer,
    size_t buffersize,
    const char* format, 
    ...);

ZCEXPORT  int    zvsnprintf
    (char *buffer,
    size_t buffersize,
    const char* format,
    va_list ap);

ZCEXPORT  char*    zsaprintf(
    char *buf,
    size_t len,
    const char *format,
    ...);
ZCEXPORT  char*    zvsaprintf(
    char *buf,
    size_t len,
    const char *format,
    va_list ap);

ZCEXPORT  int    zvformatter(
    int (*flush_func)(zvformatter_buff *),
    zvformatter_buff *vbuff, 
    const char *fmt,
    va_list ap);

#ifdef __cplusplus
}
#endif

#endif /*__zprintf_h_*/





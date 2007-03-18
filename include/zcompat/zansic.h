/*
* Header:	zansic.h
*
* Project:	common use header file
*
* Description:
*   Define macros that change color of text
*   using ANSI control characters.
*
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/
#ifndef __zansic_h_
#define __zansic_h_

#define INC_ZANSIC

#ifndef __zsystype_h_
#include "zcompat/zsystype.h"
#endif
#if defined ZSYS_UNIX || defined ZUSE_ANSI_COLORS
#define ZANSI_BLACK   0
#define ZANSI_RED     1
#define ZANSI_GREEN   2
#define ZANSI_YELLOW  3
#define ZANSI_BLUE    4
#define ZANSI_MAGENTA 5
#define ZANSI_MAGNETA 5
#define ZANSI_CYAN    6
#define ZANSI_WHITE   7
#define ZANSI_T_COLOR(f,b)	"\033[3" # f ";4" # b "m"
#define ZANSI_T_FG_LCOLOR(f)   	"\033[0;3" #f "m"
#define ZANSI_T_FG_HCOLOR(f)   	"\033[1;3" #f "m"
#define ZANSI_T_DEFAULT		"\033[0m"
#define ZANSI_T_GOTOXY(c,r)    	"\033[" #c ";" #r "H"
#define ZANSI_T_CLEAR_SCREEN 	"\033[2J"
#define ANSI_C_INVERT(a)	"\033[30;47m" a ZANSI_T_DEFAULT
#define ANSI_C_BLACK(a)		"\033[0;30m"  a ZANSI_T_DEFAULT
#define ANSI_C_RED(a) 		"\033[0;31m"  a ZANSI_T_DEFAULT
#define ANSI_C_GREEN(a) 	"\033[0;32m"  a ZANSI_T_DEFAULT
#define ANSI_C_YELLOW(a)	"\033[0;33m"  a ZANSI_T_DEFAULT
#define ANSI_C_BLUE(a)		"\033[0;34m"  a ZANSI_T_DEFAULT
#define ANSI_C_MAGNETA(a)	"\033[0;35m"  a ZANSI_T_DEFAULT
#define ANSI_C_CYAN(a)		"\033[0;36m"  a ZANSI_T_DEFAULT
#define ANSI_C_WHITE(a)		"\033[0;37m"  a ZANSI_T_DEFAULT
#define ANSI_C_LT_BLACK(a)	"\033[1;30m"  a "\033[0m"
#define ANSI_C_LT_RED(a) 	"\033[1;31m"  a "\033[0m"
#define ANSI_C_LT_GREEN(a) 	"\033[1;32m"  a "\033[0m"
#define ANSI_C_LT_YELLOW(a)	"\033[1;33m"  a "\033[0m"
#define ANSI_C_LT_BLUE(a)	"\033[1;34m"  a "\033[0m"
#define ANSI_C_LT_MAGNETA(a)	"\033[1;35m"  a "\033[0m"
#define ANSI_C_LT_CYAN(a)	"\033[1;36m"  a "\033[0m"
#define ANSI_C_LT_WHITE(a)	"\033[1;37m"  a "\033[0m"
#else
#define ANSI_C_INVERT(a)	a
#define ANSI_C_RED(a) 		a
#define ANSI_C_GREEN(a) 	a
#define ANSI_C_YELLOW(a)	a
#define ANSI_C_BLUE(a)		a
#define ANSI_C_MAGNETA(a)	a
#define ANSI_C_CYAN(a)		a
#endif
/* C Extern Identifiers. */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif
#endif /* force single include of file */

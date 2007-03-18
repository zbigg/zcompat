 /*
   * File:	zopt.c
   *
   * Id:	$Id: zopt.c,v 1.4 2003/02/26 18:28:43 trurl Exp $
   *
   * Project:	common use utilities
   *
   * For documentation see zopt.h
   *
   * Author Zbigniew Zagorski <zzbigg@o2.pl>
   *
   * Copyright:
   *
   *   This program is Copyright(c) Zbigniew Zagorski 2002-2003,
   *   All rights reserved, and is distributed as free software under the
   *   license in the file "LICENSE", which is included in the distribution.
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "zcompat/zopt.h"
/*
struct zopt_rec {
   char*   name,
   char*   shortcut,
   int	    flags,
   int	    type,
   void*   param,
   unsigned size,
   const char* info
};
*/
static struct zopt_rec* match_short_form(struct zopt_rec* rec,int no,const char* s,const char** param)
{
    int i,r,l;
    for( i = 0; i< no; i++ ) {
	r = 0;
	if( rec[i].shortcut ) {
	    l = strlen(rec[i].shortcut);
	    r = strncmp(rec[i].shortcut,s,l) == 0;
	    if( !r ) continue;
	    switch( s[l] ) {
	    case '\0':
		/* no extra text in parameter */
		*param = 0;
		break;
	    case '=':
		/* parameter after equal sign */
		*param = s+l+1;
		break;
	    default:
		if( rec[i].flags & ZOPT_ALLOW_POSTFIX )
		    *param = s+l;
		else {
		    /* some other char? so don't match it */
		    r = 0;
		}
	    }
	}
	if( r )
	    return rec+i;
    }
    return NULL;
}

static struct zopt_rec* match_long_form(struct zopt_rec* rec,int no,const char* s,const char** param)
{
    int i,r,l;
    *param = 0;
    for( i = 0; i< no; i++ ) {
	r = 0;
	if( rec[i].name ) {
	    *param = 0;
	    l = strlen(rec[i].name);
	    r = strncmp(rec[i].name,s,l) == 0;
	    if( !r ) continue;
	    switch( s[l] ) {
	    case '\0':
		/* no extra text in parameter */
		*param = 0;
		break;
	    case '=':
		/* parameter after equal sign */
		*param = s+l+1;
		break;
	    default:
		/* some other char? so don't match it */
		r = 0;
		break;
	    }
	}
	if( r )
	    return rec+i;
    }
    return NULL;
}

int string2bool(const char* s)
{
    if( strcmp(s,"true") == 0 )
	return 1;
    if( strcmp(s,"yes") == 0 )
	return 1;
    if( strcmp(s,"enable") == 0 )
	return 1;
    if( strcmp(s,"y") == 0 )
	return 1;
    if( atoi(s) != 0)
	return 1;
    return 0;
}

static void printspaces(FILE* out,int n) {
    for( ; n > 0 ; n-- ) 
	fputc(' ',out);
}
#define COL 30

int zc_genusage(struct zopt_rec* records,int no,FILE* output)
{
    int i;
    if( !output )
	output = stderr;
    for( i =0; i< no; i++ ) {
	zopt_rec* rec = &records[i];
	char* default_value = NULL;
	char* name = NULL;
	char buffer[100];
	int l;
	{ /* set default values */
	    switch( rec->type ) {
	    case ZOPT_BOOL:
		{
		    int* d = (int*) rec->param;
		    int dv = d ? *d : 1;
		    default_value = dv ? "yes" : "no";
		    name = "enable";
		}
		break;
	    case ZOPT_NEGBOOL:
		{
		    int* d = (int*) rec->param;
		    int dv = d ? *d : 0;
		    default_value = dv ? "yes" : "no";
		    name = "disable";
		}
		break;
	    case ZOPT_DOUBLE:
		{
		    double* d = (double*) rec->param;
		    double dv = d ? *d : (double)rec->size;
		    default_value = buffer;
		    sprintf(buffer,"%g",dv);
		    name = "set value (floating number)";		    
		}
	    case ZOPT_INT:
		{
		    int* d = (int*) rec->param;
		    int dv = d ? *d : rec->size;
		    default_value = buffer;
		    sprintf(buffer,"%i",dv);
		    name = "set value (integer number) ";
		}
		break;
	    case ZOPT_STRING:
		{
		    const char** d = (const char**) rec->param;
		    const char* dv = d ? *d : (const char*)rec->size;
		    default_value = (char*)dv;
		    name ="set value (string)";
		}
		break;
	    case ZOPT_CALLBACK:
		{
		    
		}
		break;
	    }
	}
	l = 0;
	if( rec->flags & ZOPT_NO_PARAM ) {
	    l = 6+strlen(rec->name);
	    fprintf(output,"    --%s", rec->name);
	    if( rec->shortcut) {
	        l += 3+strlen(rec->shortcut);
		fprintf(output," | -%s",rec->shortcut);
	    }
	} else {
	    l = 12+strlen(rec->name);
	    fprintf(output,"    --%s value", rec->name);
	    if( rec->shortcut) {
		if( rec->flags & ZOPT_ALLOW_POSTFIX ) {
	    	    l += 8+strlen(rec->shortcut);
		    fprintf(output," | -%svalue",rec->shortcut);
		} else {
	    	    l += 9+strlen(rec->shortcut);
		    fprintf(output," | -%s value",rec->shortcut);
		}
	    }
	}
	if( name || rec->info || default_value) {
	    if( l >= COL ) {
		fprintf(output,"\n");
		printspaces(output,COL+1);
	    } else
		printspaces(output,COL-l);
	    fprintf(output, ": ");
	}
	if( rec->info ) 
	    fprintf(output,"%s",rec->info);
	if( name )
	    fprintf(output," - %s",name);
	if( default_value ) 
	    fprintf(output," (default=%s)",default_value);
	fprintf(output,"\n");
    }
    return NULL;
}
int zc_getopt(struct zopt_rec* rec,int no,
            int* argc,char*** argv,int flags,
	    void (*usage)(const char* msg))
{
    int margc;
    int	result = 0,cont = 1;
    char** margv;
    char* s;
    const char* p;
    char* new_p;
    char buf[1000];
    zopt_rec* frec;
    if( !argc || !argv )
	return -1;
    margc = *argc;
    margv = *argv;

    while( cont && *margv && margv[0][0] == '-' ) {
	p = 0;
	frec = 0;
	if( margv[0][1] == '\0' ) {
	    /* We have "-", so no more arguments will be options
		so silently quit
	    */
	    cont = 0;
	    result = 0;
	} else if( margv[0][1] == '-' ) {
	    /* long form */
	    s = margv[0] + 2;
	    frec = match_long_form(rec,no,s,&p);
	} else {
	    /* short form */
	    s = margv[0] + 1;
	    frec = match_short_form(rec,no,s,&p);
	}
	if( frec ) {
	    if( frec->type >= ZOPT_STRING && !(frec->flags & ZOPT_NO_PARAM) ) {
		if( !p ) {
		    if( !margv[1] ) {
			/* ERR: we need parameter but it does not exists */
			sprintf(buf,"argument to '%s' is missing",margv[0]);
			usage(buf);
			result = -1;
			goto _ret;
		    }
		    p = margv[1];
		    margv++;
		    margc--;
		}
	    }
	    /* now we have frec found with parameter p (if needed) */
	    switch( frec->type ) {
	    case ZOPT_BOOL:
		{
		    int* b = (int*) frec->param;
		    int bp = p ? string2bool(p) : 1;
		    if( b ) *b = bp;
		}
		break;
	    case ZOPT_NEGBOOL:
		{
		    int* b = (int*) frec->param;
		    int bp = p ? string2bool(p) : 0;
		    if( b ) *b = bp;
		}
		break;
	    case ZOPT_DOUBLE:
		{
		    double* x = (double*) frec->param;
		    double dp = strtod(p,&new_p);
		    double xp = p ? dp : (double)frec->size;
		    if( x ) *x = xp;
		}
	    case ZOPT_INT:
		{
		    int* x = (int*) frec->param;
		    int dp = strtol(p,&new_p,0);
		    int xp = p ? dp : frec->size;
		    if( x ) *x = xp;
		}
		break;
	    case ZOPT_STRING:
		{
		    const char** d = (const char**) frec->param;
		    const char* dp = p ? p : (const char*)frec->size;
		    if( d ) *d = dp;
		}
		break;
	    case ZOPT_CALLBACK:
		{
		    int (*callback)(const char*) = (int (*)(const char*))frec->param;

		    if( callback )
			if( callback(p ? p : (const char*)frec->size) < 0 ) {
			    result = -1;
			    goto _ret;
			}
		}
		break;
	    }
	} else {
	    sprintf(buf,"%s unrecongnized option",margv[0]);
	    usage(buf);
	    result = -1;
	    goto _ret;
	}
	margv++;
	margc--;
    }
_ret:
    *argv = margv;
    *argc = margc;
    return result;
}


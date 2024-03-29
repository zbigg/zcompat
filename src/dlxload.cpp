/*
    DLX Dynamic Loading and eXecution V2.91-2
    
    Copyright (c) 2001, Zbigniew Zagorski
        
*   This program is Copyright(c) Zbigniew Zag�rski 1999-2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.


   Modifications:
	    * Added DLXErrorStr
	    * Allegro DAT file support turned off
*/
// DLX Dynamic Loading and eXecution V2.91
// Copyright (c) 1997-1998, Nanosoft, Inc.

/*
	Todo:
	      * Fix bug in bidirectional (and let libraries load themselves)
	      * Fix instantiation
	      * Fix VERGEN
	      * Rewrite DLXList
	      * Rewrite DLXGen
	      * Add CodeAlloc
	      - Add DPLUG interface
	      * Fix use of new operator
	      * Check why silent failures can occur
	      * Finish memory bulletproofing
	      * Add resource files
	      * Add finalization for destructors
	      * Insert all relevant error codes
	      * Check myimps and derror for invalid usage
	      * Fix auto-unloader
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __MAIN_INCLUDED__
#include <stddef.h>
#include <stdio.h>
#include <iostream.h>
#include <malloc.h>
#include <io.h>
#include <string.h>
// #include <pc.h>
#include "zsystype.h"
#include "dlx.h"


#if !defined ALLEGRO_H && 0
#define ALLEGRO_H
typedef struct DATAFILE_PROPERTY
{
   char *dat;			       /* pointer to the data */
   int type;			       /* property type */
} DATAFILE_PROPERTY;


typedef struct DATAFILE
{
   void *dat;			       /* pointer to the data */
   int type;			       /* object type */
   long size;			       /* size of the object */
   DATAFILE_PROPERTY *prop;	       /* object properties */
} DATAFILE;

extern "C" DATAFILE *load_datafile(char *filename);
extern "C" void unload_datafile(DATAFILE *dat);

#endif



//#include <unistd.h>
//#include <dir.h>

#define EXE_HEADER   512

#define DLX_ID(a,b,c,d)     ((a<<24) | (b<<16) | (c<<8) | d)

#define ID_DLXDATAFILE DLX_ID('0','T','A','D')

typedef ZLONG_LONG Hdlx_t;

class CDLXImports {
	protected:
		char** names;
		long* weakoff;
		long* numlocs;
		long** relocs;
		long numimports;
		long errors;
	public:
		CDLXImports(void* infile, long nums);
		~CDLXImports();
		void* operator new(size_t mysize);
		void operator delete(void* p);
		void Replace(char* data, char* name, char* placement);
		void ReplaceWeak(char* data, char* name, char* placement);
		long CheckEmpty();
		long Validate();
};
class CDLX;
class CDLXListLink;
class CDLXLinkedList {
	protected:
		friend CDLXListLink;
		friend CDLX;
		CDLX* mypointer;
		CDLXLinkedList* next;
		CDLXLinkedList* last;
	public:
		void* operator new(size_t mysize);
		void operator delete(void* p);
		CDLXLinkedList(CDLX*);
		~CDLXLinkedList();
		operator CDLX*();
		void Link(CDLXLinkedList* mydlx);
};
class CDLXListLink {
	protected:
		CDLXLinkedList* mylist;
		CDLXLinkedList* lastinline;
	public:
		CDLXListLink();
		~CDLXListLink();
		void AddLink(CDLXLinkedList* mylink);
		int AddNewLink(CDLXLinkedList* mylink);
		CDLXLinkedList* GetLink(char* name);
		CDLXLinkedList* GetLink(hdlx_t cl);
		void RemoveLink(CDLXLinkedList* mylink);
		void GetLinks(CDLXListLink& tolink);
		CDLXLinkedList* GetFullList();
	public:
		// passed on functions (for CDLX):
		void Unload1();
		void Unload2();
		void Clear();
		int Pass3();
		void RInstanceInc();
		void Initialize();
		void RInstanceDec();
		void StripToUnload();
		void DeInitialize();
		long Allow(hdlx_t toal);
		void LUnload();
		void Suicide();
};
class CDLXSymbolList {
	protected:
		char*** tosymbols;
		hdlx_t* ids;
		long numdlx;
	public:
		CDLXSymbolList();
		~CDLXSymbolList();
		int AddExports(hdlx_t id, char** tosymbol);
		long Resolve(CDLXListLink& ils, CDLXImports& ims, char* data);
		void RemoveExports(hdlx_t id);
};
class CEntryTracker {
	protected:
		void** functions;
		char** names;
		long numentries;
	public:
		CEntryTracker();
		~CEntryTracker();
		long AddPoint(char* name, void* function);
		long AddEntries(char **data);
		void* GetEntry(char* name);
};
class CDLX {
	public:
		friend CDLXListLink;
		char *args;
		char* data;
		long datalen;
		void* resdata;
		long reslen;
	protected:
		void (*LibMain)(int, Hdlx_t, char*);
		char* name;
		/* this was used for instance r && l, now is in public */
		long libloadpos;
		CDLXImports* myimps;
		long derror;
		dlxiddesc versions;
		CEntryTracker myentries;
		CDLXListLink mylibs;
		ZLONG_LONG dplugid;
	public:
		long linstance;
		long lmaxinstance;  /* new, my */
		long rinstance;
		long rmaxinstance;  /* new, my */
		int  loverload;     /* new, my */
		void*datafile;	    /* new, my */
		void* operator new(size_t mysize);
		void operator delete(void* p);
		int AddDependancies(CDLXListLink& m);
		CDLX(char* iname, void* infile);
		~CDLX();
		operator hdlx_t();
		int operator==(char*);
		int operator++();
		int operator--();
		int operator!();
		void* GetEntry(char* name);
		CDLX* Pass1();
		int Pass2();
		int Pass3();
		void RInstanceInc();
		void LInstanceInc();
		void Initialize();
		void RInstanceDec();
		void LInstanceDec();
		void DeInitialize();
};

static CDLXListLink _dlxlist;
static CDLXListLink _toilist;
static CDLXListLink _toflist;
static void dlx_auto_unload() __attribute__((destructor));
static CDLXSymbolList GlobalSymbols;

#define LMAXINSTANCE	((1<<31)-1) /* my */
#define RMAXINSTANCE	((1<<31)-1) /* my */

static void  _DefaultSetLOverload(hdlx_t n, int i ){ /* my */
  CDLXLinkedList* tdlx = _dlxlist.GetLink(n);
  if ( tdlx && (CDLX*)*tdlx ) ((CDLX*)*tdlx)->loverload = i;
};

static void  _DefaultSetLMaxInstance(hdlx_t n, long i ){ /* my */
  CDLXLinkedList* tdlx = _dlxlist.GetLink(n);
  if ( tdlx && (CDLX*)*tdlx ) ((CDLX*)*tdlx)->lmaxinstance = i;
};

static	long _DefaultGetLInstance(hdlx_t n){ /* my */
  CDLXLinkedList* tdlx = _dlxlist.GetLink(n);
  if ( tdlx && (CDLX*)*tdlx ) return ((CDLX*)*tdlx)->linstance;
  return 0;
};

static	long _DefaultGetLMaxInstance(hdlx_t n){ /* my */
  CDLXLinkedList* tdlx = _dlxlist.GetLink(n);
  if ( tdlx && (CDLX*)*tdlx ) return ((CDLX*)*tdlx)->lmaxinstance;
  return 0;
};

static	int _DefaultGetLOverload(hdlx_t n){ /* my */
  CDLXLinkedList* tdlx = _dlxlist.GetLink(n);
  if ( tdlx && (CDLX*)*tdlx ) return ((CDLX*)*tdlx)->loverload;
  return 0;
};

static hdlx_t _DefaultGet(char* f){  /* my */
  CDLXLinkedList* tdlx = _dlxlist.GetLink(f);
  if ( tdlx ) return (hdlx_t)((CDLX*)*tdlx);
  return 0;
}

static	void* _DefaultGetFileData(hdlx_t n) { /* my */
  CDLXLinkedList* tdlx = _dlxlist.GetLink(n);
  if ( tdlx && (CDLX*)*tdlx ) return ((CDLX*)*tdlx)->datafile;
  return NULL;
};

#if defined ZDEV_MSC
#define size_t unsigned long
#endif

static	long  DLXGetLInstanceC(CDLX *c) { return (c?c->linstance:0); }; /* my */
static	long  DLXGetLMaxInstanceC(CDLX *c) { return (c?c->lmaxinstance:0); }; /* my */
static	int   DLXGetLOverloadC(CDLX *c) { return (c?c->loverload:0); }; /* my */

static void* _DefaultOpen(char* name){return((void*)fopen(name,"rb"));}
static void _DefaultClose(void* fl){fclose((FILE*)fl); }
static long _DefaultRead(void* d, long l, void* fl){ return fread(d,1,l,(FILE*)fl); }
static void* _DefaultMalloc(size_t s){return malloc(s);}
static void* _DefaultCodeAlloc(size_t s, long memcls){if(memcls!=0) return NULL; return malloc(s);}
static void* _DefaultRealloc(void* m, size_t s){return realloc(m,s);}
static void _DefaultFree(void* m){free(m);}
static char* __dlxerrortbl[]= { 
    "file error:",
        "DLX not in memory:",
        "unresolved external:",
        "error unloading named symbols:",
        "invalid DLX:",
        "illegal operating system:",
        "load Error:",
        "memory allocation error:",
        "code memory allocation error:",
        "file read error:",
        "DLX critical failure:" 
};
static char _dlx_error_buf[300] = "no error";

void _DefaultError(long t,char *i)
{
    sprintf(_dlx_error_buf,"%s %s", __dlxerrortbl[t], i);
};
const char* DLXErrorStr(void)
{
    return  _dlx_error_buf;
}

void* (*DLXOpenFile)(char*)=_DefaultOpen;
void (*DLXCloseFile)(void*)=_DefaultClose;
long (*DLXReadFile)(void*,long,void*)=_DefaultRead;
void (*DLXError)(long,char*)=_DefaultError;
void* (*DLXMalloc)(size_t)=_DefaultMalloc;
void (*DLXFree)(void*)=_DefaultFree;
void* (*DLXRealloc)(void*,size_t)=_DefaultRealloc;
int   (*DLXGetLOverload)(hdlx_t n) = _DefaultGetLOverload; /* my */
long  (*DLXGetLMaxInstance)(hdlx_t n) = _DefaultGetLMaxInstance; /* my */
long  (*DLXGetLInstance)(hdlx_t n) = _DefaultGetLInstance; /* my */
void  (*DLXSetLMaxInstance)(hdlx_t n, long i) = _DefaultSetLMaxInstance; /* my */
void  (*DLXSetLOverload)(hdlx_t n, int i) = _DefaultSetLOverload; /* my */
void* (*DLXGetFileData)(hdlx_t n) = _DefaultGetFileData; /* my */
hdlx_t (*DLXGetID)(char*)=_DefaultGet;
void* (*DLXCodeAlloc)(size_t s, long mclas)  =_DefaultCodeAlloc;

#if defined ZDEV_MSC
#undef size_t
#endif

static CDLX* _DLXLoad1(char* name)
{
	void* me=DLXOpenFile(name);
	if(me==NULL)
	{
		DLXError(0,name);
		return NULL;
	}
	CDLX* ndlx=new CDLX(name,me);
	DLXCloseFile(me);
	if(ndlx==NULL)
	{
		DLXError(7,name);
		return NULL;
	}
	ndlx=ndlx->Pass1();
	return ndlx;
}
static void _DLXUnload1(CDLX* d)
{
	delete(d);
}
static CDLX* _DLXLoad2(char* name)
{
	CDLXLinkedList* tdlx=_dlxlist.GetLink(name);
	if(tdlx!=NULL)
	   if ( DLXGetLInstanceC((CDLX*)*tdlx) < DLXGetLMaxInstanceC((CDLX*)*tdlx) ) { /* that's my */
	      if ( DLXGetLOverloadC((CDLX*)*tdlx) ) return (CDLX*)*tdlx;
	      else tdlx = NULL;
	   } else return NULL;

	tdlx=_toilist.GetLink(name);
	if(tdlx!=NULL)
		return (CDLX*)*tdlx;
	hdlx_t test=DLXGetID(name);
	if(test!=0)
	{
		tdlx=_dlxlist.GetLink(test);
		if(tdlx!=NULL)
		   if ( DLXGetLInstanceC((CDLX*)*tdlx) < DLXGetLMaxInstanceC((CDLX*)*tdlx) ) { /* that's my */
		      if ( DLXGetLOverloadC((CDLX*)*tdlx) ) return (CDLX*)*tdlx;
		      else tdlx = NULL;
		   } else return NULL;

		tdlx=_toilist.GetLink(test);
		if(tdlx!=NULL)
			return (CDLX*)*tdlx;
	}
	CDLX* ndlx=_DLXLoad1(name);
	if(ndlx==NULL)
		return NULL;
	CDLXLinkedList* n=new CDLXLinkedList(ndlx);
	if(n==NULL)
	{
		DLXError(7,name);
		_DLXUnload1(ndlx);
		return NULL;
	}
	_toilist.AddNewLink(n);
	return ndlx;
}
static void _DLXUnload2(CDLX* d)
{
	CDLXLinkedList* n=_toilist.GetLink((hdlx_t)*d);
	if(n==NULL)
		return;
	_toilist.RemoveLink(n);
	_DLXUnload1(d);
}
// continue here...
static CDLX* _DLXLoad3(char* name, char* args)
{
	CDLX* tdlx=_DLXLoad2(name);
	if(tdlx==NULL)
		return NULL;
	CDLXLinkedList* n=new CDLXLinkedList(tdlx);
	if(n==NULL)
	{
		DLXError(7,name);
		_DLXUnload2(tdlx);
		return NULL;
	}
	if(_toflist.AddNewLink(n))  // only load libs if not linked...
		if(tdlx->Pass2())  // load libraries...
		{
			_toilist.Unload1();
			_toflist.Clear();
			return NULL;
		}
	tdlx->args = strdup(args);
	return tdlx;
}


hdlx_t _DLXLoad(char* name, char *args)
{
	strlwr(name);

	if(name==NULL)
		return 0;
	CDLX* myd=_DLXLoad3(name, args);
	if(myd==NULL)
	{
		return 0;
	}
	hdlx_t mydlx=(hdlx_t)*myd;
	if(_toilist.Pass3())	    // intrace
	{
		_toilist.Unload1();
		_toflist.Clear();
		return 0;
	}
	_toilist.Initialize();
	_dlxlist.GetLinks(_toilist);
	_toflist.RInstanceInc();
	myd->LInstanceInc();
	_toflist.Clear();
	return mydlx;
}
static int __DLXUnload(CDLX* tdlx)
{
	if(tdlx==NULL)
	{
		DLXError(1,"(by number)");
		return 1;
	}
	if(tdlx->AddDependancies(_toflist))
	{
		return 1;
	}
	// working from here...
	tdlx->LInstanceDec();
	_toflist.RInstanceDec();
	_toilist.GetLinks(_toflist);
	_toilist.StripToUnload();
	_toilist.DeInitialize();
	_toilist.Unload2();
	return 0;
}
int _DLXUnload(hdlx_t handle)
{
	return __DLXUnload((CDLX*)*_dlxlist.GetLink(handle));
}
int _DLXUnloadEx(char* name)
{
	strlwr(name);
	return __DLXUnload((CDLX*)*_dlxlist.GetLink(name));
}
int _DLXImport(char** symbols)
{
	return GlobalSymbols.AddExports(0,symbols);
}
void* _DLXGetEntry(hdlx_t target, char* name)
{
	CDLX* tdlx=(CDLX*)*_dlxlist.GetLink(target);
	if(tdlx==0)
		return NULL;
	return tdlx->GetEntry(name);
}
void* _DLXGetMemoryBlock(hdlx_t target)
{
	CDLX* tdlx=(CDLX*)*_dlxlist.GetLink(target);
	if(tdlx==0)
		return NULL;
	return tdlx->data;
}
void* _DLXGetResource(hdlx_t target)
{
	CDLX* tdlx=(CDLX*)*_dlxlist.GetLink(target);
	if(tdlx==0)
		return NULL;
	return tdlx->resdata;
}
long _DLXGetResourceLength(hdlx_t target)
{
	CDLX* tdlx=(CDLX*)*_dlxlist.GetLink(target);
	if(tdlx==0)
		return NULL;
	return tdlx->reslen;
}
long _DLXGetMemoryBlockLength(hdlx_t target)
{
	CDLX* tdlx=(CDLX*)*_dlxlist.GetLink(target);
	if(tdlx==0)
		return NULL;
	return tdlx->datalen;
}


hdlx_t (*DLXLoad)(char* name, char* args) = &_DLXLoad;
int    (*DLXUnload)(hdlx_t handle) = &_DLXUnload;
int    (*DLXUnloadEx)(char* name) = &_DLXUnloadEx;
int    (*DLXImport)(char** symbols) = &_DLXImport;
void*  (*DLXGetEntry)(hdlx_t target, char* name) = &_DLXGetEntry;
void*  (*DLXGetMemoryBlock)(hdlx_t target) = &_DLXGetMemoryBlock;
long   (*DLXGetMemoryBlockLength)(hdlx_t target) = &_DLXGetMemoryBlockLength;
void*  (*DLXGetResource)(hdlx_t target) = &_DLXGetResource;
long   (*DLXGetResourceLength)(hdlx_t target) = &_DLXGetResourceLength;


CDLXImports::CDLXImports(void* infile, long nums)
{
	long l;
	if(this==NULL)
		return;
	errors=0;
	numimports=nums;
	names = (char**)DLXMalloc(sizeof(char *)*nums);
	numlocs = (long*)DLXMalloc(sizeof(long)*nums);
	relocs = (long**)DLXMalloc(sizeof(long*)*nums);
	weakoff = (long*)DLXMalloc(sizeof(long)*nums);
	if( names == NULL || numlocs==NULL||relocs==NULL||weakoff==NULL)
	{
		DLXError(7,"(symbol imports)");
		errors=1;
		return;
	}
	long stlen;
	for(l=0; l<nums; l++)
	{
		names[l]=NULL;
		relocs[l]=NULL;
	}
	for(l=0; l<nums; l++)
	{
		if(DLXReadFile(&stlen, sizeof(long),infile)!=sizeof(long))
		{
			DLXError(0,"(symbol imports)");
			errors=1;
			return;
		}
		names[l]=(char *)DLXMalloc(stlen);
		if(names[l]==NULL)
		{
			DLXError(7,"(symbol imports)");
			errors=1;
			return;
		}
		if(DLXReadFile(names[l],stlen,infile)!=stlen)
		{
			DLXError(0,"(symbol imports)");
			errors=1;
			return;
		}
		weakoff[l]=0;
		if(DLXReadFile(&numlocs[l],sizeof(long),infile)!=sizeof(long))
		{
			DLXError(0,"(symbol imports)");
			errors=1;
			return;
		}
		relocs[l]=(long *)DLXMalloc(sizeof(long)*numlocs[l]);
		if(relocs[l]==NULL)
		{
			DLXError(7,"(symbol imports)");
			errors=1;
			return;
		}
		if(DLXReadFile(relocs[l],sizeof(long)*numlocs[l],infile)!=sizeof(long)*numlocs[l])
		{
			DLXError(0,"(symbol imports)");
			errors=1;
			return;
		}
	}
}
long CDLXImports::Validate()
{
	if(errors==0)
		return 0;
	// memory cleanups
	if(numlocs!=NULL)
		DLXFree(numlocs);
	if(weakoff!=NULL)
		DLXFree(weakoff);
	for(long l=0; l<numimports; l++)
	{
		if(names!=NULL)
			if(names[l]!=NULL)
				DLXFree(names[l]);
		if(relocs!=NULL)
			if(relocs[l]!=NULL)
				DLXFree(relocs[l]);
	}
	return 1;
}
CDLXImports::~CDLXImports()
{
	if(errors)
		return;
	DLXFree(numlocs);
	DLXFree(weakoff);
	for(long l=0; l<numimports; l++)
	{
		DLXFree(names[l]);
		DLXFree(relocs[l]);
	}
	DLXFree(names);
	DLXFree(relocs);
}
void CDLXImports::Replace(char* data, char* name, char* placement)
{
	if(strlen(name)==0) return;
	for(long l=0; l<numimports; l++)
		if(strcmp(name, names[l])==0)
		{
			long adj=0;
			if(weakoff[l])
				adj=(char *)weakoff[l]-data;
			names[l][0]=0;
			for(long l2=0; l2<numlocs[l]; l2++)
			{
				*(long *)(data+relocs[l][l2])+=placement-data-adj;
			}
		}
}
void CDLXImports::ReplaceWeak(char* data, char* name, char* placement)
{
	if(strlen(name)==0) return;
	for(long l=0; l<numimports; l++)
		if(strcmp(name, names[l])==0)
		{
			long weakpos=weakoff[l];
			weakoff[l]=(long)placement;
			long adj=0;
			if(weakpos)
				adj=(char *)weakpos-data;
			names[l][0]=0;
			for(long l2=0; l2<numlocs[l]; l2++)
			{
				*(long *)(data+relocs[l][l2])+=placement-data-adj;
			}
		}
}
long CDLXImports::CheckEmpty()
{
	for(long l=0; l<numimports; l++)
		if(strlen(names[l])!=0)
		{
			if(weakoff[l]==0)
			{
				DLXError(2,names[l]);
				return 1;
			}
		}
	return 0;
}
CDLXSymbolList::CDLXSymbolList()
{
	numdlx=0;
	ids=NULL;
	tosymbols=NULL;
}
CDLXSymbolList::~CDLXSymbolList()
{
	if(ids!=NULL)
		free(ids);
	if(tosymbols!=NULL)
		free(tosymbols);
}
int CDLXSymbolList::AddExports(hdlx_t id, char** tosymbol)
{
	if(ids==NULL || tosymbols==NULL)
	{
		if(ids==NULL)
			ids=(hdlx_t*)DLXMalloc(0);
		if(tosymbols==NULL)
			tosymbols=(char ***)DLXMalloc(0);
		if(ids==NULL || tosymbols==NULL)
		{
			DLXError(10,"Symbol List Allocation Failed!");
			return 1;
		}
	}
	numdlx++;
	hdlx_t* ids2=(hdlx_t*)DLXRealloc(ids, sizeof(hdlx_t)*numdlx);
	char*** tosymbols2=(char***)DLXRealloc(tosymbols,sizeof(char**)*numdlx);
	if(ids2==NULL || tosymbols2==NULL)
	{
		DLXError(7,"(by number)");
		numdlx--;
		if(ids2!=NULL)
			ids=ids2;
		if(tosymbols2!=NULL)
			tosymbols=tosymbols2;
		return 1;
	}
	ids=ids2;
	tosymbols=tosymbols2;
	ids[numdlx-1]=id;
	tosymbols[numdlx-1]=tosymbol;
	return 0;
}
void CDLXSymbolList::RemoveExports(hdlx_t id)
{
	if(ids==NULL || tosymbols==NULL)
	{
		DLXError(10,"Symbol List Allocation Failed!");
		return;
	}
	for(long l=0; l<numdlx; l++)
		if(id==ids[l])
		{
			ids[l]=ids[numdlx-1];
			tosymbols[l]=tosymbols[numdlx-1];
			numdlx--;
			hdlx_t* ids2=(hdlx_t*)DLXRealloc(ids, sizeof(hdlx_t)*numdlx);
			char*** tosymbols2=(char***)DLXRealloc(tosymbols,sizeof(char**)*numdlx);
			// cant really fail, just will suck some memory temporarily
			if(ids2==NULL || tosymbols2==NULL)
			{
				if(ids2!=NULL)
					ids=ids2;
				if(tosymbols2!=NULL)
					tosymbols=tosymbols2;
				return;
			}
			ids=ids2;
			tosymbols=tosymbols2;

			return;
		}
	DLXError(3,"(by number)");
}
long CDLXSymbolList::Resolve(CDLXListLink& ils, CDLXImports& ims, char* data)
{
	if(ids==NULL || tosymbols==NULL)
	{
		DLXError(10,"Symbol List Allocation Failed!");
		return 1;
	}
	for(long l=0; l<numdlx; l++)
	{
		if(ils.Allow( ids[l] ) )
		{
			char** stemps=tosymbols[l];
			while(1)
			{
				if(*stemps==0)
				{ // entries and stuff...
					if( stemps[1] == 0 )
					{
						break;
					} else if( (long)stemps[1] == 1 )
					{
						stemps+=4;
					} else if( (long)stemps[1] == 2 )
					{
						ims.ReplaceWeak(data, stemps[2], stemps[3]);
						stemps+=4;
					}
				} else {
					ims.Replace(data, stemps[0], stemps[1]);
					stemps+=2;
				}
			}
		}
	}
	return(ims.CheckEmpty());
}

CDLX::CDLX(char* iname, void* infile)
{
	if(this==NULL)
		return;
	long mclas=0;
	args = NULL;
	derror=0;
	rinstance=0;
	linstance=0;
	loverload=0; /* my */
	datafile=NULL; /* my */
	rmaxinstance=RMAXINSTANCE; /* my */
	lmaxinstance=LMAXINSTANCE; /* my */
	void* resdata=NULL;
	long reslen=0;
	name=(char*)DLXMalloc(strlen(iname)+1);
	if(name==NULL)
	{
		DLXError(7,iname);
		derror=1;
		return;
	}
	strcpy(name,iname);
	dlxheader dh;
	if(DLXReadFile(&dh, sizeof(dh),infile)!=sizeof(dh))
	{
		DLXError(0,iname);
		derror=2;
		return;
	}

	if ( dh.magic != DLX3_MAGIC && dh.magic != DLX2_MAGIC ) { /* if exe seek to 512 in file */
	  fseek((FILE*)infile, EXE_HEADER, SEEK_SET);
	  if(DLXReadFile(&dh, sizeof(dh),infile)!=sizeof(dh))
	  {
		DLXError(0,iname);
		derror=2;
		return;
	  }
	}

	if( dh.magic == DLX3_MAGIC )
	{
		if(DLXReadFile(&mclas, sizeof(long), infile)!=sizeof(long))
		{
			DLXError(0,iname);
			derror=2;
			return;
		}
		if(DLXReadFile(&dplugid, sizeof(ZLONG_LONG), infile)!=sizeof(ZLONG_LONG))
		{
			DLXError(0,iname);
			derror=2;
			return;
		}
		if(DLXReadFile(&reslen, sizeof(long), infile)!=sizeof(long))
		{
			DLXError(0,iname);
			derror=2;
			return;
		}
	} else if( dh.magic != DLX2_MAGIC )
	{
		//DLXError(4,iname); /* it's not DLX */
		derror=2;
		return;
	}
	libloadpos=dh.libloadpos;
	ZLONG_LONG platform;
	if(DLXReadFile(&platform,sizeof(ZLONG_LONG),infile)!=sizeof(ZLONG_LONG))
	{
		DLXError(0,iname);
		derror=2;
		return;
	}
	if( platform != DLX_BUILD_DOS )
	{
		char a[9]={0,0,0,0,0,0,0,0,0};
		memcpy(a, &platform, 8);
		DLXError( 5, a );
		derror=2;
		return;
	}
	if(DLXReadFile( &versions, sizeof(dlxiddesc), infile )!=sizeof(dlxiddesc))
	{
		DLXError(0,iname);
		derror=2;
		return;
	}
	myimps=new CDLXImports(infile,dh.numimports);
	if(myimps==NULL)
	{
		DLXError(7,name);
		derror=2;
		return;
	}
	if(myimps->Validate())
	{
		derror=2;
		return;
	}
	data=(char *)DLXCodeAlloc(dh.prgsize,mclas);
	if(data==NULL)
	{
		DLXError(8,iname);
		derror=3;
		return;
	}
	datalen=dh.prgsize;
	if(DLXReadFile(data, dh.prgsize, infile)!=dh.prgsize)
	{
		DLXError(0,iname);
		derror=4;
		return;
	}
	for(long l=0; l<dh.numrelocs; l++)
	{
		long reloc;
		if(DLXReadFile(&reloc,sizeof(long),infile)!=sizeof(long))
		{
			DLXError(0,iname);
			derror=4;
			return;
		}
		*((long *)(data+reloc))+=(long)data;
	}
	LibMain=(void(*)(int, Hdlx_t, char*))(data+dh.libmainpos);
	// export all exports...
	if(GlobalSymbols.AddExports(versions.UNID, (char **)(data+dh.extablepos)))
	{
		derror=4;
		return;
	}
	if(myentries.AddEntries( (char **)(data+dh.extablepos) ))
	{
		derror=4;
		return;
	}
	if( (resdata=DLXMalloc(reslen))==NULL )
	{
		DLXError(7,iname);
		derror=4;
		return;
	}
	if(DLXReadFile(resdata,reslen,infile)!=reslen)
	{
		DLXFree(resdata);
		DLXError(0,iname);
		derror=4;
		return;
	}
	// load all libraries...

	/* data file loading ( my ) */
	#ifdef ALLEGRO_H
	long oldpos = ftell((FILE*)infile);
	long il = filelength(fileno((FILE*)infile));
	long id_dat = 0;
	fseek((FILE*)infile, il-sizeof(long), SEEK_SET);
	DLXReadFile(&id_dat, sizeof(long), infile);
	if ( id_dat == ID_DLXDATAFILE ) { /* there is DATAFILE at the end */
	  long len = 0;
	  fseek((FILE*)infile, il-(sizeof(long)*2), SEEK_SET);
	  DLXReadFile(&len, sizeof(long), infile);
	  if ( len > 0 ) {
	    fseek((FILE*)infile, il-((sizeof(long)*2)+len), SEEK_SET);
	    char *dta = (char*)malloc(len);
	    if ( dta ) {
	      DLXReadFile(dta, len, infile);
	      FILE *out = fopen("_0_dat__._x_", "wb");
	      if ( out ) {
		fwrite(dta, len, 1, out);
		fclose(out);
		datafile = (void*)load_datafile("_0_dat__._x_");
		remove("_0_dat__._x_");
	      };
	    };
	  };
	};

	fseek((FILE*)infile, oldpos, SEEK_SET);
	#endif
	/* end of my */
}
CDLX* CDLX::Pass1()
{
	// just do error cleanup...
	if(derror==0)
		return this;
	if(derror==1)
	{
		delete this;
		return NULL;
	}
	if(derror==2)
	{
		free(name);
		delete this;
		return NULL;
	}
	if(derror==3)
	{
		free(name);
		delete myimps;
		delete this;
		return NULL;
	}
	if(derror==4)
	{
		free(name);
		free(data);
		delete myimps;
		delete this;
		return NULL;
	}

	// oh oh...
	return NULL;
}
int CDLX::Pass2()
{
	char** myptr=(char **)(data+libloadpos);
	while( (**myptr) != 0)
	{
		CDLX* loh=_DLXLoad3(*myptr, args);
		if(loh==NULL)
		{
			delete(myimps);
			return 1;
		}
		CDLXLinkedList* myl=new CDLXLinkedList(loh);
		if(myl==NULL)
		{
			DLXError(7,name);
			delete(myimps);
			return 1;
		}
		mylibs.AddNewLink(myl);
		myptr++;
	}
	return 0;
}
int CDLX::Pass3()
{
	// resolve symbols...

	if(GlobalSymbols.Resolve(mylibs,*myimps, data))
	{
		delete(myimps);
		return 1;
	}
	delete(myimps);

	return 0;
}
CDLX::~CDLX()
{
	if(!derror)
	{
		free(args);
		args = NULL;
		GlobalSymbols.RemoveExports(versions.UNID);
		DLXFree(data);
		DLXFree(name);
		#ifdef ALLEGRO_H
		unload_datafile((DATAFILE*)datafile);
		#endif
	}
}
int CDLX::operator==(char*i)
{
	return (strcmp(name,i)==0);
}
int CDLX::operator!()
{
	return(rinstance==0 && linstance==0);
}
CDLX::operator hdlx_t()
{
	return( versions.UNID );
}
void* CDLX::GetEntry(char* name)
{
	return(myentries.GetEntry(name));
}
void CDLX::RInstanceInc()
{
      if ( rinstance < rmaxinstance ) { /* my */
	rinstance++;
	LibMain(4, (Hdlx_t)(*this), args);
      };
}
void CDLX::LInstanceInc()
{
	if ( linstance < lmaxinstance ) { /* my */
	  linstance++;
	  LibMain(2, (Hdlx_t)(*this), args);
	};
}
void CDLX::Initialize()
{
	LibMain(0, (Hdlx_t)(*this), args);
}
void CDLX::RInstanceDec()
{
      if ( rinstance > 0 ) { /* my */
	rinstance--;
	LibMain(5, (Hdlx_t)(*this), NULL);
      };
}
void CDLX::LInstanceDec()
{
	if ( linstance > 0 ) { /* my */
	  linstance--;
	  LibMain(3, (Hdlx_t)(*this), NULL);
	};
}
void CDLX::DeInitialize()
{
	LibMain(1, (Hdlx_t)(*this), NULL);
}
int CDLX::AddDependancies(CDLXListLink& m)
{
	CDLXLinkedList* m2=new CDLXLinkedList(this);
	if(m2==NULL)
	{
		DLXError(7,name);
		return 1;
	}
	m.AddNewLink(m2);
	CDLXLinkedList* myl=mylibs.GetFullList();
	while(myl!=NULL)
	{
		if(m.GetLink((hdlx_t)*(CDLX*)*myl)==NULL)
		{
			if(((CDLX*)*myl)->AddDependancies(m))
				return 1;
		}
		myl=myl->next;
	}
	return 0;
}
CEntryTracker::CEntryTracker()
{
	if(this==NULL)
		return;
	functions=(void **)DLXMalloc(0);
	names=(char **)DLXMalloc(0);
	numentries=0;
}
CEntryTracker::~CEntryTracker()
{
	if(functions!=NULL)
		DLXFree(functions);
	if(names!=NULL)
	{
		for(long a=0; a<numentries; a++)
			if(names[a]!=NULL)
				DLXFree( names[a] );
		DLXFree(names);
	}
}
long CEntryTracker::AddPoint(char* name, void* function)
{
	if(names==NULL||functions==NULL)
	{
		DLXError(7, "(Entry Tracker)");
		return 1;
	}
	numentries++;
	char** n2=(char **)DLXRealloc(names, sizeof(char *)*numentries);
	if(n2==NULL)
	{
		DLXError(7, "(Entry Tracker)");
		return 1;
	}
	names=n2;
	char* n3=(char *)DLXMalloc(strlen(name)+1);
	if(n3==NULL)
	{
		DLXError(7, "(Entry Tracker)");
		return 1;
	}
	names[numentries-1]=strcpy(n3,name);
	void** f2=(void **)DLXRealloc(functions, sizeof(void *)*numentries);
	if(f2==NULL)
	{
		DLXError(7, "(Entry Tracker)");
		return 1;
	}
	functions=f2;
	functions[numentries-1]=function;
	return 0;
}
long CEntryTracker::AddEntries(char **stemps)
{
	while(1)
	{
		if(*stemps==0)
		{ // entries and stuff...
			if( stemps[1] == 0 )
			{
				return 0;
			} else if( (long)stemps[1] == 1 )
			{
				if(AddPoint( stemps[2], (void*)stemps[3] ))
					return 1;
				stemps+=4;
			} else if( (long)stemps[1] == 2 )
			{
				stemps+=4;
			}
		} else {
			stemps+=2;
		}
	}
}
void* CEntryTracker::GetEntry(char* name)
{
	for(long l=0; l<numentries; l++)
		if( strcmp(names[l],name)==0 )
			return(functions[l]);
	return NULL;
}
void CDLXListLink::LUnload()
{
	while(mylist!=NULL)
	{
		CDLXLinkedList* myl=mylist;
		long decs=0;
		while(myl!=NULL)
		{
			if( ((CDLX*)*myl)->linstance > 0 )
			{
				decs++;
				__DLXUnload( (CDLX*)*myl );
				break;
			}
			myl=myl->next;
		}
		if(decs==0)
			return;  // well we failed the unload...
	}
}
void CDLXListLink::Suicide()
{
	CDLXLinkedList* myl=mylist;
	while(myl!=NULL)
	{
		((CDLX*)*myl)->LibMain(6, (Hdlx_t)(((CDLX*)*myl)), NULL);
		myl=myl->next;
	}
}
void* CDLX::operator new(size_t mysize)
{
	return DLXMalloc(mysize);
}
void* CDLXImports::operator new(size_t mysize)
{
	return DLXMalloc(mysize);
}
void CDLX::operator delete(void* p)
{
	free(p);
}
void CDLXImports::operator delete(void* p)
{
	free(p);
}
void* CDLXLinkedList::operator new(size_t mysize)
{
	return DLXMalloc(mysize);
}
void CDLXLinkedList::operator delete(void* p)
{
	free(p);
}
CDLXLinkedList::CDLXLinkedList(CDLX* mydlx)
{
	if(this==NULL)
		return;
	mypointer=mydlx;
	next=NULL;
	last=NULL;
}
CDLXLinkedList::~CDLXLinkedList()
{
	if(next!=NULL)
		next->last=last;
	if(last!=NULL)
		last->next=next;
}
CDLXLinkedList::operator CDLX*()
{
	return mypointer;
}
CDLXListLink::CDLXListLink()
{
	mylist=NULL;
	lastinline=NULL;
}
CDLXLinkedList* CDLXListLink::GetFullList()
{
	return mylist;
}
CDLXListLink::~CDLXListLink()
{
	CDLXLinkedList* temp=mylist;
	while(temp!=NULL)
	{
		CDLXLinkedList* temp2=temp->next;
		delete temp; // only deletes the wrapper...
		temp=temp2;
	}
}
void CDLXListLink::AddLink(CDLXLinkedList* mylink)
{
	mylink->Link(mylist);
	mylist=mylink;
	if(lastinline==NULL)
		lastinline=mylink;
}
int CDLXListLink::AddNewLink(CDLXLinkedList* mylink)
{
	CDLXLinkedList* temp=mylist;
	while(temp!=NULL)
	{
		if( (CDLX*)*temp == (CDLX*)*mylink )
		{
			delete mylink; // only deletes the wrapper...
			return 0;
		}
		temp=temp->next;
	}
	mylink->Link(mylist);
	mylist=mylink;
	if(lastinline==NULL)
		lastinline=mylink;
	return 1;
}
void CDLXListLink::GetLinks(CDLXListLink& tolink)
{
	if(tolink.lastinline==NULL || tolink.mylist==NULL)
		return;
	tolink.lastinline->Link(mylist);
	mylist=tolink.mylist;
	tolink.mylist=NULL;
	tolink.lastinline=NULL;
}
void CDLXLinkedList::Link(CDLXLinkedList* mydlx)
{
	if(mydlx!=NULL)
		mydlx->last=this;
	next=mydlx;
}
CDLXLinkedList* CDLXListLink::GetLink(char* name)
{
	CDLXLinkedList* temp=mylist;
	while(temp!=NULL)
	{
		if( *(CDLX*)*temp==name )
			return temp;
		temp=temp->next;
	}
	return NULL;
}
CDLXLinkedList* CDLXListLink::GetLink(hdlx_t cl)
{
	CDLXLinkedList* temp=mylist;
	while(temp!=NULL)
	{
		if( *(CDLX*)*temp==cl )
			return temp;
		temp=temp->next;
	}
	return NULL;
}
void CDLXListLink::RemoveLink(CDLXLinkedList* mylink)
{
	if(mylink==lastinline)
		lastinline=mylink->last;
	if(mylink==mylist)
		mylist=mylink->next;
	delete(mylink);
}
void CDLXListLink::Unload1()
{
	while(mylist!=NULL)
	{
		_DLXUnload1((CDLX*)*mylist);
		RemoveLink(mylist);
	}
}
void CDLXListLink::Unload2()
{
	while(mylist!=NULL)
	{
		// HACK
		CDLXLinkedList* mytest=_dlxlist.GetLink((hdlx_t)*(CDLX*)*mylist);
		if(mytest!=NULL)
			_dlxlist.RemoveLink(mytest);
		_DLXUnload1((CDLX*)*mylist);
		RemoveLink(mylist);
	}
}
void CDLXListLink::Clear()
{
	while(mylist!=NULL)
	{
		RemoveLink(mylist);
	}
}
int CDLXListLink::Pass3()
{
	CDLXLinkedList* myl=mylist;
	while(myl!=NULL)
	{
		if(((CDLX*)*myl)->Pass3())
			return 1;
		myl=myl->next;
	}
	return 0;
}
void CDLXListLink::RInstanceInc()
{
	CDLXLinkedList* myl=mylist;
	while(myl!=NULL)
	{
		((CDLX*)*myl)->RInstanceInc();
		myl=myl->next;
	}
}
void CDLXListLink::RInstanceDec()
{
	CDLXLinkedList* myl=mylist;
	while(myl!=NULL)
	{
		((CDLX*)*myl)->RInstanceDec();
		myl=myl->next;
	}
}
void CDLXListLink::StripToUnload()
{
	CDLXLinkedList* myl=mylist;
	CDLXLinkedList* mylnext;
	while(myl!=NULL)
	{
		mylnext=myl->next;
		if(!!*(CDLX*)*myl)
			RemoveLink(myl);
		myl=mylnext;
	}
}
void CDLXListLink::DeInitialize()
{
	CDLXLinkedList* myl=mylist;
	while(myl!=NULL)
	{
		((CDLX*)*myl)->DeInitialize();
		myl=myl->next;
	}
}
void CDLXListLink::Initialize()
{
	CDLXLinkedList* myl=mylist;
	while(myl!=NULL)
	{
		((CDLX*)*myl)->Initialize();
		myl=myl->next;
	}
}
long CDLXListLink::Allow(hdlx_t toal)
{
	if(toal==0)
		return 1;
	CDLXLinkedList* myl=mylist;
	while(myl!=NULL)
	{
		if( (hdlx_t)*(CDLX*)*myl == toal )
			return 1;
		myl=myl->next;
	}
	return 0;
}
static long dlx_unloader_count=0;
static void dlx_auto_unload()
{
	dlx_unloader_count++;
	if(dlx_unloader_count==1)
	{
		_dlxlist.LUnload();
	} else if(dlx_unloader_count==2) {
		// try to safely die...
		_dlxlist.Suicide();
	} else if(dlx_unloader_count==3) {
		// try to error and die...
		DLXError(10,"Unloader Called Recursively");
	} else {
		return;  // if all else fails, just dump.
	}
}

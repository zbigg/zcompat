###################################################################
#
# File	       : dist.mak
# Project      : zcompat
# Platform     : MSW
#
# Id           : $Id$
#
# 	Author Zbigniew Zagorski <zzbigg@o2.pl>
#
###################################################################
#
LIBDIR=..\..\lib\msvc
DISTDIR=zcompat-dev
!IF "$(DIST)" != ""
DISTDIR=$(DISTDIR)-$(DIST)
!ENDIF

install:
	@echo off
	@-mkdir $(DISTDIR)
	@-mkdir "$(DISTDIR)\include"
	@-mkdir "$(DISTDIR)\include\zcompat"
	@-mkdir "$(DISTDIR)\lib"
!IF "$(LIB)" != ""
	@-copy $(LIBDIR)\$(LIB) "$(DISTDIR)\lib"
!ENDIF
!IF "$(DLL)" != ""
	-copy $(LIBDIR)\$(DLL) "$(DISTDIR)\lib"
!ENDIF
	@-copy ..\..\include\zcompat "$(DISTDIR)\include\zcompat"
	@-copy readme.txt $(DISTDIR)




	

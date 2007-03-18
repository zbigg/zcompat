# Microsoft Developer Studio Project File - Name="zcompat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=zcompat - Win32 Release Static
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zcompat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zcompat.mak" CFG="zcompat - Win32 Release Static"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zcompat - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zcompat - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zcompat - Win32 Debug Static" (based on "Win32 (x86) Static Library")
!MESSAGE "zcompat - Win32 Release Static" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "zcompat"
# PROP Scc_LocalPath "..\..\.."

!IF  "$(CFG)" == "zcompat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../lib/msvc"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZCOMPAT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../" /I "../../../include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZCOMPAT_EXPORTS" /D "HAVE_CONFIG_H" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x415 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "zcompat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../lib/msvc"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZCOMPAT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../" /I "../../../include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZCOMPAT_EXPORTS" /D "HAVE_CONFIG_H" /YX /FD /GZ /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /out:"../../../lib/msvc/zcompatd.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "zcompat - Win32 Debug Static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zcompat___Win32_Debug_Static"
# PROP BASE Intermediate_Dir "zcompat___Win32_Debug_Static"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../lib/msvc"
# PROP Intermediate_Dir "Debug_Static"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../" /I "../../../include" /I "../../../common" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZCOMPAT_EXPORTS" /D "HAVE_CONFIG_H" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../" /I "../../../include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZCOMPAT_EXPORTS" /D "HAVE_CONFIG_H" /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo /out:"../../../lib/msvc/szcompatd.lib"

!ELSEIF  "$(CFG)" == "zcompat - Win32 Release Static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zcompat___Win32_Release_Static"
# PROP BASE Intermediate_Dir "zcompat___Win32_Release_Static"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../lib/msvc"
# PROP Intermediate_Dir "Release_Static"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../" /I "../../../include" /I "../../../common" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZCOMPAT_EXPORTS" /D "HAVE_CONFIG_H" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../" /I "../../../include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZCOMPAT_EXPORTS" /D "HAVE_CONFIG_H" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../out_debug\szcompat.lib"
# ADD LIB32 /nologo /out:"../../../lib/msvc/szcompat.lib"

!ENDIF 

# Begin Target

# Name "zcompat - Win32 Release"
# Name "zcompat - Win32 Debug"
# Name "zcompat - Win32 Debug Static"
# Name "zcompat - Win32 Release Static"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ZPIO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\ansi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\binio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\bsdsock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\chain.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\chconv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\dir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\dstream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\error.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\filter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\getcon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\membuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\open.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pipe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\posix.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\scipher.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\smtp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\stdio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\textio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\upipe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\vform.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\werror.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\wgetcon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\win32f.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\wpipe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\wstdio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\wutil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\wutil.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zopen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zpio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zpio_i.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zpio_m.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zprintf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zscanf.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "ZMOD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\dlx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\dlxgen.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\dlxload.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zmod.c
# End Source File
# End Group
# Begin Group "ZTOOLS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\mdmfdbg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbtree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sbuffer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\slist.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\smap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sstack.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sstr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ztools_i.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ztools_i.h
# End Source File
# End Group
# Begin Group "GNU Build system"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\configure.in
# End Source File
# Begin Source File

SOURCE=..\..\..\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Makefile.in
# End Source File
# End Group
# Begin Group "ZMM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\zmm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zmmchunk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zmminc.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\heap.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\heap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zcompat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zcompat.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\wsockerr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zansic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zcompat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zlinkdll.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zmm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zmod.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zpio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zpiocpp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zprintf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\zsystype.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\zcompat\ztools.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Misc Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\coff.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\convtabl.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\filter.template.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\win32err.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\zmodmake.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=..\config.h
# End Source File
# End Target
# End Project

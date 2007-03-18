# Microsoft Developer Studio Project File - Name="zcompat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=zcompat - Win32 BUILD_debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zcompat_zcompat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zcompat_zcompat.mak" CFG="zcompat - Win32 BUILD_debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zcompat - Win32 BUILD_release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zcompat - Win32 BUILD_debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zcompat - Win32 BUILD_release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release\zcompat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release\zcompat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /GR /GX /O1 /I "build\msvc" /W1 /FdRelease\zcompat.pdb /I ".\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "ZCOMPAT_MAKINGDLL" /c
# ADD CPP /nologo /FD /MD /GR /GX /O1 /I "build\msvc" /W1 /FdRelease\zcompat.pdb /I ".\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "ZCOMPAT_MAKINGDLL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "ZCOMPAT_MAKINGDLL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "ZCOMPAT_MAKINGDLL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i "build\msvc" /i ".\include" /d ZCOMPAT_MAKINGDLL
# ADD RSC /l 0x405 /i "build\msvc" /i ".\include" /d ZCOMPAT_MAKINGDLL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /dll /machine:i386 /out:"Release\zcompat.dll" /implib:"Release\zcompat.lib"
# ADD LINK32 /nologo /dll /machine:i386 /out:"Release\zcompat.dll" /implib:"Release\zcompat.lib"

!ELSEIF  "$(CFG)" == "zcompat - Win32 BUILD_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug\zcompat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\zcompat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /GR /GX /Od /I "build\msvc" /W4 /Zi /Gm /GZ /FdDebug\zcompat.pdb /I ".\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "ZCOMPAT_MAKINGDLL" /c
# ADD CPP /nologo /FD /MDd /GR /GX /Od /I "build\msvc" /W4 /Zi /Gm /GZ /FdDebug\zcompat.pdb /I ".\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "ZCOMPAT_MAKINGDLL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "ZCOMPAT_MAKINGDLL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "ZCOMPAT_MAKINGDLL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i "build\msvc" /d "_DEBUG" /i ".\include" /d ZCOMPAT_MAKINGDLL
# ADD RSC /l 0x405 /i "build\msvc" /d "_DEBUG" /i ".\include" /d ZCOMPAT_MAKINGDLL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /dll /machine:i386 /out:"Debug\zcompat.dll" /implib:"Debug\zcompat.lib" /debug
# ADD LINK32 /nologo /dll /machine:i386 /out:"Debug\zcompat.dll" /implib:"Debug\zcompat.lib" /debug

!ENDIF

# Begin Target

# Name "zcompat - Win32 BUILD_release"
# Name "zcompat - Win32 BUILD_debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\ansi.c
# End Source File
# Begin Source File

SOURCE=.\src\binio.c
# End Source File
# Begin Source File

SOURCE=.\src\bsdsock.c
# End Source File
# Begin Source File

SOURCE=.\src\chain.c
# End Source File
# Begin Source File

SOURCE=.\src\chconv.c
# End Source File
# Begin Source File

SOURCE=.\src\dir.c
# End Source File
# Begin Source File

SOURCE=.\src\dstream.c
# End Source File
# Begin Source File

SOURCE=.\src\error.c
# End Source File
# Begin Source File

SOURCE=.\src\filter.c
# End Source File
# Begin Source File

SOURCE=.\src\getcon.c
# End Source File
# Begin Source File

SOURCE=.\src\mdmfdbg.c
# End Source File
# Begin Source File

SOURCE=.\src\membuf.c
# End Source File
# Begin Source File

SOURCE=.\src\mfifo.c
# End Source File
# Begin Source File

SOURCE=.\src\netio.c
# End Source File
# Begin Source File

SOURCE=.\src\open.c
# End Source File
# Begin Source File

SOURCE=.\src\pipe.c
# End Source File
# Begin Source File

SOURCE=.\src\posix.c
# End Source File
# Begin Source File

SOURCE=.\src\sbtree.c
# End Source File
# Begin Source File

SOURCE=.\src\sbuffer.c
# End Source File
# Begin Source File

SOURCE=.\src\scipher.c
# End Source File
# Begin Source File

SOURCE=.\src\slist.c
# End Source File
# Begin Source File

SOURCE=.\src\smap.c
# End Source File
# Begin Source File

SOURCE=.\src\smtp.c
# End Source File
# Begin Source File

SOURCE=.\src\sstack.c
# End Source File
# Begin Source File

SOURCE=.\src\sstr.c
# End Source File
# Begin Source File

SOURCE=.\src\stdio.c
# End Source File
# Begin Source File

SOURCE=.\src\str.c
# End Source File
# Begin Source File

SOURCE=.\src\textio.c
# End Source File
# Begin Source File

SOURCE=.\src\upipe.c
# End Source File
# Begin Source File

SOURCE=.\src\utils.c
# End Source File
# Begin Source File

SOURCE=.\src\vform.c
# End Source File
# Begin Source File

SOURCE=.\src\werror.c
# End Source File
# Begin Source File

SOURCE=.\src\wgetcon.c
# End Source File
# Begin Source File

SOURCE=.\src\win32f.c
# End Source File
# Begin Source File

SOURCE=.\src\wpipe.c
# End Source File
# Begin Source File

SOURCE=.\src\wstdio.c
# End Source File
# Begin Source File

SOURCE=.\src\wutil.c
# End Source File
# Begin Source File

SOURCE=.\src\zcompat.c
# End Source File
# Begin Source File

SOURCE=.\src\zmm.c
# End Source File
# Begin Source File

SOURCE=.\src\zmmchunk.c
# End Source File
# Begin Source File

SOURCE=.\src\zmminc.c
# End Source File
# Begin Source File

SOURCE=.\src\zmod.c
# End Source File
# Begin Source File

SOURCE=.\src\zopen.c
# End Source File
# Begin Source File

SOURCE=.\src\zopt.c
# End Source File
# Begin Source File

SOURCE=.\src\zpio.c
# End Source File
# Begin Source File

SOURCE=.\src\zpio_m.c
# End Source File
# Begin Source File

SOURCE=.\src\zprintf.c
# End Source File
# Begin Source File

SOURCE=.\src\zscanf.c
# End Source File
# Begin Source File

SOURCE=.\src\zthread.c
# End Source File
# Begin Source File

SOURCE=.\src\ztools_i.c
# End Source File
# End Group
# End Target
# End Project


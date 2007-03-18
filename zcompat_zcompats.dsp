# Microsoft Developer Studio Project File - Name="zcompats" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=zcompats - Win32 BUILD_debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zcompat_zcompats.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zcompat_zcompats.mak" CFG="zcompats - Win32 BUILD_debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zcompats - Win32 BUILD_release" (based on "Win32 (x86) Static Library")
!MESSAGE "zcompats - Win32 BUILD_debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zcompats - Win32 BUILD_release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release\zcompats"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release\zcompats"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /GR /GX /O1 /I "build\msvc" /W1 /FdRelease\zcompats.pdb /I ".\include" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /MD /GR /GX /O1 /I "build\msvc" /W1 /FdRelease\zcompats.pdb /I ".\include" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Release\zcompats.lib"
# ADD LIB32 /nologo /out:"Release\zcompats.lib"

!ELSEIF  "$(CFG)" == "zcompats - Win32 BUILD_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug\zcompats"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\zcompats"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /GR /GX /Od /I "build\msvc" /W4 /Zi /Gm /GZ /FdDebug\zcompats.pdb /I ".\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /GR /GX /Od /I "build\msvc" /W4 /Zi /Gm /GZ /FdDebug\zcompats.pdb /I ".\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\zcompats.lib"
# ADD LIB32 /nologo /out:"Debug\zcompats.lib"

!ENDIF

# Begin Target

# Name "zcompats - Win32 BUILD_release"
# Name "zcompats - Win32 BUILD_debug"
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


# Microsoft Developer Studio Project File - Name="dist" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=dist - Win32 Release Static
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dist.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dist.mak" CFG="dist - Win32 Release Static"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dist - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "dist - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "dist - Win32 Debug Static" (based on "Win32 (x86) External Target")
!MESSAGE "dist - Win32 Release Static" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "dist"
# PROP Scc_LocalPath ".."

!IF  "$(CFG)" == "dist - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f dist.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dist.exe"
# PROP BASE Bsc_Name "dist.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "dist.mak" LIB="zcompat.lib" DLL="zcompat.dll" DIST=dll-release"
# PROP Rebuild_Opt "/a"
# PROP Target_File "dist/readme.txt"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "dist - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f dist.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dist.exe"
# PROP BASE Bsc_Name "dist.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "NMAKE /f dist.mak LIB="zcompatd.lib" DLL="zcompatd.dll" DIST=dll-debug"
# PROP Rebuild_Opt "/a"
# PROP Target_File "dist/readme.txt"
# PROP Bsc_Name "dist.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "dist - Win32 Debug Static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug Static"
# PROP BASE Intermediate_Dir "Debug Static"
# PROP BASE Cmd_Line "NMAKE /f dist.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dist.exe"
# PROP BASE Bsc_Name "dist.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug Static"
# PROP Intermediate_Dir "Debug Static"
# PROP Cmd_Line "NMAKE /f dist.mak LIB="szcompatd.lib" DIST="static-debug""
# PROP Rebuild_Opt "/a"
# PROP Target_File "dist/readme.txt"
# PROP Bsc_Name "dist.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "dist - Win32 Release Static"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release Static"
# PROP BASE Intermediate_Dir "Release Static"
# PROP BASE Cmd_Line "NMAKE /f dist.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "dist.exe"
# PROP BASE Bsc_Name "dist.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release Static"
# PROP Intermediate_Dir "Release Static"
# PROP Cmd_Line "NMAKE /f dist.mak LIB="szcompat.lib" DIST="static-release""
# PROP Rebuild_Opt "/a"
# PROP Target_File "dist/readme.txt"
# PROP Bsc_Name "dist.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "dist - Win32 Release"
# Name "dist - Win32 Debug"
# Name "dist - Win32 Debug Static"
# Name "dist - Win32 Release Static"

!IF  "$(CFG)" == "dist - Win32 Release"

!ELSEIF  "$(CFG)" == "dist - Win32 Debug"

!ELSEIF  "$(CFG)" == "dist - Win32 Debug Static"

!ELSEIF  "$(CFG)" == "dist - Win32 Release Static"

!ENDIF 

# Begin Source File

SOURCE=.\dist.mak
# End Source File
# End Target
# End Project

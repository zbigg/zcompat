$Id: readme.txt,v 1.1 2003/05/07 14:11:36 trurl Exp $

The README file for M$ Visual Studio (R)(TM) (as thay want ;-P)
build.

If you want to fast build zcompat using MS Visual C++ IDE (R)(TM) simply
open zcompat.dsw workspace in IDE.

- Select the dist project and Release configuration.

- Edit InstallBin.mak (seen in Workspace Window) and change
  installation directory as described.

- Now you can build the AXL by selecing "Build" command on current
  (dist) project.

The projects are:
	zcompat/libxl.dsp
    	        lib/msvc/*.lib|dll
	
It creates directory called dist. Which
includes:
- dll (optional)
- libs
- header files
- readme files and copyright information


# Microsoft Developer Studio Project File - Name="lincity" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=lincity - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lincity.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lincity.mak" CFG="lincity - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lincity - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "lincity - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lincity - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "gettext-w32" /I "libpng-1.2.5" /I "zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gnu_gettext.lib libpng.lib zdll.lib /nologo /subsystem:windows /machine:I386 /libpath:"zlib" /libpath:"libpng-1.2.5" /libpath:"gettext-w32"

!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "gettext-w32" /I "libpng-1.2.5" /I "zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gnu_gettext.lib libpng.lib zdll.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"zlib" /libpath:"libpng-1.2.5" /libpath:"gettext-w32"

!ENDIF 

# Begin Target

# Name "lincity - Win32 Release"
# Name "lincity - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\modules\blacksmith.c
# End Source File
# Begin Source File

SOURCE=.\cliglobs.c
# End Source File
# Begin Source File

SOURCE=.\modules\coal_power.c
# End Source File
# Begin Source File

SOURCE=.\modules\coalmine.c
# End Source File
# Begin Source File

SOURCE=.\modules\commune.c
# End Source File
# Begin Source File

SOURCE=.\modules\cricket.c
# End Source File
# Begin Source File

SOURCE=.\dialbox.c
# End Source File
# Begin Source File

SOURCE=.\engine.c
# End Source File
# Begin Source File

SOURCE=.\fileutil.c
# End Source File
# Begin Source File

SOURCE=.\modules\fire.c
# End Source File
# Begin Source File

SOURCE=.\modules\firestation.c
# End Source File
# Begin Source File

SOURCE=.\geometry.c
# End Source File
# Begin Source File

SOURCE=.\modules\health_centre.c
# End Source File
# Begin Source File

SOURCE=.\modules\heavy_industry.c
# End Source File
# Begin Source File

SOURCE=.\lchelp.c
# End Source File
# Begin Source File

SOURCE=.\lclib.c
# End Source File
# Begin Source File

SOURCE=.\lcwin32.c
# End Source File
# Begin Source File

SOURCE=.\ldsvgui.c
# End Source File
# Begin Source File

SOURCE=.\ldsvguts.c
# End Source File
# Begin Source File

SOURCE=.\modules\light_industry.c
# End Source File
# Begin Source File

SOURCE=.\lincity.rc
# End Source File
# Begin Source File

SOURCE=.\lintypes.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\market.c
# End Source File
# Begin Source File

SOURCE=.\modules\mill.c
# End Source File
# Begin Source File

SOURCE=.\module_buttons.c
# End Source File
# Begin Source File

SOURCE=.\modules\monument.c
# End Source File
# Begin Source File

SOURCE=.\mouse.c
# End Source File
# Begin Source File

SOURCE=.\mps.c
# End Source File
# Begin Source File

SOURCE=.\modules\oremine.c
# End Source File
# Begin Source File

SOURCE=.\modules\organic_farm.c
# End Source File
# Begin Source File

SOURCE=.\modules\parkland.c
# End Source File
# Begin Source File

SOURCE=.\pbar.c
# End Source File
# Begin Source File

SOURCE=.\pixmap.c
# End Source File
# Begin Source File

SOURCE=.\modules\port.c
# End Source File
# Begin Source File

SOURCE=.\modules\pottery.c
# End Source File
# Begin Source File

SOURCE=.\power.c
# End Source File
# Begin Source File

SOURCE=.\modules\power_line.c
# End Source File
# Begin Source File

SOURCE=.\modules\rail.c
# End Source File
# Begin Source File

SOURCE=.\readpng.c
# End Source File
# Begin Source File

SOURCE=.\modules\recycle.c
# End Source File
# Begin Source File

SOURCE=.\modules\residence.c
# End Source File
# Begin Source File

SOURCE=.\modules\road.c
# End Source File
# Begin Source File

SOURCE=.\modules\rocket_pad.c
# End Source File
# Begin Source File

SOURCE=.\modules\school.c
# End Source File
# Begin Source File

SOURCE=.\screen.c
# End Source File
# Begin Source File

SOURCE=.\modules\shanty.c
# End Source File
# Begin Source File

SOURCE=.\shrglobs.c
# End Source File
# Begin Source File

SOURCE=.\shrtypes.c
# End Source File
# Begin Source File

SOURCE=.\simulate.c
# End Source File
# Begin Source File

SOURCE=.\modules\solar_power.c
# End Source File
# Begin Source File

SOURCE=.\splash.c
# End Source File
# Begin Source File

SOURCE=.\stats.c
# End Source File
# Begin Source File

SOURCE=.\modules\substation.c
# End Source File
# Begin Source File

SOURCE=.\timer.c
# End Source File
# Begin Source File

SOURCE=.\modules\tip.c
# End Source File
# Begin Source File

SOURCE=.\modules\track.c
# End Source File
# Begin Source File

SOURCE=.\transport.c
# End Source File
# Begin Source File

SOURCE=.\modules\university.c
# End Source File
# Begin Source File

SOURCE=.\modules\water.c
# End Source File
# Begin Source File

SOURCE=.\modules\windmill.c
# End Source File
# Begin Source File

SOURCE=.\winmain.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\modules\modules.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

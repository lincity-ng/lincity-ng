# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=lcserver - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to lcserver - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "lc12 - Win32 Release" && "$(CFG)" != "lc12 - Win32 Debug" &&\
 "$(CFG)" != "lincity - Win32 Release" && "$(CFG)" != "lincity - Win32 Debug" &&\
 "$(CFG)" != "lcserver - Win32 Release" && "$(CFG)" != "lcserver - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "lc12.mak" CFG="lcserver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lc12 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "lc12 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "lincity - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "lincity - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "lcserver - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "lcserver - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "lincity - Win32 Debug"

!IF  "$(CFG)" == "lc12 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/lc12.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lc12.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib wsock32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib wsock32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/lc12.pdb" /machine:I386 /out:"$(OUTDIR)/lc12.exe" 
LINK32_OBJS=

!ELSEIF  "$(CFG)" == "lc12 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/lc12.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lc12.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib wsock32.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/lc12.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/lc12.exe" 
LINK32_OBJS=

!ELSEIF  "$(CFG)" == "lincity - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lincity\Release"
# PROP BASE Intermediate_Dir "lincity\Release"
# PROP BASE Target_Dir "lincity"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "lincity\Release"
# PROP Intermediate_Dir "lincity\Release"
# PROP Target_Dir "lincity"
OUTDIR=.\lincity\Release
INTDIR=.\lincity\Release

ALL : "$(OUTDIR)\lincity.exe"

CLEAN : 
	-@erase ".\lincity\Release\lincity.exe"
	-@erase ".\lincity\Release\Mps.obj"
	-@erase ".\lincity\Release\Lintypes.obj"
	-@erase ".\lincity\Release\Clinet.obj"
	-@erase ".\lincity\Release\Animate.obj"
	-@erase ".\lincity\Release\Mouse.obj"
	-@erase ".\lincity\Release\Splash.obj"
	-@erase ".\lincity\Release\Screen.obj"
	-@erase ".\lincity\Release\Climsg.obj"
	-@erase ".\lincity\Release\Main.obj"
	-@erase ".\lincity\Release\Clitypes.obj"
	-@erase ".\lincity\Release\transport.obj"
	-@erase ".\lincity\Release\Timer.obj"
	-@erase ".\lincity\Release\Lcwin32.obj"
	-@erase ".\lincity\Release\Geometry.obj"
	-@erase ".\lincity\Release\Market.obj"
	-@erase ".\lincity\Release\Ldsvgui.obj"
	-@erase ".\lincity\Release\Fileutil.obj"
	-@erase ".\lincity\Release\Shrnet.obj"
	-@erase ".\lincity\Release\Lchelp.obj"
	-@erase ".\lincity\Release\Simulate.obj"
	-@erase ".\lincity\Release\Shrtypes.obj"
	-@erase ".\lincity\Release\Winmain.obj"
	-@erase ".\lincity\Release\Cliglobs.obj"
	-@erase ".\lincity\Release\Shrglobs.obj"
	-@erase ".\lincity\Release\Ldsvguts.obj"
	-@erase ".\lincity\Release\Clistubs.obj"
	-@erase ".\lincity\Release\Engine.obj"
	-@erase ".\lincity\Release\Lincity.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/lincity.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\lincity\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Lincity.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lincity.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib wsock32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib wsock32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/lincity.pdb" /machine:I386 /out:"$(OUTDIR)/lincity.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Mps.obj" \
	"$(INTDIR)/Lintypes.obj" \
	"$(INTDIR)/Clinet.obj" \
	"$(INTDIR)/Animate.obj" \
	"$(INTDIR)/Mouse.obj" \
	"$(INTDIR)/Splash.obj" \
	"$(INTDIR)/Screen.obj" \
	"$(INTDIR)/Climsg.obj" \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Clitypes.obj" \
	"$(INTDIR)/transport.obj" \
	"$(INTDIR)/Timer.obj" \
	"$(INTDIR)/Lcwin32.obj" \
	"$(INTDIR)/Geometry.obj" \
	"$(INTDIR)/Market.obj" \
	"$(INTDIR)/Ldsvgui.obj" \
	"$(INTDIR)/Fileutil.obj" \
	"$(INTDIR)/Shrnet.obj" \
	"$(INTDIR)/Lchelp.obj" \
	"$(INTDIR)/Simulate.obj" \
	"$(INTDIR)/Shrtypes.obj" \
	"$(INTDIR)/Winmain.obj" \
	"$(INTDIR)/Cliglobs.obj" \
	"$(INTDIR)/Shrglobs.obj" \
	"$(INTDIR)/Ldsvguts.obj" \
	"$(INTDIR)/Clistubs.obj" \
	"$(INTDIR)/Engine.obj" \
	"$(INTDIR)/Lincity.res"

"$(OUTDIR)\lincity.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lincity\Debug"
# PROP BASE Intermediate_Dir "lincity\Debug"
# PROP BASE Target_Dir "lincity"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "lincity\Debug"
# PROP Intermediate_Dir "lincity\Debug"
# PROP Target_Dir "lincity"
OUTDIR=.\lincity\Debug
INTDIR=.\lincity\Debug

ALL : "$(OUTDIR)\lincity.exe"

CLEAN : 
	-@erase ".\lincity\Debug\vc40.pdb"
	-@erase ".\lincity\Debug\vc40.idb"
	-@erase ".\lincity\Debug\lincity.exe"
	-@erase ".\lincity\Debug\Shrglobs.obj"
	-@erase ".\lincity\Debug\Lchelp.obj"
	-@erase ".\lincity\Debug\Timer.obj"
	-@erase ".\lincity\Debug\Main.obj"
	-@erase ".\lincity\Debug\Lintypes.obj"
	-@erase ".\lincity\Debug\Lcwin32.obj"
	-@erase ".\lincity\Debug\Ldsvgui.obj"
	-@erase ".\lincity\Debug\Fileutil.obj"
	-@erase ".\lincity\Debug\Simulate.obj"
	-@erase ".\lincity\Debug\Shrnet.obj"
	-@erase ".\lincity\Debug\Clitypes.obj"
	-@erase ".\lincity\Debug\Winmain.obj"
	-@erase ".\lincity\Debug\Cliglobs.obj"
	-@erase ".\lincity\Debug\Ldsvguts.obj"
	-@erase ".\lincity\Debug\Clistubs.obj"
	-@erase ".\lincity\Debug\Geometry.obj"
	-@erase ".\lincity\Debug\Engine.obj"
	-@erase ".\lincity\Debug\Mouse.obj"
	-@erase ".\lincity\Debug\Clinet.obj"
	-@erase ".\lincity\Debug\Animate.obj"
	-@erase ".\lincity\Debug\transport.obj"
	-@erase ".\lincity\Debug\Market.obj"
	-@erase ".\lincity\Debug\Mps.obj"
	-@erase ".\lincity\Debug\Shrtypes.obj"
	-@erase ".\lincity\Debug\Splash.obj"
	-@erase ".\lincity\Debug\Screen.obj"
	-@erase ".\lincity\Debug\Climsg.obj"
	-@erase ".\lincity\Debug\Lincity.res"
	-@erase ".\lincity\Debug\lincity.ilk"
	-@erase ".\lincity\Debug\lincity.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/lincity.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\lincity\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Lincity.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lincity.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib wsock32.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/lincity.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/lincity.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Shrglobs.obj" \
	"$(INTDIR)/Lchelp.obj" \
	"$(INTDIR)/Timer.obj" \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Lintypes.obj" \
	"$(INTDIR)/Lcwin32.obj" \
	"$(INTDIR)/Ldsvgui.obj" \
	"$(INTDIR)/Fileutil.obj" \
	"$(INTDIR)/Simulate.obj" \
	"$(INTDIR)/Shrnet.obj" \
	"$(INTDIR)/Clitypes.obj" \
	"$(INTDIR)/Winmain.obj" \
	"$(INTDIR)/Cliglobs.obj" \
	"$(INTDIR)/Ldsvguts.obj" \
	"$(INTDIR)/Clistubs.obj" \
	"$(INTDIR)/Geometry.obj" \
	"$(INTDIR)/Engine.obj" \
	"$(INTDIR)/Mouse.obj" \
	"$(INTDIR)/Clinet.obj" \
	"$(INTDIR)/Animate.obj" \
	"$(INTDIR)/transport.obj" \
	"$(INTDIR)/Market.obj" \
	"$(INTDIR)/Mps.obj" \
	"$(INTDIR)/Shrtypes.obj" \
	"$(INTDIR)/Splash.obj" \
	"$(INTDIR)/Screen.obj" \
	"$(INTDIR)/Climsg.obj" \
	"$(INTDIR)/Lincity.res"

"$(OUTDIR)\lincity.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lcserver - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lcserver\Release"
# PROP BASE Intermediate_Dir "lcserver\Release"
# PROP BASE Target_Dir "lcserver"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "lcserver\Release"
# PROP Intermediate_Dir "lcserver\Release"
# PROP Target_Dir "lcserver"
OUTDIR=.\lcserver\Release
INTDIR=.\lcserver\Release

ALL : "$(OUTDIR)\lcserver.exe"

CLEAN : 
	-@erase ".\lcserver\Release\lcserver.exe"
	-@erase ".\lcserver\Release\transport.obj"
	-@erase ".\lcserver\Release\Fileutil.obj"
	-@erase ".\lcserver\Release\Ldsvguts.obj"
	-@erase ".\lcserver\Release\Lintypes.obj"
	-@erase ".\lcserver\Release\Market.obj"
	-@erase ".\lcserver\Release\Sermain.obj"
	-@erase ".\lcserver\Release\Sermsg.obj"
	-@erase ".\lcserver\Release\Sernet.obj"
	-@erase ".\lcserver\Release\Serstubs.obj"
	-@erase ".\lcserver\Release\Shrglobs.obj"
	-@erase ".\lcserver\Release\Shrnet.obj"
	-@erase ".\lcserver\Release\Shrtypes.obj"
	-@erase ".\lcserver\Release\Simulate.obj"
	-@erase ".\lcserver\Release\Timer.obj"
	-@erase ".\lcserver\Release\Engine.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/lcserver.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\lcserver\Release/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lcserver.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/lcserver.pdb" /machine:I386 /out:"$(OUTDIR)/lcserver.exe" 
LINK32_OBJS= \
	"$(INTDIR)/transport.obj" \
	"$(INTDIR)/Fileutil.obj" \
	"$(INTDIR)/Ldsvguts.obj" \
	"$(INTDIR)/Lintypes.obj" \
	"$(INTDIR)/Market.obj" \
	"$(INTDIR)/Sermain.obj" \
	"$(INTDIR)/Sermsg.obj" \
	"$(INTDIR)/Sernet.obj" \
	"$(INTDIR)/Serstubs.obj" \
	"$(INTDIR)/Shrglobs.obj" \
	"$(INTDIR)/Shrnet.obj" \
	"$(INTDIR)/Shrtypes.obj" \
	"$(INTDIR)/Simulate.obj" \
	"$(INTDIR)/Timer.obj" \
	"$(INTDIR)/Engine.obj"

"$(OUTDIR)\lcserver.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lcserver - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lcserver\Debug"
# PROP BASE Intermediate_Dir "lcserver\Debug"
# PROP BASE Target_Dir "lcserver"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "lcserver\Debug"
# PROP Intermediate_Dir "lcserver\Debug"
# PROP Target_Dir "lcserver"
OUTDIR=.\lcserver\Debug
INTDIR=.\lcserver\Debug

ALL : "$(OUTDIR)\lcserver.exe"

CLEAN : 
	-@erase ".\lcserver\Debug\lcserver.exe"
	-@erase ".\lcserver\Debug\transport.obj"
	-@erase ".\lcserver\Debug\Fileutil.obj"
	-@erase ".\lcserver\Debug\Ldsvguts.obj"
	-@erase ".\lcserver\Debug\Lintypes.obj"
	-@erase ".\lcserver\Debug\Market.obj"
	-@erase ".\lcserver\Debug\Sermain.obj"
	-@erase ".\lcserver\Debug\Sermsg.obj"
	-@erase ".\lcserver\Debug\Sernet.obj"
	-@erase ".\lcserver\Debug\Serstubs.obj"
	-@erase ".\lcserver\Debug\Shrglobs.obj"
	-@erase ".\lcserver\Debug\Shrnet.obj"
	-@erase ".\lcserver\Debug\Shrtypes.obj"
	-@erase ".\lcserver\Debug\Simulate.obj"
	-@erase ".\lcserver\Debug\Timer.obj"
	-@erase ".\lcserver\Debug\Engine.obj"
	-@erase ".\lcserver\Debug\lcserver.ilk"
	-@erase ".\lcserver\Debug\lcserver.pdb"
	-@erase ".\lcserver\Debug\vc40.pdb"
	-@erase ".\lcserver\Debug\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/lcserver.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\lcserver\Debug/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lcserver.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/lcserver.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/lcserver.exe" 
LINK32_OBJS= \
	"$(INTDIR)/transport.obj" \
	"$(INTDIR)/Fileutil.obj" \
	"$(INTDIR)/Ldsvguts.obj" \
	"$(INTDIR)/Lintypes.obj" \
	"$(INTDIR)/Market.obj" \
	"$(INTDIR)/Sermain.obj" \
	"$(INTDIR)/Sermsg.obj" \
	"$(INTDIR)/Sernet.obj" \
	"$(INTDIR)/Serstubs.obj" \
	"$(INTDIR)/Shrglobs.obj" \
	"$(INTDIR)/Shrnet.obj" \
	"$(INTDIR)/Shrtypes.obj" \
	"$(INTDIR)/Simulate.obj" \
	"$(INTDIR)/Timer.obj" \
	"$(INTDIR)/Engine.obj"

"$(OUTDIR)\lcserver.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "lc12 - Win32 Release"
# Name "lc12 - Win32 Debug"

!IF  "$(CFG)" == "lc12 - Win32 Release"

!ELSEIF  "$(CFG)" == "lc12 - Win32 Debug"

!ENDIF 

# End Target
################################################################################
# Begin Target

# Name "lincity - Win32 Release"
# Name "lincity - Win32 Debug"

!IF  "$(CFG)" == "lincity - Win32 Release"

!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Animate.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_ANIMA=\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\lctypes.h"\
	".\clinet.h"\
	".\animate.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\config.h"\
	".\confw32.h"\
	

"$(INTDIR)\Animate.obj" : $(SOURCE) $(DEP_CPP_ANIMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_ANIMA=\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\lctypes.h"\
	".\clinet.h"\
	".\animate.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\config.h"\
	".\confw32.h"\
	

"$(INTDIR)\Animate.obj" : $(SOURCE) $(DEP_CPP_ANIMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Cliglobs.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_CLIGL=\
	".\cliglobs.h"\
	".\common.h"\
	".\geometry.h"\
	".\lin-city.h"\
	".\lintypes.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	

"$(INTDIR)\Cliglobs.obj" : $(SOURCE) $(DEP_CPP_CLIGL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_CLIGL=\
	".\cliglobs.h"\
	".\common.h"\
	".\geometry.h"\
	

"$(INTDIR)\Cliglobs.obj" : $(SOURCE) $(DEP_CPP_CLIGL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Climsg.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_CLIMS=\
	".\lcconfig.h"\
	".\lin-city.h"\
	".\lctypes.h"\
	".\engglobs.h"\
	".\cliglobs.h"\
	".\clinet.h"\
	".\shrnet.h"\
	".\config.h"\
	".\confw32.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\protocol.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

"$(INTDIR)\Climsg.obj" : $(SOURCE) $(DEP_CPP_CLIMS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_CLIMS=\
	".\lcconfig.h"\
	".\lin-city.h"\
	".\lctypes.h"\
	".\engglobs.h"\
	".\cliglobs.h"\
	".\clinet.h"\
	".\shrnet.h"\
	".\config.h"\
	".\confw32.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	".\protocol.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

"$(INTDIR)\Climsg.obj" : $(SOURCE) $(DEP_CPP_CLIMS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Clinet.c
DEP_CPP_CLINE=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\shrnet.h"\
	".\clinet.h"\
	".\climsg.h"\
	".\config.h"\
	".\confw32.h"\
	".\protocol.h"\
	".\lin-city.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Clinet.obj" : $(SOURCE) $(DEP_CPP_CLINE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Clistubs.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_CLIST=\
	".\lin-city.h"\
	".\engglobs.h"\
	".\lctypes.h"\
	".\clinet.h"\
	".\climsg.h"\
	".\simulate.h"\
	".\animate.h"\
	".\engine.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\config.h"\
	".\confw32.h"\
	

"$(INTDIR)\Clistubs.obj" : $(SOURCE) $(DEP_CPP_CLIST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_CLIST=\
	".\lin-city.h"\
	".\engglobs.h"\
	".\lctypes.h"\
	".\clinet.h"\
	".\climsg.h"\
	".\simulate.h"\
	".\animate.h"\
	".\engine.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\config.h"\
	".\confw32.h"\
	

"$(INTDIR)\Clistubs.obj" : $(SOURCE) $(DEP_CPP_CLIST) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Clitypes.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_CLITY=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\typeinit.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Clitypes.obj" : $(SOURCE) $(DEP_CPP_CLITY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_CLITY=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\typeinit.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Clitypes.obj" : $(SOURCE) $(DEP_CPP_CLITY) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_ENGIN=\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\engine.h"\
	".\engglobs.h"\
	".\cliglobs.h"\
	".\simulate.h"\
	".\sernet.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\config.h"\
	".\confw32.h"\
	

"$(INTDIR)\Engine.obj" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_ENGIN=\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\engine.h"\
	".\engglobs.h"\
	".\cliglobs.h"\
	".\simulate.h"\
	".\sernet.h"\
	".\lintypes.h"\
	".\geometry.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\config.h"\
	".\confw32.h"\
	

"$(INTDIR)\Engine.obj" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Fileutil.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_FILEU=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\fileutil.h"\
	".\config.h"\
	".\confw32.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Fileutil.obj" : $(SOURCE) $(DEP_CPP_FILEU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_FILEU=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\fileutil.h"\
	".\config.h"\
	".\confw32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Fileutil.obj" : $(SOURCE) $(DEP_CPP_FILEU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Geometry.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_GEOME=\
	".\config.h"\
	".\confw32.h"\
	".\lin-city.h"\
	".\common.h"\
	".\geometry.h"\
	".\cliglobs.h"\
	".\lintypes.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	

"$(INTDIR)\Geometry.obj" : $(SOURCE) $(DEP_CPP_GEOME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_GEOME=\
	".\config.h"\
	".\confw32.h"\
	".\lin-city.h"\
	".\common.h"\
	".\geometry.h"\
	".\cliglobs.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Geometry.obj" : $(SOURCE) $(DEP_CPP_GEOME) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Lchelp.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_LCHEL=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\help.h"\
	".\clistubs.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\cliglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Lchelp.obj" : $(SOURCE) $(DEP_CPP_LCHEL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_LCHEL=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\help.h"\
	".\clistubs.h"\
	".\lintypes.h"\
	".\cliglobs.h"\
	

"$(INTDIR)\Lchelp.obj" : $(SOURCE) $(DEP_CPP_LCHEL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Lcwin32.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_LCWIN=\
	".\lin-city.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lintypes.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	

"$(INTDIR)\Lcwin32.obj" : $(SOURCE) $(DEP_CPP_LCWIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_LCWIN=\
	".\lin-city.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Lcwin32.obj" : $(SOURCE) $(DEP_CPP_LCWIN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ldsvgui.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_LDSVG=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\protocol.h"\
	".\clinet.h"\
	".\ldsvguts.h"\
	".\fileutil.h"\
	".\config.h"\
	".\confw32.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	".\shrnet.h"\
	

"$(INTDIR)\Ldsvgui.obj" : $(SOURCE) $(DEP_CPP_LDSVG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_LDSVG=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\protocol.h"\
	".\clinet.h"\
	".\ldsvguts.h"\
	".\fileutil.h"\
	".\config.h"\
	".\confw32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	".\shrnet.h"\
	

"$(INTDIR)\Ldsvgui.obj" : $(SOURCE) $(DEP_CPP_LDSVG) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ldsvguts.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_LDSVGU=\
	".\config.h"\
	".\confw32.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\fileutil.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Ldsvguts.obj" : $(SOURCE) $(DEP_CPP_LDSVGU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_LDSVGU=\
	".\config.h"\
	".\confw32.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\fileutil.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Ldsvguts.obj" : $(SOURCE) $(DEP_CPP_LDSVGU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Lintypes.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_LINTY=\
	".\lin-city.h"\
	".\engglobs.h"\
	".\lctypes.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	

"$(INTDIR)\Lintypes.obj" : $(SOURCE) $(DEP_CPP_LINTY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_LINTY=\
	".\lin-city.h"\
	".\engglobs.h"\
	".\lctypes.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	

"$(INTDIR)\Lintypes.obj" : $(SOURCE) $(DEP_CPP_LINTY) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Main.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_MAIN_=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\animate.h"\
	".\timer.h"\
	".\clistubs.h"\
	".\ldsvgui.h"\
	".\simulate.h"\
	".\config.h"\
	".\confw32.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_MAIN_=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\animate.h"\
	".\timer.h"\
	".\clistubs.h"\
	".\ldsvgui.h"\
	".\simulate.h"\
	".\config.h"\
	".\confw32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Market.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_MARKE=\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\market.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\engglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Market.obj" : $(SOURCE) $(DEP_CPP_MARKE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_MARKE=\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\market.h"\
	".\lintypes.h"\
	".\engglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Market.obj" : $(SOURCE) $(DEP_CPP_MARKE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mouse.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_MOUSE=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\mouse.h"\
	".\engglobs.h"\
	".\clistubs.h"\
	".\engine.h"\
	".\screen.h"\
	".\climsg.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\cliglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Mouse.obj" : $(SOURCE) $(DEP_CPP_MOUSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_MOUSE=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\mouse.h"\
	".\engglobs.h"\
	".\clistubs.h"\
	".\engine.h"\
	".\screen.h"\
	".\climsg.h"\
	".\lintypes.h"\
	".\cliglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Mouse.obj" : $(SOURCE) $(DEP_CPP_MOUSE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mps.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_MPS_C=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\mps.h"\
	".\engglobs.h"\
	".\clistubs.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\cliglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Mps.obj" : $(SOURCE) $(DEP_CPP_MPS_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_MPS_C=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\mps.h"\
	".\engglobs.h"\
	".\clistubs.h"\
	".\lintypes.h"\
	".\cliglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Mps.obj" : $(SOURCE) $(DEP_CPP_MPS_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Screen.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_SCREE=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\screen.h"\
	".\engglobs.h"\
	".\clistubs.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\cliglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_SCREE=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\screen.h"\
	".\engglobs.h"\
	".\clistubs.h"\
	".\lintypes.h"\
	".\cliglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Shrglobs.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_SHRGL=\
	".\engglobs.h"\
	".\common.h"\
	".\lin-city.h"\
	".\geometry.h"\
	".\lintypes.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	

"$(INTDIR)\Shrglobs.obj" : $(SOURCE) $(DEP_CPP_SHRGL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_SHRGL=\
	".\engglobs.h"\
	".\common.h"\
	".\lin-city.h"\
	".\geometry.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Shrglobs.obj" : $(SOURCE) $(DEP_CPP_SHRGL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Shrnet.c
DEP_CPP_SHRNE=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\config.h"\
	".\confw32.h"\
	".\lin-city.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Shrnet.obj" : $(SOURCE) $(DEP_CPP_SHRNE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Shrtypes.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_SHRTY=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\typeinit.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Shrtypes.obj" : $(SOURCE) $(DEP_CPP_SHRTY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_SHRTY=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\typeinit.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Shrtypes.obj" : $(SOURCE) $(DEP_CPP_SHRTY) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Simulate.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_SIMUL=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\config.h"\
	".\confw32.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Simulate.obj" : $(SOURCE) $(DEP_CPP_SIMUL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_SIMUL=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\config.h"\
	".\confw32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Simulate.obj" : $(SOURCE) $(DEP_CPP_SIMUL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Splash.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_SPLAS=\
	".\config.h"\
	".\confw32.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Splash.obj" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_SPLAS=\
	".\config.h"\
	".\confw32.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Splash.obj" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Timer.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_TIMER=\
	".\config.h"\
	".\confw32.h"\
	".\cliglobs.h"\
	".\common.h"\
	".\lin-city.h"\
	".\geometry.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Timer.obj" : $(SOURCE) $(DEP_CPP_TIMER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_TIMER=\
	".\config.h"\
	".\confw32.h"\
	".\cliglobs.h"\
	".\common.h"\
	

"$(INTDIR)\Timer.obj" : $(SOURCE) $(DEP_CPP_TIMER) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\transport.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_TRANS=\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\transport.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\engglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\transport.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_TRANS=\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\transport.h"\
	".\lintypes.h"\
	".\engglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\transport.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Winmain.c

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_WINMA=\
	".\lin-city.h"\
	".\common.h"\
	".\generic.h"\
	".\mouse.h"\
	".\lintypes.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\cliglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Winmain.obj" : $(SOURCE) $(DEP_CPP_WINMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_WINMA=\
	".\lin-city.h"\
	".\common.h"\
	".\generic.h"\
	".\mouse.h"\
	".\lintypes.h"\
	".\cliglobs.h"\
	

"$(INTDIR)\Winmain.obj" : $(SOURCE) $(DEP_CPP_WINMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Lincity.rc
DEP_RSC_LINCI=\
	".\SMALL.ICO"\
	".\lincity.ico"\
	".\resource.h"\
	".\generic.h"\
	

"$(INTDIR)\Lincity.res" : $(SOURCE) $(DEP_RSC_LINCI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "lcserver - Win32 Release"
# Name "lcserver - Win32 Debug"

!IF  "$(CFG)" == "lcserver - Win32 Release"

!ELSEIF  "$(CFG)" == "lcserver - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\transport.c
DEP_CPP_TRANS=\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\transport.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\engglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\transport.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Fileutil.c
DEP_CPP_FILEU=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\fileutil.h"\
	".\config.h"\
	".\confw32.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Fileutil.obj" : $(SOURCE) $(DEP_CPP_FILEU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ldsvguts.c

!IF  "$(CFG)" == "lcserver - Win32 Release"

DEP_CPP_LDSVGU=\
	".\config.h"\
	".\confw32.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\fileutil.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Ldsvguts.obj" : $(SOURCE) $(DEP_CPP_LDSVGU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lcserver - Win32 Debug"

DEP_CPP_LDSVGU=\
	".\config.h"\
	".\confw32.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\fileutil.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Ldsvguts.obj" : $(SOURCE) $(DEP_CPP_LDSVGU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Lintypes.c
DEP_CPP_LINTY=\
	".\lin-city.h"\
	".\engglobs.h"\
	".\lctypes.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	

"$(INTDIR)\Lintypes.obj" : $(SOURCE) $(DEP_CPP_LINTY) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Market.c
DEP_CPP_MARKE=\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\market.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\engglobs.h"\
	".\geometry.h"\
	

"$(INTDIR)\Market.obj" : $(SOURCE) $(DEP_CPP_MARKE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Sermain.c

!IF  "$(CFG)" == "lcserver - Win32 Release"

DEP_CPP_SERMA=\
	".\config.h"\
	".\confw32.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\timer.h"\
	".\fileutil.h"\
	".\simulate.h"\
	".\shrnet.h"\
	".\sernet.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	".\protocol.h"\
	".\lcconfig.h"\
	

"$(INTDIR)\Sermain.obj" : $(SOURCE) $(DEP_CPP_SERMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lcserver - Win32 Debug"


"$(INTDIR)\Sermain.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Sermsg.c
DEP_CPP_SERMS=\
	".\lcconfig.h"\
	".\lin-city.h"\
	".\lctypes.h"\
	".\engglobs.h"\
	".\sernet.h"\
	".\shrnet.h"\
	".\simulate.h"\
	".\engine.h"\
	".\config.h"\
	".\confw32.h"\
	".\lintypes.h"\
	".\common.h"\
	".\geometry.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\protocol.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

"$(INTDIR)\Sermsg.obj" : $(SOURCE) $(DEP_CPP_SERMS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Sernet.c
DEP_CPP_SERNE=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\sernet.h"\
	".\shrnet.h"\
	".\config.h"\
	".\confw32.h"\
	".\protocol.h"\
	".\lin-city.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Sernet.obj" : $(SOURCE) $(DEP_CPP_SERNE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Serstubs.c
DEP_CPP_SERST=\
	".\clinet.h"\
	".\lctypes.h"\
	".\engine.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\lcconfig.h"\
	".\lin-city.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\config.h"\
	".\confw32.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Serstubs.obj" : $(SOURCE) $(DEP_CPP_SERST) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Shrglobs.c
DEP_CPP_SHRGL=\
	".\engglobs.h"\
	".\common.h"\
	".\lin-city.h"\
	".\geometry.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Shrglobs.obj" : $(SOURCE) $(DEP_CPP_SHRGL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Shrnet.c
DEP_CPP_SHRNE=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\config.h"\
	".\confw32.h"\
	".\lin-city.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Shrnet.obj" : $(SOURCE) $(DEP_CPP_SHRNE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Shrtypes.c
DEP_CPP_SHRTY=\
	".\config.h"\
	".\confw32.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\typeinit.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Shrtypes.obj" : $(SOURCE) $(DEP_CPP_SHRTY) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Simulate.c
DEP_CPP_SIMUL=\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\cliglobs.h"\
	".\engglobs.h"\
	".\config.h"\
	".\confw32.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	

"$(INTDIR)\Simulate.obj" : $(SOURCE) $(DEP_CPP_SIMUL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Timer.c
DEP_CPP_TIMER=\
	".\config.h"\
	".\confw32.h"\
	".\cliglobs.h"\
	".\common.h"\
	".\lin-city.h"\
	".\geometry.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	

"$(INTDIR)\Timer.obj" : $(SOURCE) $(DEP_CPP_TIMER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine.c
DEP_CPP_ENGIN=\
	".\common.h"\
	".\lctypes.h"\
	".\lin-city.h"\
	".\engine.h"\
	".\engglobs.h"\
	".\cliglobs.h"\
	".\simulate.h"\
	".\sernet.h"\
	".\lcx11.h"\
	".\lcwin32.h"\
	".\lintypes.h"\
	".\geometry.h"\
	".\shrnet.h"\
	".\protocol.h"\
	".\lcconfig.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\config.h"\
	".\confw32.h"\
	

"$(INTDIR)\Engine.obj" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################

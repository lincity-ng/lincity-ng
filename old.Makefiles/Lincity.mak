# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=lincity - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to lincity - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "lincity - Win32 Release" && "$(CFG)" !=\
 "lincity - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lincity.mak" CFG="lincity - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lincity - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "lincity - Win32 Debug" (based on "Win32 (x86) Application")
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
MTL=mktyplib.exe
CPP=cl.exe
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
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Lincity.exe" "$(OUTDIR)\Lincity.bsc"

CLEAN : 
	-@erase ".\Release\Lincity.bsc"
	-@erase ".\Release\Help.sbr"
	-@erase ".\Release\Main.sbr"
	-@erase ".\Release\Mps.sbr"
	-@erase ".\Release\Market.sbr"
	-@erase ".\Release\Screen.sbr"
	-@erase ".\Release\transport.sbr"
	-@erase ".\Release\Typeinit.sbr"
	-@erase ".\Release\Lcwin32.sbr"
	-@erase ".\Release\Mouse.sbr"
	-@erase ".\Release\winmain.sbr"
	-@erase ".\Release\Engine.sbr"
	-@erase ".\Release\Lincity.exe"
	-@erase ".\Release\Mouse.obj"
	-@erase ".\Release\winmain.obj"
	-@erase ".\Release\Engine.obj"
	-@erase ".\Release\Help.obj"
	-@erase ".\Release\Main.obj"
	-@erase ".\Release\Mps.obj"
	-@erase ".\Release\Market.obj"
	-@erase ".\Release\Screen.obj"
	-@erase ".\Release\transport.obj"
	-@erase ".\Release\Typeinit.obj"
	-@erase ".\Release\Lcwin32.obj"
	-@erase ".\Release\lincity.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/Lincity.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/lincity.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Lincity.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Help.sbr" \
	"$(INTDIR)/Main.sbr" \
	"$(INTDIR)/Mps.sbr" \
	"$(INTDIR)/Market.sbr" \
	"$(INTDIR)/Screen.sbr" \
	"$(INTDIR)/transport.sbr" \
	"$(INTDIR)/Typeinit.sbr" \
	"$(INTDIR)/Lcwin32.sbr" \
	"$(INTDIR)/Mouse.sbr" \
	"$(INTDIR)/winmain.sbr" \
	"$(INTDIR)/Engine.sbr"

"$(OUTDIR)\Lincity.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /profile /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /profile /machine:I386\
 /out:"$(OUTDIR)/Lincity.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Mouse.obj" \
	"$(INTDIR)/winmain.obj" \
	"$(INTDIR)/Engine.obj" \
	"$(INTDIR)/Help.obj" \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Mps.obj" \
	"$(INTDIR)/Market.obj" \
	"$(INTDIR)/Screen.obj" \
	"$(INTDIR)/transport.obj" \
	"$(INTDIR)/Typeinit.obj" \
	"$(INTDIR)/Lcwin32.obj" \
	"$(INTDIR)/lincity.res"

"$(OUTDIR)\Lincity.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

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
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Lincity.exe" "$(OUTDIR)\Lincity.bsc"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Lincity.bsc"
	-@erase ".\Debug\Screen.sbr"
	-@erase ".\Debug\Help.sbr"
	-@erase ".\Debug\Typeinit.sbr"
	-@erase ".\Debug\Mps.sbr"
	-@erase ".\Debug\winmain.sbr"
	-@erase ".\Debug\Engine.sbr"
	-@erase ".\Debug\Mouse.sbr"
	-@erase ".\Debug\Main.sbr"
	-@erase ".\Debug\Lcwin32.sbr"
	-@erase ".\Debug\transport.sbr"
	-@erase ".\Debug\Market.sbr"
	-@erase ".\Debug\Lincity.exe"
	-@erase ".\Debug\Mouse.obj"
	-@erase ".\Debug\Main.obj"
	-@erase ".\Debug\Lcwin32.obj"
	-@erase ".\Debug\transport.obj"
	-@erase ".\Debug\Market.obj"
	-@erase ".\Debug\Screen.obj"
	-@erase ".\Debug\Help.obj"
	-@erase ".\Debug\Typeinit.obj"
	-@erase ".\Debug\Mps.obj"
	-@erase ".\Debug\winmain.obj"
	-@erase ".\Debug\Engine.obj"
	-@erase ".\Debug\lincity.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/Lincity.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/lincity.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Lincity.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/Screen.sbr" \
	"$(INTDIR)/Help.sbr" \
	"$(INTDIR)/Typeinit.sbr" \
	"$(INTDIR)/Mps.sbr" \
	"$(INTDIR)/winmain.sbr" \
	"$(INTDIR)/Engine.sbr" \
	"$(INTDIR)/Mouse.sbr" \
	"$(INTDIR)/Main.sbr" \
	"$(INTDIR)/Lcwin32.sbr" \
	"$(INTDIR)/transport.sbr" \
	"$(INTDIR)/Market.sbr"

"$(OUTDIR)\Lincity.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /profile /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib version.lib /nologo /subsystem:windows /profile /debug\
 /machine:I386 /out:"$(OUTDIR)/Lincity.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Mouse.obj" \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Lcwin32.obj" \
	"$(INTDIR)/transport.obj" \
	"$(INTDIR)/Market.obj" \
	"$(INTDIR)/Screen.obj" \
	"$(INTDIR)/Help.obj" \
	"$(INTDIR)/Typeinit.obj" \
	"$(INTDIR)/Mps.obj" \
	"$(INTDIR)/winmain.obj" \
	"$(INTDIR)/Engine.obj" \
	"$(INTDIR)/lincity.res"

"$(OUTDIR)\Lincity.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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

################################################################################
# Begin Target

# Name "lincity - Win32 Release"
# Name "lincity - Win32 Debug"

!IF  "$(CFG)" == "lincity - Win32 Release"

!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Help.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_HELP_=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Help.h"\
	

"$(INTDIR)\Help.obj" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"

"$(INTDIR)\Help.sbr" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_HELP_=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Help.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\Help.obj" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"

"$(INTDIR)\Help.sbr" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Main.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_MAIN_=\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Main.h"\
	

"$(INTDIR)\Main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"

"$(INTDIR)\Main.sbr" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_MAIN_=\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Main.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\Main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"

"$(INTDIR)\Main.sbr" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Market.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_MARKE=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Market.h"\
	

"$(INTDIR)\Market.obj" : $(SOURCE) $(DEP_CPP_MARKE) "$(INTDIR)"

"$(INTDIR)\Market.sbr" : $(SOURCE) $(DEP_CPP_MARKE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_MARKE=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Market.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\Market.obj" : $(SOURCE) $(DEP_CPP_MARKE) "$(INTDIR)"

"$(INTDIR)\Market.sbr" : $(SOURCE) $(DEP_CPP_MARKE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mouse.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_MOUSE=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Mouse.h"\
	

"$(INTDIR)\Mouse.obj" : $(SOURCE) $(DEP_CPP_MOUSE) "$(INTDIR)"

"$(INTDIR)\Mouse.sbr" : $(SOURCE) $(DEP_CPP_MOUSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_MOUSE=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Mouse.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\Mouse.obj" : $(SOURCE) $(DEP_CPP_MOUSE) "$(INTDIR)"

"$(INTDIR)\Mouse.sbr" : $(SOURCE) $(DEP_CPP_MOUSE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mps.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_MPS_C=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Mps.h"\
	

"$(INTDIR)\Mps.obj" : $(SOURCE) $(DEP_CPP_MPS_C) "$(INTDIR)"

"$(INTDIR)\Mps.sbr" : $(SOURCE) $(DEP_CPP_MPS_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_MPS_C=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Mps.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\Mps.obj" : $(SOURCE) $(DEP_CPP_MPS_C) "$(INTDIR)"

"$(INTDIR)\Mps.sbr" : $(SOURCE) $(DEP_CPP_MPS_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Screen.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_SCREE=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Screen.h"\
	

"$(INTDIR)\Screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"

"$(INTDIR)\Screen.sbr" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_SCREE=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Screen.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\Screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"

"$(INTDIR)\Screen.sbr" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\transport.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_TRANS=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\transport.h"\
	

"$(INTDIR)\transport.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"

"$(INTDIR)\transport.sbr" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_TRANS=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\transport.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\transport.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"

"$(INTDIR)\transport.sbr" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Typeinit.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_TYPEI=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Typeinit.h"\
	

"$(INTDIR)\Typeinit.obj" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"

"$(INTDIR)\Typeinit.sbr" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_TYPEI=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Typeinit.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\Typeinit.obj" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"

"$(INTDIR)\Typeinit.sbr" : $(SOURCE) $(DEP_CPP_TYPEI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Engine.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_ENGIN=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Engine.h"\
	

"$(INTDIR)\Engine.obj" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"

"$(INTDIR)\Engine.sbr" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_ENGIN=\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lin-city.h"\
	".\Engine.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\Engine.obj" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"

"$(INTDIR)\Engine.sbr" : $(SOURCE) $(DEP_CPP_ENGIN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Lcwin32.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_LCWIN=\
	".\Lin-city.h"\
	".\Common.h"\
	".\Lctypes.h"\
	

"$(INTDIR)\Lcwin32.obj" : $(SOURCE) $(DEP_CPP_LCWIN) "$(INTDIR)"

"$(INTDIR)\Lcwin32.sbr" : $(SOURCE) $(DEP_CPP_LCWIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_LCWIN=\
	".\Lin-city.h"\
	".\Common.h"\
	".\Lctypes.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\Lcwin32.obj" : $(SOURCE) $(DEP_CPP_LCWIN) "$(INTDIR)"

"$(INTDIR)\Lcwin32.sbr" : $(SOURCE) $(DEP_CPP_LCWIN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Lcwin32.h

!IF  "$(CFG)" == "lincity - Win32 Release"

!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lincity.rc
DEP_RSC_LINCI=\
	".\Small.ico"\
	".\lincity.ico"\
	".\Generic.h"\
	

"$(INTDIR)\lincity.res" : $(SOURCE) $(DEP_RSC_LINCI) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\winmain.cxx

!IF  "$(CFG)" == "lincity - Win32 Release"

DEP_CPP_WINMA=\
	".\Lin-city.h"\
	".\Common.h"\
	".\Generic.h"\
	".\Mouse.h"\
	

"$(INTDIR)\winmain.obj" : $(SOURCE) $(DEP_CPP_WINMA) "$(INTDIR)"

"$(INTDIR)\winmain.sbr" : $(SOURCE) $(DEP_CPP_WINMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

DEP_CPP_WINMA=\
	".\Lin-city.h"\
	".\Common.h"\
	".\Generic.h"\
	".\Mouse.h"\
	".\Lcx11.h"\
	".\Lcwin32.h"\
	

"$(INTDIR)\winmain.obj" : $(SOURCE) $(DEP_CPP_WINMA) "$(INTDIR)"

"$(INTDIR)\winmain.sbr" : $(SOURCE) $(DEP_CPP_WINMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\README.win32

!IF  "$(CFG)" == "lincity - Win32 Release"

!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################

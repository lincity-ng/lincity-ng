# Microsoft Developer Studio Generated NMAKE File, Based on lincity.dsp
!IF "$(CFG)" == ""
CFG=lincity - Win32 Debug
!MESSAGE No configuration specified. Defaulting to lincity - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "lincity - Win32 Release" && "$(CFG)" != "lincity - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "lincity - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\lincity.exe"


CLEAN :
	-@erase "$(INTDIR)\cliglobs.obj"
	-@erase "$(INTDIR)\dialbox.obj"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\fileutil.obj"
	-@erase "$(INTDIR)\geometry.obj"
	-@erase "$(INTDIR)\lchelp.obj"
	-@erase "$(INTDIR)\lclib.obj"
	-@erase "$(INTDIR)\lcwin32.obj"
	-@erase "$(INTDIR)\ldsvgui.obj"
	-@erase "$(INTDIR)\ldsvguts.obj"
	-@erase "$(INTDIR)\lincity.res"
	-@erase "$(INTDIR)\lintypes.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\market.obj"
	-@erase "$(INTDIR)\module_buttons.obj"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\mps.obj"
	-@erase "$(INTDIR)\pbar.obj"
	-@erase "$(INTDIR)\pixmap.obj"
	-@erase "$(INTDIR)\power.obj"
	-@erase "$(INTDIR)\readpng.obj"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\shrglobs.obj"
	-@erase "$(INTDIR)\shrtypes.obj"
	-@erase "$(INTDIR)\simulate.obj"
	-@erase "$(INTDIR)\splash.obj"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\transport.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\winmain.obj"
	-@erase "$(OUTDIR)\lincity.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "gettext-w32" /I "libpng-1.2.5" /I "zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\lincity.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\lincity.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lincity.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gnu_gettext.lib libpng.lib zdll.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\lincity.pdb" /machine:I386 /out:"$(OUTDIR)\lincity.exe" /libpath:"zlib" /libpath:"libpng-1.2.5" /libpath:"gettext-w32" 
LINK32_OBJS= \
	"$(INTDIR)\cliglobs.obj" \
	"$(INTDIR)\dialbox.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\fileutil.obj" \
	"$(INTDIR)\geometry.obj" \
	"$(INTDIR)\lchelp.obj" \
	"$(INTDIR)\lclib.obj" \
	"$(INTDIR)\lcwin32.obj" \
	"$(INTDIR)\ldsvgui.obj" \
	"$(INTDIR)\ldsvguts.obj" \
	"$(INTDIR)\lintypes.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\market.obj" \
	"$(INTDIR)\module_buttons.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\mps.obj" \
	"$(INTDIR)\pbar.obj" \
	"$(INTDIR)\pixmap.obj" \
	"$(INTDIR)\power.obj" \
	"$(INTDIR)\readpng.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\shrglobs.obj" \
	"$(INTDIR)\shrtypes.obj" \
	"$(INTDIR)\simulate.obj" \
	"$(INTDIR)\splash.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\transport.obj" \
	"$(INTDIR)\winmain.obj" \
	"$(INTDIR)\lincity.res"

"$(OUTDIR)\lincity.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\lincity.exe" "$(OUTDIR)\lincity.bsc"


CLEAN :
	-@erase "$(INTDIR)\cliglobs.obj"
	-@erase "$(INTDIR)\cliglobs.sbr"
	-@erase "$(INTDIR)\dialbox.obj"
	-@erase "$(INTDIR)\dialbox.sbr"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\engine.sbr"
	-@erase "$(INTDIR)\fileutil.obj"
	-@erase "$(INTDIR)\fileutil.sbr"
	-@erase "$(INTDIR)\geometry.obj"
	-@erase "$(INTDIR)\geometry.sbr"
	-@erase "$(INTDIR)\lchelp.obj"
	-@erase "$(INTDIR)\lchelp.sbr"
	-@erase "$(INTDIR)\lclib.obj"
	-@erase "$(INTDIR)\lclib.sbr"
	-@erase "$(INTDIR)\lcwin32.obj"
	-@erase "$(INTDIR)\lcwin32.sbr"
	-@erase "$(INTDIR)\ldsvgui.obj"
	-@erase "$(INTDIR)\ldsvgui.sbr"
	-@erase "$(INTDIR)\ldsvguts.obj"
	-@erase "$(INTDIR)\ldsvguts.sbr"
	-@erase "$(INTDIR)\lincity.res"
	-@erase "$(INTDIR)\lintypes.obj"
	-@erase "$(INTDIR)\lintypes.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\market.obj"
	-@erase "$(INTDIR)\market.sbr"
	-@erase "$(INTDIR)\module_buttons.obj"
	-@erase "$(INTDIR)\module_buttons.sbr"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\mouse.sbr"
	-@erase "$(INTDIR)\mps.obj"
	-@erase "$(INTDIR)\mps.sbr"
	-@erase "$(INTDIR)\pbar.obj"
	-@erase "$(INTDIR)\pbar.sbr"
	-@erase "$(INTDIR)\pixmap.obj"
	-@erase "$(INTDIR)\pixmap.sbr"
	-@erase "$(INTDIR)\power.obj"
	-@erase "$(INTDIR)\power.sbr"
	-@erase "$(INTDIR)\readpng.obj"
	-@erase "$(INTDIR)\readpng.sbr"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\screen.sbr"
	-@erase "$(INTDIR)\shrglobs.obj"
	-@erase "$(INTDIR)\shrglobs.sbr"
	-@erase "$(INTDIR)\shrtypes.obj"
	-@erase "$(INTDIR)\shrtypes.sbr"
	-@erase "$(INTDIR)\simulate.obj"
	-@erase "$(INTDIR)\simulate.sbr"
	-@erase "$(INTDIR)\splash.obj"
	-@erase "$(INTDIR)\splash.sbr"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\stats.sbr"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\timer.sbr"
	-@erase "$(INTDIR)\transport.obj"
	-@erase "$(INTDIR)\transport.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\winmain.obj"
	-@erase "$(INTDIR)\winmain.sbr"
	-@erase "$(OUTDIR)\lincity.bsc"
	-@erase "$(OUTDIR)\lincity.exe"
	-@erase "$(OUTDIR)\lincity.ilk"
	-@erase "$(OUTDIR)\lincity.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "gettext-w32" /I "libpng-1.2.5" /I "zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\lincity.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\lincity.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lincity.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cliglobs.sbr" \
	"$(INTDIR)\dialbox.sbr" \
	"$(INTDIR)\engine.sbr" \
	"$(INTDIR)\fileutil.sbr" \
	"$(INTDIR)\geometry.sbr" \
	"$(INTDIR)\lchelp.sbr" \
	"$(INTDIR)\lclib.sbr" \
	"$(INTDIR)\lcwin32.sbr" \
	"$(INTDIR)\ldsvgui.sbr" \
	"$(INTDIR)\ldsvguts.sbr" \
	"$(INTDIR)\lintypes.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\market.sbr" \
	"$(INTDIR)\module_buttons.sbr" \
	"$(INTDIR)\mouse.sbr" \
	"$(INTDIR)\mps.sbr" \
	"$(INTDIR)\pbar.sbr" \
	"$(INTDIR)\pixmap.sbr" \
	"$(INTDIR)\power.sbr" \
	"$(INTDIR)\readpng.sbr" \
	"$(INTDIR)\screen.sbr" \
	"$(INTDIR)\shrglobs.sbr" \
	"$(INTDIR)\shrtypes.sbr" \
	"$(INTDIR)\simulate.sbr" \
	"$(INTDIR)\splash.sbr" \
	"$(INTDIR)\stats.sbr" \
	"$(INTDIR)\timer.sbr" \
	"$(INTDIR)\transport.sbr" \
	"$(INTDIR)\winmain.sbr"

"$(OUTDIR)\lincity.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gnu_gettext.lib libpng.lib zdll.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\lincity.pdb" /debug /machine:I386 /out:"$(OUTDIR)\lincity.exe" /pdbtype:sept /libpath:"zlib" /libpath:"libpng-1.2.5" /libpath:"gettext-w32" 
LINK32_OBJS= \
	"$(INTDIR)\cliglobs.obj" \
	"$(INTDIR)\dialbox.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\fileutil.obj" \
	"$(INTDIR)\geometry.obj" \
	"$(INTDIR)\lchelp.obj" \
	"$(INTDIR)\lclib.obj" \
	"$(INTDIR)\lcwin32.obj" \
	"$(INTDIR)\ldsvgui.obj" \
	"$(INTDIR)\ldsvguts.obj" \
	"$(INTDIR)\lintypes.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\market.obj" \
	"$(INTDIR)\module_buttons.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\mps.obj" \
	"$(INTDIR)\pbar.obj" \
	"$(INTDIR)\pixmap.obj" \
	"$(INTDIR)\power.obj" \
	"$(INTDIR)\readpng.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\shrglobs.obj" \
	"$(INTDIR)\shrtypes.obj" \
	"$(INTDIR)\simulate.obj" \
	"$(INTDIR)\splash.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\transport.obj" \
	"$(INTDIR)\winmain.obj" \
	"$(INTDIR)\lincity.res"

"$(OUTDIR)\lincity.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("lincity.dep")
!INCLUDE "lincity.dep"
!ELSE 
!MESSAGE Warning: cannot find "lincity.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "lincity - Win32 Release" || "$(CFG)" == "lincity - Win32 Debug"
SOURCE=.\cliglobs.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\cliglobs.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\cliglobs.obj"	"$(INTDIR)\cliglobs.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dialbox.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\dialbox.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\dialbox.obj"	"$(INTDIR)\dialbox.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\engine.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\engine.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\engine.obj"	"$(INTDIR)\engine.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\fileutil.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\fileutil.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\fileutil.obj"	"$(INTDIR)\fileutil.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\geometry.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\geometry.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\geometry.obj"	"$(INTDIR)\geometry.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\lchelp.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\lchelp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\lchelp.obj"	"$(INTDIR)\lchelp.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\lclib.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\lclib.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\lclib.obj"	"$(INTDIR)\lclib.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\lcwin32.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\lcwin32.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\lcwin32.obj"	"$(INTDIR)\lcwin32.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ldsvgui.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\ldsvgui.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\ldsvgui.obj"	"$(INTDIR)\ldsvgui.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ldsvguts.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\ldsvguts.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\ldsvguts.obj"	"$(INTDIR)\ldsvguts.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\lincity.rc

"$(INTDIR)\lincity.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\lintypes.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\lintypes.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\lintypes.obj"	"$(INTDIR)\lintypes.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\main.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\market.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\market.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\market.obj"	"$(INTDIR)\market.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\module_buttons.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\module_buttons.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\module_buttons.obj"	"$(INTDIR)\module_buttons.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mouse.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\mouse.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\mouse.obj"	"$(INTDIR)\mouse.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mps.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\mps.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\mps.obj"	"$(INTDIR)\mps.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pbar.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\pbar.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\pbar.obj"	"$(INTDIR)\pbar.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pixmap.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\pixmap.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\pixmap.obj"	"$(INTDIR)\pixmap.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\power.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\power.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\power.obj"	"$(INTDIR)\power.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\readpng.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\readpng.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\readpng.obj"	"$(INTDIR)\readpng.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\screen.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\screen.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\screen.obj"	"$(INTDIR)\screen.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\shrglobs.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\shrglobs.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\shrglobs.obj"	"$(INTDIR)\shrglobs.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\shrtypes.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\shrtypes.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\shrtypes.obj"	"$(INTDIR)\shrtypes.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\simulate.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\simulate.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\simulate.obj"	"$(INTDIR)\simulate.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\splash.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\splash.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\splash.obj"	"$(INTDIR)\splash.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stats.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\stats.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\stats.obj"	"$(INTDIR)\stats.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\timer.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\timer.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\timer.obj"	"$(INTDIR)\timer.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\transport.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\transport.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\transport.obj"	"$(INTDIR)\transport.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\winmain.c

!IF  "$(CFG)" == "lincity - Win32 Release"


"$(INTDIR)\winmain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lincity - Win32 Debug"


"$(INTDIR)\winmain.obj"	"$(INTDIR)\winmain.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 


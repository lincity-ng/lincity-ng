## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

### DEFS = @DEFS@ -DLIBDIR=\"${pkgdatadir}\"
DEFS = @DEFS@
X_DEFS = -DLC_X11=1
SVGA_DEFS = -DSVGALIB=1

### SUBDIRS = intl po
## (deleted)
##	acconfig.h   \
##	config.h \

noinst_HEADERS = \
	cliglobs.h   \
	common.h     confw32.h    \
	engglobs.h   engine.h	 fileutil.h \
	generic.h    geometry.h  dialbox.h \
	lcconfig.h   \
	lchelp.h       \
	lcintl.h     lcstring.h	lclib.h\
	lctypes.h    lcwin32.h \
	lcx11.h      lcsvga.h \
	ldsvgui.h    ldsvguts.h   \
	lin-city.h   lintypes.h	 market.h \
	module_buttons.h \
	mouse.h      mps.h	  pbar.h    \
	picedit.h    pixmap.h     power.h \
	resource.h	 screen.h \
	shrglobs.h   stats.h \
	simulate.h   timer.h	 transport.h \
	typeinit.h   xpicedit.h

# Directories that need to have CVS and ca subdirectories removed
LC_DISTDIRS = \
	help icons messages \
	old.Makefiles opening tools

EXTRA_DIST = $(LC_DISTDIRS) \
	colour.pal \
	lcwin32.c winmain.c \
	lincity.ico  small.ico lincity.rc \
	FAQ CHANGES COPYRIGHT Acknowledgements \
	config.sh \
	README.profiling \
	lincity.6

man_MANS = lincity.6
## pkgdata_DATA = help/* icons/* messages/* messages/ca/* opening/* colour.pal
## pkgdata_DATA = help/* icons/* messages/* opening/* colour.pal
nobase_pkgdata_DATA = help/* icons/* messages/* opening/* colour.pal


############################################################################
## INSTALL SCRIPTS
############################################################################
## There are three "non-standard" things that I need to do here.  
## 1)  I need to make the directories .../share/lincity/help, icons, 
##     messages and opening because automake is not smart enough to do this.
## 2)  I need to check to see if the user chose to make the optionally
##     compiled picedit/xpicedit, and install these.
## 3)  I need to set the suid bit on lincity/picedit.
############################################################################
###########   GCS Jan 19, 2003
############################################################################
##   1)  No longer need to make the directories.  Automake now supports the
##       "nobase" option.
##	    $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
##	    $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/help
##	    $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/icons
##	    $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/messages
##	    $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/opening
##   2)  Removed the following line from install-exec-local:
##	    $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/messages/ca
############################################################################

install-exec-local:
	@list='$(EXTRA_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "  $(INSTALL_PROGRAM) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`"; \
	     $(INSTALL_PROGRAM) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	  else :; fi; \
	done; \
        if test -f $(DESTDIR)$(bindir)/`echo lincity|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; then \
	  chown root $(DESTDIR)$(bindir)/`echo lincity|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	  chmod 4755 $(DESTDIR)$(bindir)/`echo lincity|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	fi

## Uninstall also removes the extra programs and directories
uninstall-local:
	list='$(EXTRA_PROGRAMS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	done
	rm -rf $(DESTDIR)$(pkgdatadir)


############################################################################
## DISTRIBUTION BUILD SCRIPTS
## --
## This is needed to keep the CVS directories from being installed in 
## my subdirectories.
## --
## GCS Jan 19, 2003  Added a check for deleting the "ca" subdirectory
##                   Added lowercase "cvs" directory (when copied from dos)
##  (removed this!)
##          cp -r $$p $(distdir)/$$p; \
##  (modified this)
# 	  if test -d $(distdir)/$$p/CVS ; then \
# 	    echo "rm -rf $(distdir)/$$p/CVS" ; \
# 	    rm -rf $(distdir)/$$p/CVS ; \
# 	    echo "rm -rf $(distdir)/$$p/*/CVS" ; \
# 	    rm -rf $(distdir)/$$p/*/CVS ; \
# 	    echo "rm -rf $(distdir)/$$p/*/*/CVS" ; \
# 	    rm -rf $(distdir)/$$p/*/*/CVS ; \
# 	    echo "rm -rf $(distdir)/$$p/cvs" ; \
# 	    rm -rf $(distdir)/$$p/cvs ; \
# 	    echo "rm -rf $(distdir)/$$p/*/cvs" ; \
# 	    rm -rf $(distdir)/$$p/*/cvs ; \
# 	    echo "rm -rf $(distdir)/$$p/*/*/cvs" ; \
# 	    rm -rf $(distdir)/$$p/*/*/cvs ; \
# 	  else :; fi; \
##
##	  echo "Erasing CVS directories..." \
############################################################################
dist-hook:
	@list='$(LC_DISTDIRS)'; for p in $$list; do \
	  rm -rf `find $(distdir)/$$p -name CVS -print` ; \
	  rm -rf `find $(distdir)/$$p -name cvs -print` ; \
	  rm -rf `find $(distdir)/$$p -name ca -print` ; \
	done


############################################################################
## PROGRAM BUILD SCRIPTS
############################################################################

## These executables might not be built
EXTRA_PROGRAMS = xlincity lincity xpicedit picedit

X_PROGS: xlincity$(EXEEXT)
SVGA_PROGS: lincity$(EXEEXT)
LCSERVER_PROGS: lcserver$(EXEEXT)

## These executables are always built
bin_PROGRAMS = @PROGS@


XLC_OBJ = \
	x_ldsvgui.o x_splash.o x_lchelp.o x_main.o x_mouse.o \
	x_cliglobs.o x_mps.o x_screen.o x_lcx11.o x_pixmap.o x_geometry.o \
	x_pbar.o x_dialbox.o x_module_buttons.o

LC_OBJ = \
	vga_ldsvgui.o vga_splash.o vga_lchelp.o vga_main.o vga_mouse.o \
	vga_cliglobs.o vga_mps.o vga_lcsvga.o vga_screen.o vga_geometry.o \
	vga_pbar.o vga_dialbox.o vga_module_buttons.o


## Note:  If the executable requires a prefix (x_ or vga_), then 
##        it should be listed below under EXTRA_*_SOURCES.  Otherwise,
##        it is a "regular" source file.


## ----- xlincity -----
xlincity_SOURCES = \
	lclib.c power.c \
	ldsvguts.c engine.c transport.c market.c \
	simulate.c shrtypes.c lintypes.c \
	timer.c fileutil.c shrglobs.c stats.c
EXTRA_xlincity_SOURCES = \
	ldsvgui.c splash.c lchelp.c main.c mouse.c mps.c screen.c \
	lcx11.c pixmap.c cliglobs.c geometry.c pbar.c module_buttons.c \
	dialbox.c
xlincity_DEPENDENCIES = \
	${XLC_OBJ}
xlincity_LDADD = \
	${XLC_OBJ} \
	@X_LIBS@ -lXext -lX11 -lm


## ----- lincity -----
lincity_SOURCES = \
	lclib.c power.c \
	simulate.c shrtypes.c lintypes.c \
	timer.c fileutil.c shrglobs.c stats.c
EXTRA_lincity_SOURCES = \
	ldsvgui.c splash.c lchelp.c main.c mouse.c mps.c screen.c \
	lcx11.c pixmap.c cliglobs.c geometry.c pbar.c module_buttons.c \
	dialbox.c
lincity_DEPENDENCIES = \
	${LC_OBJ}
lincity_LDADD = \
	${LC_OBJ} \
	-lvgagl -lvga -lm

## ----- xpicedit -----
xpicedit_SOURCES = 
EXTRA_xpicedit_SOURCES = picedit.c
xpicedit_LDADD = x_picedit.o -lXext -lX11 -lm

## ----- picedit -----
picedit_SOURCES = 
EXTRA_picedit_SOURCES = picedit.c
picedit_LDADD = vga_picedit.o -lvgagl -lvga -lm

## ----------------- VGA RULES ------------------
vga_cliglobs.o: cliglobs.c cliglobs.h
	$(COMPILE) $(SVGA_DEFS) -o vga_cliglobs.o -c cliglobs.c

vga_splash.o: splash.c
	$(COMPILE) $(SVGA_DEFS) -o vga_splash.o -c splash.c

vga_ldsvgui.o: ldsvgui.c
	$(COMPILE) $(SVGA_DEFS) -o vga_ldsvgui.o -c ldsvgui.c

vga_geometry.o: geometry.c geometry.h
	$(COMPILE) $(SVGA_DEFS) -o vga_geometry.o -c geometry.c

vga_lchelp.o: lchelp.c lchelp.h lin-city.h lctypes.h module_buttons.h
	$(COMPILE) $(SVGA_DEFS) -o vga_lchelp.o -c lchelp.c

vga_lcsvga.o: lcsvga.c lcsvga.h lin-city.h lctypes.h
	$(COMPILE) $(SVGA_DEFS) -o vga_lcsvga.o -c lcsvga.c

vga_main.o: main.c lin-city.h lctypes.h
	$(COMPILE) $(SVGA_DEFS) -o vga_main.o -c main.c

vga_mouse.o: mouse.c mouse.h lin-city.h lctypes.h module_buttons.h
	$(COMPILE) $(SVGA_DEFS) -o vga_mouse.o -c mouse.c

vga_mps.o: mps.c mps.h lin-city.h lctypes.h
	$(COMPILE) $(SVGA_DEFS) -o vga_mps.o -c mps.c

vga_pbar.o: pbar.c pbar.h lin-city.h shrglobs.h lchelp.h cliglobs.h
	$(COMPILE) $(SVGA_DEFS) -o vga_pbar.o -c pbar.c

vga_screen.o: screen.c screen.h lin-city.h lctypes.h module_buttons.h
	$(COMPILE) $(SVGA_DEFS) -o vga_screen.o -c screen.c

vga_dialbox.o: dialbox.c dialbox.h geometry.h
	$(COMPILE) $(SVGA_DEFS) -o vga_dialbox.o -c dialbox.c

vga_module_buttons.o: module_buttons.c module_buttons.h lin-city.h lctypes.h \
	mouse.h
	$(COMPILE) $(SVGA_DEFS) -o vga_module_buttons.o -c module_buttons.c


## ----------------- X RULES ------------------
x_cliglobs.o: cliglobs.c cliglobs.h
	$(COMPILE) $(X_DEFS) -o x_cliglobs.o -c cliglobs.c

x_splash.o: splash.c
	$(COMPILE) $(X_DEFS) -o x_splash.o -c splash.c

x_ldsvgui.o: ldsvgui.c
	$(COMPILE) $(X_DEFS) -o x_ldsvgui.o -c ldsvgui.c

x_geometry.o: geometry.c geometry.h
	$(COMPILE) $(X_DEFS) -o x_geometry.o -c geometry.c

x_lchelp.o: lchelp.c lchelp.h lin-city.h lctypes.h module_buttons.h
	$(COMPILE) $(X_DEFS) -o x_lchelp.o -c lchelp.c

x_lcx11.o: lcx11.c lcx11.h lin-city.h lctypes.h
	$(COMPILE) $(X_DEFS) -o x_lcx11.o -c lcx11.c

x_pixmap.o: pixmap.c lcx11.h lin-city.h lctypes.h
	$(COMPILE) $(X_DEFS) -o x_pixmap.o -c pixmap.c

x_main.o: main.c lin-city.h lctypes.h lcx11.h
	$(COMPILE) $(X_DEFS) -o x_main.o -c main.c

x_mouse.o: mouse.c mouse.h lin-city.h lctypes.h module_buttons.h
	$(COMPILE) $(X_DEFS) -o x_mouse.o -c mouse.c

x_mps.o: mps.c mps.h lin-city.h lctypes.h
	$(COMPILE) $(X_DEFS) -o x_mps.o -c mps.c

x_pbar.o: pbar.c pbar.h lin-city.h shrglobs.h lchelp.h cliglobs.h
	$(COMPILE) $(X_DEFS) -o x_pbar.o -c pbar.c

x_screen.o: screen.c screen.h lin-city.h lctypes.h lcx11.h module_buttons.h
	$(COMPILE) $(X_DEFS) -o x_screen.o -c screen.c

x_dialbox.o: dialbox.c dialbox.h lcx11.h geometry.h
	$(COMPILE) $(X_DEFS) -o x_dialbox.o -c dialbox.c

x_module_buttons.o: module_buttons.c module_buttons.h lin-city.h lctypes.h \
	mouse.h
	$(COMPILE) $(X_DEFS) -o x_module_buttons.o -c module_buttons.c


## ----------------- PICEDIT RULES ------------------
x_picedit.o: picedit.c
	$(COMPILE) $(X_DEFS) -o x_picedit.o -c picedit.c

vga_picedit.o: picedit.c
	$(COMPILE) $(X_DEFS) -o vga_picedit.o -c picedit.c


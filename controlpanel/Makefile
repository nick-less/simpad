# $Id$
TARGETS		= controlPanel
DIST		= controlpanel
VERMAJ		= 1
VERMIN		= 0
VERPAT		= 0
VERSION		= $(VERMAJ).$(VERMIN).$(VERPAT)

CC		= g++

# Common directories and libraries
INCDIR		= -I.
LIBDIR		= -L.
LIBS		= -lfltk

OPTIM		= -O3 -fomit-frame-pointer 
CCOPT		= -Wall $(OPTIM) $(INCDIR) $(XINCDIR) -DVERSION=\"$(VERSION)\"
LDOPT		= $(LIBDIR)

# Object files common to all programs.
OBJS		= controlPanel.o controlPanelImpl.o controlPanelMain.o netdevice.o wifiDev.o


all: $(TARGETS)

controlPanel.o: controlPanel.h controlPanel.cpp
	$(CC) $(CCOPT)  -g -c controlPanel.cpp

wifiDev.o: wifiDev.h wifiDev.cpp
	$(CC) $(CCOPT)  -g -c wifiDev.cpp

netdevice.o: netdevice.h netdevice.cpp
	$(CC) $(CCOPT)  -g -c netdevice.cpp

controlPanelImpl.o: controlPanel.h controlPanelImpl.hpp controlPanelImpl.cpp netdevice.h wifiDev.h
	$(CC) $(CCOPT)  -g -c controlPanelImpl.cpp

controlPanelMain.o: controlPanelImpl.hpp controlPanelMain.cpp
	$(CC) $(CCOPT)  -g -c controlPanelMain.cpp

controlPanel: $(OBJS)
	$(CC) $(CCOPT) -g -o $@ $(OBJS) $(LDOPT) $(LIBS) 

clean:
	rm -f *.o core depend *~

veryclean: clean
	rm -f $(TARGETS) $(DIST)-$(VERSION).tar.gz

chmod:
	chmod a+r *

depend dep:
	$(CC) $(INCDIR) $(XINCDIR) -MM *.cxx >depend

# To let the authors make a distribution. The rest of the Makefile
# should be used by the authors only.
LSMFILE		= $(DIST)-$(VERSION).lsm
DISTDIR		= $(DIST)-$(VERSION)
DISTFILE	= $(DIST)-$(VERSION).tar.gz
DISTFILES	= README INSTALL RULES $(LSMFILE) \
		  Makefile Makefile.bcc \
		  mancala.c mancala.h minimax.c minimax.h \
		  textmain.c \
		  rulestxt.c \
		  fltk_mancala_main.cxx

$(LSMFILE): FORCE
	VER=$(VERSION); \
	DATE=`date "+%d%b%y"|tr '[a-z]' '[A-Z]'`; \
	sed -e "s/VER/$$VER/g;s/DATE/$$DATE/g" $(DIST).lsm.in > $(LSMFILE)

FORCE:

# Warning: distclean removes the lsm-file, which can not be
# regenerated without the lsm.in-file, which is not part of the package.
distclean: veryclean
	rm -f $(LSMFILE)

dist: $(LSMFILE) chmod
	mkdir $(DISTDIR)
	chmod a+rx $(DISTDIR)
	ln $(DISTFILES) $(DISTDIR)
	tar -cvzf $(DISTFILE) $(DISTDIR)
	chmod a+r $(DISTFILE)
	rm -rf $(DISTDIR)

ifeq (depend,$(wildcard depend))
include depend
endif


JAUSDIR  := ../../..
EXTBUILDDIR := $(JAUSDIR)/ext/build/linux
LINUXBUILDDIR := $(JAUSDIR)/build/linux

MSGMFDIR    := $(LINUXBUILDDIR)/jaus++/jmsglib
CMPMFDIR    := $(LINUXBUILDDIR)/jaus++/jcmplib
SRVMFDIR    := $(LINUXBUILDDIR)/jaus++/jsrvclib
VIDMFDIR    := $(LINUXBUILDDIR)/jaus++/jvideolib
INSTALLDIR  := /usr

all:
#	cd $(EXTBUILDDIR)/tinyxml; make
	cd $(MSGMFDIR); make
	cd $(CMPMFDIR); make
	cd $(SRVMFDIR); make
	cd $(VIDMFDIR); make


clean:
#	cd $(EXTBUILDDIR)/tinyxml; make clean
	cd $(MSGMFDIR); make clean
	cd $(CMPMFDIR); make clean
	cd $(SRVMFDIR); make clean
	cd $(VIDMFDIR); make clean

allclean:
#	cd $(EXTBUILDDIR)/tinyxml; make allclean
	cd $(MSGMFDIR); make allclean
	cd $(CMPMFDIR); make allclean
	cd $(SRVMFDIR); make allclean
	cd $(VIDMFDIR); make allclean
	
install:
	if test -d $(INSTALLDIR)/include/jaus; \
	then echo exists;                \
	else mkdir $(INSTALLDIR)/include/jaus; \
	fi
	cd $(MSGMFDIR); make install
	cd $(CMPMFDIR); make install
	cd $(SRVMFDIR); make install
	cd $(VIDMFDIR); make install
	
uninstall:
	cd $(MSGMFDIR); make uninstall
	cd $(CMPMFDIR); make uninstall
	cd $(SRVMFDIR); make uninstall
	cd $(VIDMFDIR); make uninstall
	if test -d $(INSTALLDIR)/include/jaus; \
	then rmdir $(INSTALLDIR)/include/jaus; \
	else echo done; \
	fi

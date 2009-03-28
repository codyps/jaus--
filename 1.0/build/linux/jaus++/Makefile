
JAUSDIR  := ../../..
EXTBUILDDIR := $(JAUSDIR)/ext/build/linux
LINUXBUILDDIR := $(JAUSDIR)/build/linux

MSGMFDIR    := $(LINUXBUILDDIR)/jaus++/jmsglib
CMPMFDIR    := $(LINUXBUILDDIR)/jaus++/jcmplib
SRVMFDIR    := $(LINUXBUILDDIR)/jaus++/jsrvclib
VIDMFDIR    := $(LINUXBUILDDIR)/jaus++/jvideolib
INCDIR		:= include/jaus
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

install:
	cp -rf $(JAUSDIR)/$(INCDIR)/ $(INSTALLDIR)/include
	cd $(MSGMFDIR); make install
	cd $(CMPMFDIR); make install
	cd $(SRVMFDIR); make install
	cd $(VIDMFDIR); make install
	
uninstall:
	rm -rf $(INSTALLDIR)/$(INCDIR)
	cd $(MSGMFDIR); make uninstall
	cd $(CMPMFDIR); make uninstall
	cd $(SRVMFDIR); make uninstall
	cd $(VIDMFDIR); make uninstall


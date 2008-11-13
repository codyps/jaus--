
JAUSDIR  := ../../..
LINUXBUILDDIR := $(JAUSDIR)/build/linux
MSGMFDIR    := $(LINUXBUILDDIR)/jaus++/jmsglib
CMPMFDIR    := $(LINUXBUILDDIR)/jaus++/jcmplib
SRVMFDIR    := $(LINUXBUILDDIR)/jaus++/jsrvclib
VIDMFDIR    := $(LINUXBUILDDIR)/jaus++/jvideolib

all:
	cd $(MSGMFDIR); make
	cd $(CMPMFDIR); make
	cd $(SRVMFDIR); make
	cd $(VIDMFDIR); make


clean:
	cd $(MSGMFDIR); make clean
	cd $(CMPMFDIR); make clean
	cd $(SRVMFDIR); make clean
	cd $(VIDMFDIR); make clean

allclean:
	cd $(MSGMFDIR); make allclean
	cd $(CMPMFDIR); make allclean
	cd $(SRVMFDIR); make allclean
	cd $(VIDMFDIR); make allclean

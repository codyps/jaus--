#main jaus makefile

CXUTILSDIR := ../../../../../cxutils/trunk/1.0/
CXUTILSLINUXDIR := $(CXUTILSDIR)/build/linux
JAUSDIR  := ../..
LINUXBUILDDIR := $(JAUSDIR)/build/linux
JAUSBUILDDIR    := $(LINUXBUILDDIR)/jaus++
EXAMPLEBUILDDIR := $(LINUXBUILDDIR)/example
EXTBUILDDIR := $(JAUSDIR)/ext/build/linux

all:
#	cd $(CXUTILSLINUXDIR); make
	cd $(JAUSBUILDDIR); make
	cd $(EXAMPLEBUILDDIR); make
	
clean:
#	cd $(EXTBUILDDIR); make clean
#	cd $(CXUTILSLINUXDIR); make clean
	cd $(JAUSBUILDDIR); make clean
	cd $(EXAMPLEBUILDDIR); make clean

external:
	cd $(EXTBUILDDIR); make libz
	cd $(EXTBUILDDIR); make libpng
	cd $(EXTBUILDDIR); make libjpeg
	cd $(EXTBUILDDIR); make libtinyxml

help:
	echo $(JAUSBUILDDIR)
	
install:
#	cd $(CXUTILSLINUXDIR); make install
	cd $(JAUSBUILDDIR); make install

uninstall:
#	cd $(CXUTILSLINUXDIR); make uninstall
	cd $(JAUSBUILDDIR); make uninstall

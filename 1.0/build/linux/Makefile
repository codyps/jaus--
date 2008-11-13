#main jaus makefile

CXUTILSDIR := ../../../../../cxutils/trunk/1.0/
CXUTILSLINUXDIR := $(CXUTILSDIR)/build/linux
JAUSDIR  := ../..
LINUXBUILDDIR := $(JAUSDIR)/build/linux
JAUSBUILDDIR    := $(LINUXBUILDDIR)/jaus++
EXAMPLEBUILDDIR := $(LINUXBUILDDIR)/example
EXTBUILDDIR := $(JAUSDIR)/ext/build/linux

all:
	cd $(CXUTILSLINUXDIR); make
	cd $(JAUSBUILDDIR); make
	cd $(EXAMPLEBUILDDIR); make
clean:
	cd $(EXTBUILDDIR); make clean
	cd $(CXUTILSLINUXDIR); make clean
	cd $(JAUSBUILDDIR); make clean
	cd $(EXAMPLEBUILDDIR); make clean

allclean:
	cd $(EXTBUILDDIR); make clean
	cd $(CXUTILSLINUXDIR); make clean
	cd $(JAUSBUILDDIR); make allclean
	cd $(EXAMPLEBUILDDIR); make allclean

external:
	cd $(EXTBUILDDIR); make configure
	cd $(EXTBUILDDIR); make install

help:
	echo $(JAUSBUILDDIR)

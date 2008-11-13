JAUSDIR  := ../../..
TINYBUILDDIR := $(JAUSDIR)/ext/build/linux/tinyxml
LIBJPEGBUILDDIR := $(JAUSDIR)/ext/build/linux/libjpeg-6b
LIBPNGBUILDDIR := $(JAUSDIR)/ext/build/linux/libpng-1.2.31
LIBZBUILDDIR := $(JAUSDIR)/ext/build/linux/zlib-1.2.3


all:
	cd $(TINYBUILDDIR); make
	cd $(LIBJPEGBUILDDIR); make
	cd $(LIBPNGBUILDDIR); make
	cd $(LIBZBUILDDIR); make

clean:
	cd $(TINYBUILDDIR); make clean
	cd $(LIBJPEGBUILDDIR); make clean
	cd $(LIBPNGBUILDDIR); make clean
	cd $(LIBZBUILDDIR); make clean

install:
	cd $(LIBJPEGBUILDDIR); make install
	cd $(LIBPNGBUILDDIR); make install
	cd $(LIBZBUILDDIR); make install

configure:
	cd $(LIBJPEGBUILDDIR); ./configure
	cd $(LIBPNGBUILDDIR); ./configure
	cd $(LIBZBUILDDIR); ./configure

JAUSDIR  := ../../..
TINYBUILDDIR := $(JAUSDIR)/ext/build/linux/tinyxml
LIBJPEGBUILDDIR := $(JAUSDIR)/ext/build/linux/libjpeg-6b
LIBPNGBUILDDIR := $(JAUSDIR)/ext/build/linux/libpng-1.2.31
LIBZBUILDDIR := $(JAUSDIR)/ext/build/linux/zlib-1.2.3


all:
	cd $(LIBZBUILDDIR); make
	cd $(TINYBUILDDIR); make
	cd $(LIBJPEGBUILDDIR); make
	cd $(LIBPNGBUILDDIR); make
	

clean:
	cd $(TINYBUILDDIR); make clean
	cd $(LIBJPEGBUILDDIR); make clean
	cd $(LIBPNGBUILDDIR); make clean
	cd $(LIBZBUILDDIR); make clean

install:
	cd $(LIBZBUILDDIR); make install
	cd $(LIBJPEGBUILDDIR); make install
	cd $(LIBPNGBUILDDIR); make install
	

configure:
	cd $(LIBZBUILDDIR); ./configure
	cd $(LIBJPEGBUILDDIR); ./configure
	cd $(LIBPNGBUILDDIR); ./configure
	
libz:
	cd $(LIBZBUILDDIR); ./configure
	cd $(LIBZBUILDDIR); make clean
	cd $(LIBZBUILDDIR); make
	cd $(LIBZBUILDDIR); make install

libjpeg:
	cd $(LIBJPEGBUILDDIR); ./configure
	cd $(LIBJPEGBUILDDIR); make clean
	cd $(LIBJPEGBUILDDIR); make
	cd $(LIBJPEGBUILDDIR); make install
libpng:
	cd $(LIBPNGBUILDDIR); ./configure
	cd $(LIBPNGBUILDDIR); make clean
	cd $(LIBPNGBUILDDIR); make
	cd $(LIBPNGBUILDDIR); make install
libtinyxml:
	cd $(TINYBUILDDIR); make clean
	cd $(TINYBUILDDIR); make


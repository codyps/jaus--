# make file
 
ARCHIVE  	:= libjcmp.1.0
CC 		:= gcc
CXX      	:= g++
CXXFLAGS 	:= -g -c -fPIC -Wall

JAUSDIR         := ../../../..
DESTDIR         := $(JAUSDIR)/lib
ODIR            := $(JAUSDIR)/obj
CXUTILSDIR      := $(JAUSDIR)/../../../cxutils/trunk/1.0
CXUTILSLIBDIR 	:= $(CXUTILSDIR)/lib
EXTLIBDIR     	:= $(JAUSDIR)/ext/lib
EXTSRCDIR       := $(JAUSDIR)/ext/src
JAUSLIBDIR	    := $(JAUSDIR)/lib
INSTALLDIR   	:= /usr
USRLIBDIR	    := $(INSTALLDIR)/lib
#--------------------------------------------
#  Component source files directories
#--------------------------------------------
CMPSRC   	:= $(JAUSDIR)/src/jaus/components

CMPNODESRC  := $(CMPSRC)/node
CMPTRANSRC  := $(CMPSRC)/transport

CMPSRCS		:=  $(CMPSRC) $(CMPNODESRC) $(CMPTRANSRC)
	

#---------------------------------------
#  Source and Object files for compiling
#---------------------------------------
TINYSRCFILES        := $(foreach DIR, $(EXTSRCDIR)/tinyxml, $(addprefix $(DIR)/, $(notdir $(wildcard $(DIR)/*.cpp))))
SOURCEFILES	        := 	$(foreach DIR,$(CMPSRCS), $(addprefix $(DIR)/, $(notdir $(wildcard $(DIR)/*.cpp)))) $(TINYSRCFILES)
SOURCEFILENAMES     := $(foreach DIR, $(CMPSRCS),$(notdir $(wildcard $(DIR)/*.cpp)))
OBJECTFILENAMES	    :=	$(foreach DIR, $(SOURCEFILENAMES),$(addprefix $(ODIR)/,$(DIR)))
OBJECTFILES         :=	$(SOURCEFILES:.cpp=.o)

#---------------------------------------
#   includes dirs
#---------------------------------------
CMPINC 		:= $(JAUSDIR)/include
CXUTILSINC := $(CXUTILSDIR)/include
	
EXTINC		:= $(JAUSDIR)/ext/include
VLDINC		:= $(EXTINC)/vld
CVINC		:= $(EXTINC)/cv
LIBPNGINC	:= $(EXTINC)/libpng
TINYXMLINC	:= $(EXTINC)/tinyxml
LIBJPEGINC	:= $(EXTINC)/libjpeg
ZLIBINC		:= $(EXTINC)/zlib
EXTINCS		:= $(VLDINC) \
			$(CVINC) \
			$(LIBPNGINC) \
			$(TINYXMLINC) \
			$(LIBJPEGINC) \
			$(ZLIBINC) \
			$(CXUTILSINC)

#---------------------------------------
#   objects
#---------------------------------------
CXUTILSOBJDIR := $(CXUTILSDIR)/build/linux
CXUTILSOBJS := $(foreach DIR,$(CXUTILSOBJ),  $(addprefix $(CXUTILSOBJ)/, $(notdir $(wildcard $(DIR)/*.o))))

#---------------------------------------
#  all Includes
#---------------------------------------

INCLUDES := $(CMPINC) $(EXTINCS)

#---------------------------------------
#  Libraries
#---------------------------------------

LIBPATHS 	:= -L$(CXUTILSLIBDIR) \
			-L$(JAUSLIBDIR) \
			-L$(EXTLIBDIR) \
			-L$(USRLIBDIR)

CXUTILSLIB	:= -lcxutils.1.0
JMSGLIB		:= -ljmsg.1.0
JVIDLIB		:= -ljvid.1.0
JSRVLIB		:= -ljsrv.1.0
TINYLIB		:= -ltinyxml
PTHREADLIB	:= -lpthread

DYNAMICLIBS		:= $(CXUTILSLIB) \
			$(JMSGLIB) \
			$(PTHREADLIB)
STATICLIBS  	:= $(TINYLIB) 
#---------------------------------------
#  dependencies
#---------------------------------------


#---------------------------------------
#  rules
#---------------------------------------


all: $(ARCHIVE).a $(ARCHIVE).so

$(ARCHIVE).so:
	$(CXX) -shared -o $(DESTDIR)/$@ $(OBJECTFILES) $(LIBPATHS) $(DYNAMICLIBS)

$(ARCHIVE).a: $(OBJECTFILES) 
	ar -ruc $(DESTDIR)/$@ $? 

.cpp.o:
	@echo $(CXX) $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) -c $? -o $@
	@$(CXX) $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) -c $? -o $@
	
#depend:
#	makedepend --  $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) $(SOURCEFILES)
	
clean:
	@echo cleaning up
	@-rm -f $(foreach DIR,$(CMPSRCS), $(DIR)/*.o)
	@-rm -f $(ODIR)/*.o

allclean:
	@echo cleaning up
	@-rm -f $(foreach DIR,$(CMPSRCS), $(DIR)/*.o)
	@-rm -f $(ODIR)/*.o
	@-rm -f $(foreach DIR,$(CMPSRCS), $(DIR)/$(ARCHIVE).a)
	@-rm -f $(DESTDIR)/$(ARCHIVE).a
	@-rm -f $(foreach DIR,$(CMPSRCS), $(DIR)/$(ARCHIVE).so)
	@-rm -f $(DESTDIR)/$(ARCHIVE).so
	
install:
	cp $(DESTDIR)/$(ARCHIVE).so $(INSTALLDIR)/lib
	cp $(DESTDIR)/$(ARCHIVE).a $(INSTALLDIR)/lib
	if test -d $(INSTALLDIR)/include/jaus/components; \
	then echo exists;                \
	else mkdir $(INSTALLDIR)/include/jaus/components; \
	fi
	cp $(JAUSDIR)/include/jaus/components/*.h $(INSTALLDIR)/include/jaus/components/
	
uninstall:
	rm $(INSTALLDIR)/lib/$(ARCHIVE).so
	rm $(INSTALLDIR)/lib/$(ARCHIVE).a
	rm $(INSTALLDIR)/include/jaus/components/*.h
	rmdir $(INSTALLDIR)/include/jaus/components/	

help: 
	@echo $(OBJECTFILES)
	@echo $@

# DO NOT DELETE


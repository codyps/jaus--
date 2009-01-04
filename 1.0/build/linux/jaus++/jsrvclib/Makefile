# make file
 
ARCHIVE  	:= libjsrvc.1.0
CC 		:= gcc
CXX      	:= g++
CXXFLAGS 	:= -g -c -fPIC -Wall

JAUSDIR  := ../../../..
DESTDIR  := $(JAUSDIR)/lib
ODIR     := $(JAUSDIR)/obj
CXUTILSDIR := $(JAUSDIR)/../../../cxutils/trunk/1.0
INSTALLDIR   	:= /usr

#--------------------------------------------
#  Service source files directories
#--------------------------------------------
SRVSRC   	:= $(JAUSDIR)/src/jaus/services
SRVSRCS		:=  $(SRVSRC)
	

#---------------------------------------
#  Source and Object files for compiling
#---------------------------------------

SOURCEFILES	:= 	$(foreach DIR,$(SRVSRCS), $(addprefix $(DIR)/, $(notdir $(wildcard $(DIR)/*.cpp))))
SOURCEFILENAMES := $(foreach DIR, $(SRVSRCS),$(notdir $(wildcard $(DIR)/*.cpp)))
OBJECTFILENAMES	:=	$(foreach DIR, $(SOURCEFILENAMES),$(addprefix $(ODIR)/,$(DIR)))
OBJECTFILES :=	$(SOURCEFILES:.cpp=.o)

#---------------------------------------
#   includes dirs
#---------------------------------------
SRVINC 		:= $(JAUSDIR)/include
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

INCLUDES := $(SRVINC) $(EXTINCS)

#---------------------------------------
#  Libraries
#---------------------------------------
CXUTILSLIBDIR := $(CXUTILSDIR)/lib
EXTLIBSDIR     := $(JAUSDIR)/ext/lib

LIBS := $(CXUTILSLIBDIR)/cxutils.1.0
LIBPATH := $(CXUTILSLIBDIR) \
			$(INSTALLDIR)/lib \
			$(JAUSDIR)/lib \
			$(CXUTILSOBJ) \
			$(EXTLIBS)
			
#---------------------------------------
#  dependencies
#---------------------------------------


#---------------------------------------
#  rules
#---------------------------------------


all: $(ARCHIVE).a $(ARCHIVE).so

$(ARCHIVE).so:
	$(CXX) -shared -o  $(DESTDIR)/$@ $(OBJECTFILES) 

$(ARCHIVE).a: $(OBJECTFILES)
	ar -ruc $(DESTDIR)/$@ $?

.cpp.o:
	@echo $(CXX) $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) -c $? -o $@
	@$(CXX) $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) -c $? -o $@
	
#depend:
#	makedepend --  $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) $(SOURCEFILES)
	
clean:
	@echo cleaning up
	@-rm -f $(foreach DIR,$(SRVSRCS), $(DIR)/*.o)
	@-rm -f $(ODIR)/*.o

allclean:
	@echo cleaning up
	@-rm -f $(foreach DIR,$(SRVSRCS), $(DIR)/*.o)
	@-rm -f $(ODIR)/*.o
	@-rm -f $(foreach DIR,$(SRVSRCS), $(DIR)/$(ARCHIVE).a)
	@-rm -f $(DESTDIR)/$(ARCHIVE).a
	@-rm -f $(foreach DIR,$(SRVSRCS), $(DIR)/$(ARCHIVE).so)
	@-rm -f $(DESTDIR)/$(ARCHIVE).so
	
install:
	cp $(DESTDIR)/$(ARCHIVE).so $(INSTALLDIR)/lib
	cp $(DESTDIR)/$(ARCHIVE).a $(INSTALLDIR)/lib
	if test -d $(INSTALLDIR)/include/jaus/services; \
	then echo exists;                \
	else mkdir $(INSTALLDIR)/include/jaus/services; \
	fi
	cp $(JAUSDIR)/include/jaus/services/*.h $(INSTALLDIR)/include/jaus/services/
	
uninstall:
	rm $(INSTALLDIR)/lib/$(ARCHIVE).so
	rm $(INSTALLDIR)/lib/$(ARCHIVE).a
	rm $(INSTALLDIR)/include/jaus/services/*.h
	rmdir $(INSTALLDIR)/include/jaus/services/	
	
help: 
	@echo $(SRVSRCS)
	@echo $@

# DO NOT DELETE


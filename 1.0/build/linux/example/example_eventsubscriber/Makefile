# make file
 
TARGET		:= example_eventsubscriber
CC 		:= gcc
CXX      	:= g++
CXXFLAGS 	:= -Wall

JAUSDIR  := ../../../..
DESTDIR  := $(JAUSDIR)/lib
ODIR     := $(JAUSDIR)/bin
CXUTILSDIR := $(JAUSDIR)/../../../cxutils/trunk/1.0

#--------------------------------------------
#  Example source files
#--------------------------------------------
SRCDIR   	:= $(JAUSDIR)/src/examples
	
#---------------------------------------
#   external includes
#---------------------------------------	
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
				$(ZLIBINC) 

#---------------------------------------
#   includes
#---------------------------------------
JAUSINC		:= $(JAUSDIR)/include
CXUTILSINC := $(CXUTILSDIR)/include
INCLUDES := $(CXUTILSINC) \
			$(JAUSINC) \
			$(EXTINCS)

#---------------------------------------
#  Objects
#---------------------------------------
CXUTILSOBJ := $(CXUTILSDIR)/build/linux
CXUTILSOBJS := $(foreach DIR,$(CXUTILSOBJ),  $(addprefix $(CXUTILSOBJ)/, $(notdir $(wildcard $(DIR)/*.o))))

#---------------------------------------
#  Libraries
#---------------------------------------
CXUTILSLIBDIR := $(CXUTILSDIR)/lib
EXTLIBDIR     := $(JAUSDIR)/ext/lib
JAUSLIBDIR		:= $(JAUSDIR)/lib
LIBPATHS := -L$(CXUTILSLIBDIR) \
			-L$(JAUSLIBDIR) \
			-L$(EXTLIBDIR) \
			-L/usr/lib

CXUTILSLIB	:= cxutils.1.0
JMSGLIB		:= jmsg.1.0
JCMPLIB		:= jcmp.1.0
JVIDLIB		:= jvid.1.0
JSRVLIB		:= jsrv.1.0
TINYLIB		:= tinyxml

LIBS		:= -l$(CXUTILSLIB) \
				-l$(JMSGLIB) \
				-l$(JCMPLIB)

#---------------------------------------
#  rules
#---------------------------------------


all: 
	$(CXX) $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) $(LIBPATHS) $(SRCDIR)/$(TARGET).cpp $(LIBS) -o $(ODIR)/$(TARGET).out
	
#depend:
#	makedepend --  $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) $(SOURCEFILES)
	
clean:
	@echo cleaning up
	@-rm -f $(ODIR)/$(TARGET).out

allclean:
	@echo cleaning up
	@-rm -f $(ODIR)/$(TARGET).out


help: 
	@echo $(VLDINC)
	@echo $@

# DO NOT DELETE


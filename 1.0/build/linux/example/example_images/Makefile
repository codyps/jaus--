# make file
 
TARGET		:= example_images
CC 		:= gcc
CXX      	:= g++
CXXFLAGS 	:= -Wall

JAUSDIR  := ../../../..
DESTDIR  := $(JAUSDIR)/lib
ODIR     := $(JAUSDIR)/bin

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
TINYXMLINC	:= $(EXTINC)/tinyxml

EXTINCS		:= $(VLDINC) \
				$(CVINC) \
				/usr/inlude

#---------------------------------------
#   includes
#---------------------------------------
JAUSINC		:= $(JAUSDIR)/include
INCLUDES := 		$(JAUSINC) \
			$(EXTINCS)


#---------------------------------------
#  Libraries
#---------------------------------------
EXTLIBDIR     := $(JAUSDIR)/ext/lib
JAUSLIBDIR		:= $(JAUSDIR)/lib

LIBPATHS := 		-L$(JAUSLIBDIR) \
			-L$(EXTLIBDIR) \
			-L/usr/local

CXUTILSLIB	:= cxutils
JMSGLIB		:= jmsg.1.0
JCMPLIB		:= jcmp.1.0
JVIDLIB		:= jvideo.1.0
JSRVLIB		:= jsrvc.1.0
TINYLIB		:= tinyxml
LIBJPG		:= jpeg
LIBPNG		:= png
LIBZ		:= z
PTHREAD		:= pthread
LIBS		:= -l$(CXUTILSLIB) \
		-l$(PTHREAD) \
		-l$(TINYLIB) \
		-l$(JMSGLIB) \
		-l$(JCMPLIB) \
                -l$(JVIDLIB) \
		-l$(JSRVLIB) \
		-l$(LIBJPG) \
		-l$(LIBPNG)

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


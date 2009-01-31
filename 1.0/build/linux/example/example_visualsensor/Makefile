# make file
 
TARGET		:= example_visualsensor
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

EXTINCS		:= -I$(VLDINC) \
			    -I$(CVINC) \
				-I$(TINYXMLINC) 

#---------------------------------------
#   includes
#---------------------------------------
JAUSINC		:= $(JAUSDIR)/include
INCLUDES := 		-I$(JAUSINC) \
			  $(EXTINCS)


#---------------------------------------
#  Libraries
#---------------------------------------
EXTLIBDIR     := $(JAUSDIR)/ext/lib
JAUSLIBDIR		:= $(JAUSDIR)/lib

LIBPATHS := 		-L$(JAUSLIBDIR) \
			-L$(EXTLIBDIR) \
			-L$(LIBJPGDIR) \
			-L/usr/lib

CXUTILSLIB	:= cxutils
JMSGLIB		:= jmsg.1.0  
JCMPLIB		:= jcmp.1.0
JVIDLIB		:= jvideo.1.0
JSRVLIB		:= jsrvc.1.0
TINYLIB		:= tinyxml
LIBJPG		:= jpeg
LIBPNG		:= png
LIBZ		:= z

LIBS		:= -l$(CXUTILSLIB) \
		        -l$(TINYLIB) \
		        -l$(JMSGLIB) \
		        -l$(JCMPLIB) \
                -l$(JVIDLIB) \
				-l$(LIBJPG) \
				-l$(LIBPNG)
                

#---------------------------------------
#  rules
#---------------------------------------


all: 
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBPATHS) $(LIBS) $(SRCDIR)/$(TARGET).cpp -o $(ODIR)/$(TARGET).out
	
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


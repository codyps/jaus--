# make file
 
ARCHIVE  	:= libjvid.1.0
CC 		:= gcc
CXX      	:= g++
CXXFLAGS 	:= -g -c -fPIC -Wall

JAUSDIR  := ../../../..
DESTDIR  := $(JAUSDIR)/lib
ODIR     := $(JAUSDIR)/obj
CXUTILSDIR := $(JAUSDIR)/../../../cxutils/trunk/1.0

#--------------------------------------------
#  project source files dirs
#--------------------------------------------
SRCDIR   	:= $(JAUSDIR)/src/jaus/video
PNGSRCDIR	:= $(SRCDIR)/png
JPEGSRCDIR	:= $(SRCDIR)/jpeg
PPMSRCDIR	:= $(SRCDIR)/ppm
PGMSRCDIR	:= $(SRCDIR)/pgm
SRCDIRS		:=  $(SRCDIR) \
		$(PNGSRCDIR) \
		$(JPEGSRCDIR) \
		$(PPMSRCDIR) \
		$(PGMSRCDIR) \
	
#---------------------------------------
#   project includes dirs
#---------------------------------------
INCDIR 		:= $(JAUSDIR)/include

#---------------------------------------
#  Source and Object files for compiling
#---------------------------------------

SOURCEFILES	:= 	$(foreach DIR,$(SRCDIRS), $(addprefix $(DIR)/, $(notdir $(wildcard $(DIR)/*.cpp))))
SOURCEFILENAMES := $(foreach DIR, $(SRCDIRS),$(notdir $(wildcard $(DIR)/*.cpp)))
OBJECTFILENAMES	:=	$(foreach DIR, $(SOURCEFILENAMES),$(addprefix $(ODIR)/,$(DIR)))
OBJECTFILES :=	$(SOURCEFILES:.cpp=.o)

#---------------------------------------
#   includes dirs
#---------------------------------------

CXUTILSINC := $(CXUTILSDIR)/include
	
EXTINC		:= $(JAUSDIR)/ext/include
VLDINC		:= $(EXTINC)/vld
CVINC		:= $(EXTINC)/cv
TINYXMLINC	:= $(EXTINC)/tinyxml
EXTINCS		:= $(VLDINC) \
				$(CVINC) \
				$(TINYXMLINC) \
				$(CXUTILSINC)

#---------------------------------------
#   objects
#---------------------------------------


#---------------------------------------
#  all Includes
#---------------------------------------

INCLUDES := $(INCDIR) $(EXTINCS)

#---------------------------------------
#  Libraries
#---------------------------------------
CXUTILSLIBDIR := $(CXUTILSDIR)/lib
EXTLIBSDIR     := $(JAUSDIR)/ext/lib

LIBS := $(CXUTILSLIBDIR)/cxutils.1.0
LIBPATH := $(CXUTILSLIBDIR) \
			/usr/lib \
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
	@-rm -f $(foreach DIR,$(SRCDIRS), $(DIR)/*.o)
	@-rm -f $(ODIR)/*.o

allclean:
	@echo cleaning up
	@-rm -f $(foreach DIR,$(SRCDIRS), $(DIR)/*.o)
	@-rm -f $(ODIR)/*.o
	@-rm -f $(foreach DIR,$(SRCDIRS), $(DIR)/$(ARCHIVE).a)
	@-rm -f $(DESTDIR)/$(ARCHIVE).a
	@-rm -f $(foreach DIR,$(SRCDIRS), $(DIR)/$(ARCHIVE).so)
	@-rm -f $(DESTDIR)/$(ARCHIVE).so

help: 
	@echo $(OBJECTFILES)
	@echo $@

# DO NOT DELETE


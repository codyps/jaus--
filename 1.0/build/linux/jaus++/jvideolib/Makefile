# make file
 
ARCHIVE  	:= libjvideo.1.0
CC 		:= gcc
CXX      	:= g++
CXXFLAGS 	:= -g -c -fPIC -Wall

JAUSDIR  := ../../../..
DESTDIR  := $(JAUSDIR)/lib
ODIR     := $(JAUSDIR)/obj
INSTALLDIR   	:= /usr

#--------------------------------------------
#  project source files dirs
#--------------------------------------------
SRCDIR   	:= $(JAUSDIR)/src/jaus/video
JPEGDIR	:= $(SRCDIR)/jpeg
PGMDIR	:= $(SRCDIR)/png
PNGDIR	:= $(SRCDIR)/ppm
PPMDIR	:= $(SRCDIR)/pgm

SRCDIRS	:=  $(SRCDIR) \
			$(JPEGDIR) \
			$(PGMDIR) \
			$(PNGDIR) \
			$(PPMDIR) \
	
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

	
EXTINC		:= $(JAUSDIR)/ext/include
VLDINC		:= $(EXTINC)/vld
CVINC		:= $(EXTINC)/cv
TINYXMLINC	:= $(EXTINC)/tinyxml
EXTINCS		:= $(VLDINC) \
				$(CVINC) \
				$(TINYXMLINC) \

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
EXTLIBSDIR     := $(JAUSDIR)/ext/lib

LIBS := cxutils
LIBPATH := 		$(INSTALLDIR)/lib \
			$(JAUSDIR)/lib \
			$(EXTLIBS)
			
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

install:
	cp -f $(DESTDIR)/$(ARCHIVE).so $(INSTALLDIR)/lib
	cp -f $(DESTDIR)/$(ARCHIVE).a $(INSTALLDIR)/lib
	if test -d $(INSTALLDIR)/include/jaus/video; \
	then echo exists;                \
	else mkdir $(INSTALLDIR)/include/jaus/video; \
	fi
	cp -rf $(JAUSDIR)/include/jaus/video/* $(INSTALLDIR)/include/jaus/video/
	
uninstall:
	rm $(INSTALLDIR)/lib/$(ARCHIVE).so
	rm $(INSTALLDIR)/lib/$(ARCHIVE).a
	rm $(INSTALLDIR)/include/jaus/video/*.h
	rm -rf $(INSTALLDIR)/include/jaus/video/	
	
help: 
	@echo $(OBJECTFILES)
	@echo $@

# DO NOT DELETE


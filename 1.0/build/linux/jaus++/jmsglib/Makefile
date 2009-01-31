# make file
 
ARCHIVE  	:= libjmsg.1.0
CC 		:= gcc
CXX      	:= g++
CXXFLAGS 	:= -g -c -fPIC -Wall

JAUSDIR  := ../../../..
DESTDIR  := $(JAUSDIR)/lib
ODIR     := $(JAUSDIR)/obj
INSTALLDIR   	:= /usr

#--------------------------------------------
#  Message source files
#--------------------------------------------
MSGSRC   	:= $(JAUSDIR)/src/jaus/messages

MSGCOMSRC	:= $(MSGSRC)/common
MSGCOMCONFSRC	:= $(MSGCOMSRC)/configuration
MSGCOMEVENSRC	:= $(MSGCOMSRC)/events
MSGCOMPLATSRC	:= $(MSGCOMSRC)/platform
MSGCOMWRLDSRC	:= $(MSGCOMSRC)/world

MSGCMDSRC	:= $(MSGSRC)/command
MSGCMDCORESRC	:= $(MSGCMDSRC)/core
MSGCMDEVENSRC	:= $(MSGCMDSRC)/events
MSGCMDPLATSRC	:= $(MSGCMDSRC)/platform
MSGCMDWRLDSRC	:= $(MSGCMDSRC)/world
MSGCMDCOMMSRC	:= $(MSGCMDSRC)/communications
MSGCMDENVISRC    := $(MSGCMDSRC)/environment

MSGINFSRC	:= $(MSGSRC)/inform
MSGINFCOMMSRC	:= $(MSGINFSRC)/communications
MSGINFCONFSRC	:= $(MSGINFSRC)/configuration	
MSGINFCORESRC	:= $(MSGINFSRC)/core
MSGINFENVISRC	:= $(MSGINFSRC)/environment
MSGINFEVENSRC	:= $(MSGINFSRC)/events
MSGINFPLATSRC	:= $(MSGINFSRC)/platform
MSGINFWRLDSRC	:= $(MSGINFSRC)/world

MSGQUESRC	:= $(MSGSRC)/query
MSGQUECOMMSRC	:= $(MSGQUESRC)/communications
MSGQUECONFSRC	:= $(MSGQUESRC)/configuration
MSGQUECORESRC	:= $(MSGQUESRC)/core
MSGQUEENVISRC	:= $(MSGQUESRC)/environment
MSGQUEEVENSRC	:= $(MSGQUESRC)/events
MSGQUEPLATSRC	:= $(MSGQUESRC)/platform

MSGEXPSRC	:= $(MSGSRC)/experimental
MSGEXPSRC_SICK	:= $(MSGSRC)/experimental/sick
MSGEXPSRC_VIDEO := $(MSGSRC)/experimental/video

MSGSRCS   	:= $(MSGSRC) \
		$(MSGCOMSRC) \
		$(MSGCOMCONFSRC) \
		$(MSGCOMEVENSRC) \
		$(MSGCOMPLATSRC) \
		$(MSGCOMWRLDSRC) \
		$(MSGCMDSRC) \
		$(MSGCMDCORESRC) \
		$(MSGCMDEVENSRC) \
		$(MSGCMDPLATSRC) \
		$(MSGCMDWRLDSRC) \
		$(MSGCMDCOMMSRC) \
		$(MSGCMDENVISRC) \
		$(MSGINFSRC) \
		$(MSGINFCOMMSRC) \
		$(MSGINFCONFSRC) \
		$(MSGINFCORESRC) \
		$(MSGINFENVISRC) \
		$(MSGINFEVENSRC) \
		$(MSGINFPLATSRC) \
		$(MSGINFWRLDSRC) \
		$(MSGQUESRC) \
		$(MSGQUECOMMSRC) \
		$(MSGQUECONFSRC) \
		$(MSGQUECORESRC) \
		$(MSGQUEENVISRC) \
		$(MSGQUEEVENSRC) \
		$(MSGQUEPLATSRC) \
		$(MSGEXPSRC) \
		$(MSGEXPSRC_SICK) \
		$(MSGEXPSRC_VIDEO)
	

#--------------------------------------------
#  Message header files
#--------------------------------------------
MSGINC 		:= $(JAUSDIR)/include
MSGINCS 	:= $(MSGINC)

#---------------------------------------
#  Rules for compiling objects
#---------------------------------------

SOURCEFILES	:= 	$(foreach DIR,$(MSGSRCS), $(addprefix $(DIR)/, $(notdir $(wildcard $(DIR)/*.cpp))))
SOURCEFILENAMES := $(foreach DIR, $(MSGSRCS),$(notdir $(wildcard $(DIR)/*.cpp)))
OBJECTFILENAMES	:=	$(foreach DIR, $(SOURCEFILENAMES),$(addprefix $(ODIR)/,$(DIR)))
OBJECTFILES :=	$(SOURCEFILES:.cpp=.o)

#---------------------------------------
#  Externals
#---------------------------------------

#---------------------------------------
#  Includes
#---------------------------------------

INCLUDES :=    $(MSGINCS)

#---------------------------------------
#  Libraries
#---------------------------------------
EXTLIBDIR     := $(JAUSDIR)/ext/lib
JAUSLIBDIR		:= $(JAUSDIR)/lib

LIBPATHS 	:= 	-L$(JAUSLIBDIR) \
			-L$(EXTLIBDIR) \
			-L$(INSTALLDIR)/lib

CXUTILSLIB	:= cxutils
LIBS		:= 

#---------------------------------------
#  rules
#---------------------------------------


all: $(ARCHIVE).a $(ARCHIVE).so
	

$(ARCHIVE).so:
	$(CXX) -shared -o $(DESTDIR)/$@ $(OBJECTFILES) $(LIBPATHS) $(LIBS) 

$(ARCHIVE).a: $(OBJECTFILES)
	ar -ruc $(DESTDIR)/$@ $?

.cpp.o:
	@echo $(CXX) $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) -c $? -o $@
	@$(CXX) $(CXXFLAGS) $(foreach INC, $(INCLUDES), -I$(INC)) -c $? -o $@
	
clean:
	@echo cleaning up
	@-rm -f $(foreach DIR,$(MSGSRCS), $(DIR)/*.o)
	@-rm -f $(ODIR)/*.o
	@-rm -f $(ODIR)/*.a

allclean:
	@echo cleaning up
	@-rm -f $(foreach DIR,$(MSGSRCS), $(DIR)/*.o)
	@-rm -f $(ODIR)/*.o
	@-rm -f $(foreach DIR,$(MSGSRCS), $(DIR)/$(ARCHIVE).a)
	@-rm -f $(DESTDIR)/$(ARCHIVE).a
	@-rm -f $(foreach DIR,$(MSGSRCS), $(DIR)/$(ARCHIVE).so)
	@-rm -f $(DESTDIR)/$(ARCHIVE).so

install:
	cp -f $(DESTDIR)/$(ARCHIVE).so $(INSTALLDIR)/lib
	cp -f $(DESTDIR)/$(ARCHIVE).a $(INSTALLDIR)/lib	
	
	if test -d $(INSTALLDIR)/include/jaus/messages; \
	then echo Directory exists...; \
	else mkdir -p $(INSTALLDIR)/include/jaus/messages; \
	fi
	cp -rf $(JAUSDIR)/include/jaus/messages/* $(INSTALLDIR)/include/jaus/messages/
	
uninstall:
	rm $(INSTALLDIR)/lib/$(ARCHIVE).so
	rm $(INSTALLDIR)/lib/$(ARCHIVE).a
	rm $(INSTALLDIR)/include/jaus/messages/*.h
	rm -rf $(INSTALLDIR)/include/jaus/messages/	
	
help: 
	@echo $(CXUTILSOBJS)
	@echo $@


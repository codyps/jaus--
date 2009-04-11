# make file
ARCHIVE  		:= libjmsg
VER  			:= 1.0
CXX      		:= g++
CXXFLAGS 		:= -c -fPIC -Wall
ROOTDIR			:= ../../../..
INCDIR 			:= $(ROOTDIR)/include
SRCDIR      	:= $(ROOTDIR)/src
DESTDIR  		:= $(ROOTDIR)/lib
INSTALLDIR   	:= /usr
ODIR			:= $(ROOTDIR)/obj/$(ARCHIVE)
LIBS			:= -lcxutils
LIBDIR			:= 
INCS			:= -I$(INCDIR) 
VPATH 			= $(SRCPATHS) 

#--------------------------------------------
#  Message source files
#--------------------------------------------
MSGSRC   	:= $(SRCDIR)/jaus/messages

MSGCOMSRC	:= $(MSGSRC)/common
MSGCOMCONFSRC	:= $(MSGCOMSRC)/configuration
MSGCOMEVENSRC	:= $(MSGCOMSRC)/events
MSGCOMPLATSRC	:= $(MSGCOMSRC)/platform
MSGCOMWRLDSRC	:= $(MSGCOMSRC)/world
MSGCOMPLANSRC	:= $(MSGCOMSRC)/planning

MSGCMDSRC	:= $(MSGSRC)/command
MSGCMDCORESRC	:= $(MSGCMDSRC)/core
MSGCMDEVENSRC	:= $(MSGCMDSRC)/events
MSGCMDPLATSRC	:= $(MSGCMDSRC)/platform
MSGCMDWRLDSRC	:= $(MSGCMDSRC)/world
MSGCMDCOMMSRC	:= $(MSGCMDSRC)/communications
MSGCMDENVISRC   := $(MSGCMDSRC)/environment
MSGCMDPLANSRC   := $(MSGCMDSRC)/planning 

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
MSGEXPSRC_DEPTH := $(MSGSRC)/experimental/waterdepth
SRCPATHS	:= $(MSGSRC) \
			$(MSGCOMSRC) \
			$(MSGCOMCONFSRC) \
			$(MSGCOMEVENSRC) \
			$(MSGCOMPLATSRC) \
			$(MSGCOMWRLDSRC) \
			$(MSGCOMPLANSRC) \
			$(MSGCMDSRC) \
			$(MSGCMDCORESRC) \
			$(MSGCMDEVENSRC) \
			$(MSGCMDPLATSRC) \
			$(MSGCMDWRLDSRC) \
			$(MSGCMDCOMMSRC) \
			$(MSGCMDENVISRC) \
			$(MSGCMDPLANSRC) \
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
			$(MSGEXPSRC_VIDEO) \
			$(MSGEXPSRC_DEPTH)
	

SRCFILES	:= 	$(foreach DIR, $(SRCPATHS), $(wildcard $(DIR)/*.cpp))
#INCFILES 	:= 	$(foreach DIR,$(INCPATHS), $(wildcard $(DIR)/*.h))
OBJFILES	:=  $(subst .cpp,.o, $(addprefix $(ODIR)/, $(notdir $(SRCFILES))))

.PHONY: help clean install clean

all: $(DESTDIR)/$(ARCHIVE).so $(DESTDIR)/$(ARCHIVE).a

%.so: $(OBJFILES)
	$(CXX) -shared -o $@ $^ $(LIBDIR) $(LIBS) 

%.a: $(OBJFILES)
	ar -rc $@ $^

$(ODIR)/%.o: %.cpp $(INCFILES) $(ODIR) 
	$(CXX) $(CXXFLAGS) -o $@ $< $(INCS) 

$(ODIR):
	if test -d $@;\
	then echo Directory exists...; \
	else mkdir -p $@; \
	fi

help:
	@echo $(OBJFILES)

clean:
	-rm -rf $(ODIR) $(DESTDIR)/$(ARCHIVE).so $(DESTDIR)/$(ARCHIVE).a

install:
	cp -f $(DESTDIR)/$(ARCHIVE).so $(INSTALLDIR)/lib/$(ARCHIVE).so.$(VER)
	cp -f $(DESTDIR)/$(ARCHIVE).a $(INSTALLDIR)/lib/$(ARCHIVE).a.$(VER)
	ln -s -f $(INSTALLDIR)/lib/$(ARCHIVE).so.$(VER) $(INSTALLDIR)/lib/$(ARCHIVE).so
	ln -s -f $(INSTALLDIR)/lib/$(ARCHIVE).so.$(VER) $(INSTALLDIR)/lib/$(ARCHIVE).a
	
uninstall:
	rm -f $(INSTALLDIR)/lib/$(ARCHIVE).so.$(VER)
	rm -f $(INSTALLDIR)/lib/$(ARCHIVE).a.$(VER)
	rm -f $(INSTALLDIR)/lib/$(ARCHIVE).so
	rm -f $(INSTALLDIR)/lib/$(ARCHIVE).a


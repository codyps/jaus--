# make file
TARGET			:= example_posesensor
CXX      		:= g++
CXXFLAGS 		:= 
ROOTDIR			:= ../../../..
INCDIR 			:= $(ROOTDIR)/include
SRCDIR      	:= $(ROOTDIR)/src
DESTDIR  		:= 
INSTALLDIR   	:= /usr
ODIR			:= $(ROOTDIR)/bin
EXTERNALS 		:= 
# standard libs -lcxutils -ljmsg -ljcmp -ljsrvc -ljvideo -ljpeg -lpng -lz
LIBS			:= -ljmsg -ljcmp -ljsrvc
LIBDIR			:= -L$(ROOTDIR)/lib
INCS			:= -I$(INCDIR) 
VPATH 			= $(SRCPATHS)


.PHONY: help clean install uninstall
SRCPATHS := $(SRCDIR)/examples

SRCFILES	:= 	$(foreach DIR, $(SRCPATHS), $(wildcard $(DIR)/$(TARGET).cpp))
OBJFILES	:=  $(subst .cpp,.out, $(addprefix $(ODIR)/, $(notdir $(SRCFILES))))

all: $(OBJFILES)

$(ODIR)/%.out: %.cpp $(ODIR) 
	$(CXX) $(CXXFLAGS) -o $@ $< $(INCS) $(LIBS) $(LIBDIR)


$(ODIR):
	if test -d $@;\
	then echo Directory exists...; \
	else mkdir -p $@; \
	fi

help:
	@echo $(OBJFILES)

clean:
	-rm -rf $(OBJFILES)


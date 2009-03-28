# make file
ARCHIVE  		:= libjcmp
VER  			:= 1.0
CXX      		:= g++
CXXFLAGS 		:= -c -fPIC -Wall
ROOTDIR			:= ../../../..
INCDIR 			:= $(ROOTDIR)/include
SRCDIR      	:= $(ROOTDIR)/src
DESTDIR  		:= $(ROOTDIR)/lib
INSTALLDIR   	:= /usr
ODIR			:= $(ROOTDIR)/obj/$(ARCHIVE)
LIBS			:=  -lcxutils -lpthread -ljmsg -ltinyxml
LIBDIR			:= -L$(ROOTDIR)/lib -L$(ROOTDIR)/ext/lib 
INCS			:= -I$(INCDIR) -I$(ROOTDIR)/ext/include/tinyxml
VPATH 			= $(SRCPATHS) 

#--------------------------------------------
#  Component source files directories
#--------------------------------------------
CMPSRC   	:= $(ROOTDIR)/src/jaus/components
CMPNODESRC  := $(CMPSRC)/node
CMPTRANSRC  := $(CMPSRC)/transport

SRCPATHS	:=  $(CMPSRC) $(CMPNODESRC) $(CMPTRANSRC)
SRCFILES	:= 	$(foreach DIR, $(SRCPATHS), $(wildcard $(DIR)/*.cpp))
#INCFILES 	:= 	$(foreach DIR,$(INCPATHS), $(wildcard $(DIR)/*.h))
OBJFILES	:=  $(subst .cpp,.o, $(addprefix $(ODIR)/, $(notdir $(SRCFILES))))

.PHONY: help clean install uninstall

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


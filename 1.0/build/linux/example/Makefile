
JAUSDIR  := ../../..
LINUXBUILDDIR := $(JAUSDIR)/build/linux

EXCOMPONENTDIR          :=$(LINUXBUILDDIR)/example/example_component
EXCONTROLTRANSFERDIR    :=$(LINUXBUILDDIR)/example/example_controltransfer
EXDISCOVERYDIR          :=$(LINUXBUILDDIR)/example/example_discovery
EXEVENTSUBSCRIBERDIR    :=$(LINUXBUILDDIR)/example/example_eventsubscriber
EXIMAGESDIR             :=$(LINUXBUILDDIR)/example/example_images
EXIMAGESUBSCRIBERDIR    :=$(LINUXBUILDDIR)/example/example_imagesubscriber
EXJOYSTICKDIR           :=$(LINUXBUILDDIR)/example/example_joystick
EXMESSAGESDIR           :=$(LINUXBUILDDIR)/example/example_messages
EXNODEMANAGERDIR        :=$(LINUXBUILDDIR)/example/example_nodemanager
EXPDCONTROLLERDIR       :=$(LINUXBUILDDIR)/example/example_pdcontroller
EXPOSESENSORDIR         :=$(LINUXBUILDDIR)/example/example_posesensor
EXPRIMITIVEDRIVERDIR    :=$(LINUXBUILDDIR)/example/example_primitivedriver
EXSCSUBSCRIBERDIR       :=$(LINUXBUILDDIR)/example/example_scsubscriber
EXVELOCITYSENSORDIR     :=$(LINUXBUILDDIR)/example/example_velocitysensor
EXVISUALSENSORDIR       :=$(LINUXBUILDDIR)/example/example_visualsensor
EXSMTESTDIR             :=$(LINUXBUILDDIR)/example/example_smtest

all:
	cd $(EXCOMPONENTDIR); $(MAKE)
	cd $(EXCONTROLTRANSFERDIR); $(MAKE)
	cd $(EXDISCOVERYDIR); $(MAKE)
	cd $(EXEVENTSUBSCRIBERDIR); $(MAKE)
	cd $(EXJOYSTICKDIR); $(MAKE)
	cd $(EXMESSAGESDIR); $(MAKE)
	cd $(EXNODEMANAGERDIR); $(MAKE)
	cd $(EXPDCONTROLLERDIR); $(MAKE)
	cd $(EXPOSESENSORDIR); $(MAKE)
	cd $(EXPRIMITIVEDRIVERDIR); $(MAKE)
	cd $(EXSCSUBSCRIBERDIR); $(MAKE)
	cd $(EXVELOCITYSENSORDIR); $(MAKE)
	cd $(EXIMAGESDIR); $(MAKE)
	cd $(EXIMAGESUBSCRIBERDIR); $(MAKE)
	cd $(EXVISUALSENSORDIR); $(MAKE)
#	cd $(EXSMTESTDIR); make


clean:
	cd $(EXCOMPONENTDIR); $(MAKE) clean
	cd $(EXCONTROLTRANSFERDIR); $(MAKE) clean
	cd $(EXDISCOVERYDIR); $(MAKE) clean
	cd $(EXEVENTSUBSCRIBERDIR); $(MAKE) clean
	cd $(EXIMAGESDIR); $(MAKE) clean
	cd $(EXIMAGESUBSCRIBERDIR); $(MAKE) clean
	cd $(EXJOYSTICKDIR); $(MAKE) clean
	cd $(EXMESSAGESDIR); $(MAKE) clean
	cd $(EXNODEMANAGERDIR); $(MAKE) clean
	cd $(EXPDCONTROLLERDIR); $(MAKE) clean
	cd $(EXPOSESENSORDIR); $(MAKE) clean
	cd $(EXPRIMITIVEDRIVERDIR); $(MAKE) clean
	cd $(EXSCSUBSCRIBERDIR); $(MAKE) clean
	cd $(EXVELOCITYSENSORDIR); $(MAKE) clean
	cd $(EXVISUALSENSORDIR); $(MAKE) clean
#	cd $(EXSMTESTDIR); make clean


	

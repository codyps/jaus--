##
## Auto Generated makefile, please do not edit
##
ProjectName:=wxnodemanager

## Release
InstallDirectory  :=/usr
ConfigurationName :=Release
IntermediateDirectory :=../../../../tmp/wxnodemanager
OutDir := $(IntermediateDirectory)
LinkerName:=g++
ArchiveTool :=ar rcus
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix :=.o
DebugSwitch :=-gstab
IncludeSwitch :=-I
LibrarySwitch :=-l
OutputSwitch :=-o 
LibraryPathSwitch :=-L
PreprocessorSwitch :=-D
SourceSwitch :=-c 
CompilerName :=g++
OutputFile :=../../../../bin/wxnodemanager
Preprocessors :=$(PreprocessorSwitch)__WX__ 
ObjectSwitch :=-o 
ArchiveOutputSwitch := 
CmpOptions := $(shell wx-config --cxxflags --unicode=yes --debug=no) $(Preprocessors)
RcCmpOptions :=$(shell wx-config --rcflags)
RcCompilerName :=windres
LinkOptions := -O2 $(shell wx-config --debug=no --libs --unicode=yes) 
IncludePath := "$(IncludeSwitch)/usr/include/wx-2.8/" "$(IncludeSwitch)/usr/include/opencv"  "$(IncludeSwitch)../../../../../../../cxutils/trunk/1.0/include" "$(IncludeSwitch)../../../../include" "$(IncludeSwitch)../../../../ext/include/tinyxml" "$(IncludeSwitch)../../../../ext/include/vld" 
RcIncludePath :=
Libs :=$(LibrarySwitch)jcmp.1.0 $(LibrarySwitch)jmsg.1.0 $(LibrarySwitch)cxutils.1.0 $(LibrarySwitch)tinyxml $(LibrarySwitch)pthread 
LibPath := "$(LibraryPathSwitch)../../../../../../../cxutils/trunk/1.0/lib" "$(LibraryPathSwitch)../../../../lib" "$(LibraryPathSwitch)../../../../ext/lib" 


Objects=$(IntermediateDirectory)/nodemanagerapp$(ObjectSuffix) $(IntermediateDirectory)/nodemanagerframe$(ObjectSuffix) 

##
## Main Build Tragets 
##
all: $(OutputFile)

$(OutputFile): makeDirStep PrePreBuild $(Objects)
	@mkdir -p $(@D)
	$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)

makeDirStep:
	@test -d ../../../../tmp/wxnodemanager || mkdir -p ../../../../tmp/wxnodemanager
PrePreBuild: 



PreBuild:


##
## Objects
##
$(IntermediateDirectory)/nodemanagerapp$(ObjectSuffix): ../../../../src/wxnodemanager/nodemanagerapp.cpp $(IntermediateDirectory)/nodemanagerapp$(ObjectSuffix).d
	@test -d ../../../../tmp/wxnodemanager || mkdir -p ../../../../tmp/wxnodemanager
	$(CompilerName) $(SourceSwitch) "../../../../src/wxnodemanager/nodemanagerapp.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/nodemanagerapp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/nodemanagerapp$(ObjectSuffix).d: ../../../../src/wxnodemanager/nodemanagerapp.cpp
	@test -d ../../../../tmp/wxnodemanager || mkdir -p ../../../../tmp/wxnodemanager
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/nodemanagerapp$(ObjectSuffix) -MF$(IntermediateDirectory)/nodemanagerapp$(ObjectSuffix).d -MM "../../../../src/wxnodemanager/nodemanagerapp.cpp"

$(IntermediateDirectory)/nodemanagerframe$(ObjectSuffix): ../../../../src/wxnodemanager/nodemanagerframe.cpp $(IntermediateDirectory)/nodemanagerframe$(ObjectSuffix).d
	@test -d ../../../../tmp/wxnodemanager || mkdir -p ../../../../tmp/wxnodemanager
	$(CompilerName) $(SourceSwitch) "../../../../src/wxnodemanager/nodemanagerframe.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/nodemanagerframe$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/nodemanagerframe$(ObjectSuffix).d: ../../../../src/wxnodemanager/nodemanagerframe.cpp
	@test -d ../../../../tmp/wxnodemanager || mkdir -p ../../../../tmp/wxnodemanager
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/nodemanagerframe$(ObjectSuffix) -MF$(IntermediateDirectory)/nodemanagerframe$(ObjectSuffix).d -MM "../../../../src/wxnodemanager/nodemanagerframe.cpp"

##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/nodemanagerapp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/nodemanagerapp$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/nodemanagerframe$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/nodemanagerframe$(ObjectSuffix).d
	$(RM) $(OutputFile)
	
-include $(IntermediateDirectory)/*.d



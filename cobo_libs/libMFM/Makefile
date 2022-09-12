
# parametrage  de la variable ARCH par lancement 

ifeq ($(INSTALLDIR),)
INSTALLDIR = ../
endif
LIBDIR        := ./

CXX           =
ObjSuf        = o
SrcSuf        = C
SrcSuf2       = cpp
SrcSuf3       = c

ExeSuf        =
DllSuf        = so
OutPutOpt     = -o

ARCH = $(shell uname -m)  
ifeq ($(ARCH),i686  )
ARCH = i686
endif
ifeq ($(ARCH),i686 )
ARCH = i686
endif
ARCH = $(shell uname -m)  
ifeq ($(ARCH),x86_64  )
ARCH = x86_64
endif
ifeq ($(ARCH),x86_64 )
ARCH = x86_64
endif

ifeq ($(ARCH),i386  )
ARCH = i686
endif
ifeq ($(ARCH),i386 )
ARCH = i686
endif

CXX	= g++ 
LD 	= g++ 
ifeq ($(COMPILER),CLANG)
CXX	= clang++	
LD 	= clang++	
endif

ifeq ($(ARCH),i686)
# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
#CXXFLAGS      = -g -O2  -fPIC -Wall
CXXFLAGS      =  -O2   -Wall -Wpadded 
#SHAREOPT      = -shared -fPIC
SHAREOPT      = -shared 
#LDFLAGS       = -g  -O   -lpthread 
LDFLAGS       =   -O 
endif

ifeq ($(ARCH),x86_64)
# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CXXFLAGS      = -g -O2  -fPIC -Wall -pthread -m64 -Wno-overloaded-virtual
#CXXFLAGS      =  -O  -fPIC -Wall
SHAREOPT      = -shared -fPIC
LDFLAGS       = -g  -O   -lpthread `root-config --glibs`

endif

ifeq ($(ARCH),macosx)  
CXXFLAGS      = -Wpadded -Wno-overloaded-virtual
SHAREOPT      = -dynamiclib -undefined dynamic_lookup 
LDFLAGS       =   
MACOSX_F := -DMACOSX
MACOSX_T := -D_SYS__SELECT_H_  -D__builtin_va_list=va_list
endif


ifeq ($(ARCH),macosx64)
CXXFLAGS      = -m64 -Wpadded -Wno-overloaded-virtual
SHAREOPT      = -dynamiclib -undefined dynamic_lookup 
LDFLAGS       = -m64 
MACOSX_F := -DMACOSX
MACOSX_T := -D_SYS__SELECT_H_  -D__builtin_va_list=va_list
endif

ifeq ($(CXX),)
$(error -$(ARCH)- invalid architecture)
endif

LOCAL_FLAGS   =  

ROOTCFLAGS    = `root-config --cflags`

INCFLAGS      =  $(ROOTCFLAGS) $(LOCAL_FLAGS) $(GANTAFLAGS) $(NETFLAGS)  $(CALIFLAGS) $(GRUCOREFLAGS) $(MACOSX_F) $(FASTERFLAG)
CXXFLAGS     +=  $(INCFLAGS)

ifeq ($(SITE),CCIN2P3)
CXXFLAGS     += -D CC_IN2P3
endif

MFMLIB         = libMFM.a

#------------------------------------------------------------------------------------------------

MFMLIB_O   =  MFMCommonFrame.o  MFMExogamFrame.o  MFMCoboFrame.o    MFMBasicFrame.o \
			  MFMEbyedatFrame.o MFMBlobFrame.o    MFMOscilloFrame.o MFMMergeFrame.o  \
			  MFMXmlDataDescriptionFrame.o MFMScalerDataFrame.o MFMXmlFileHeaderFrame.o \
			  MFMMutantFrame.o MFMRibfFrame.o DataParameters.o  MError.o

MFMLIB_C   =  MFMCommonFrame.cc  MFMExogamFrame.cc MFMCoboFrame.cc   MFMBasicFrame.cc \
			  MFMEbyedatFrame.cc MFMBlobFrame.cc   MFMOscilloFrame.cc MFMMergeFrame.cc \
			  MFMXmlDataDescriptionFrame.cc MFMScalerDataFrame.cc MFMXmlFileHeaderFrame.cc\
			  MFMMutantFrame.cc MFMRibfFrame.cc DataParameters.cc MError.cc

MFMLIB_H   =  MFMCommonFrame.h   MFMExgamFrame.h MFMCoboFrame.h MFMBasicFrame.h \
			  MFMEbyedatFrame.h MFMBlobFrame.h MFMOscilloFrame.h \
			  MFMXmlDataDescriptionFrame.h MFMScalerDataFrame.h MFMXmlFileHeaderFrame.h\
              MFMMutantFrame.h MFMRibfFrame.h DataParameters.h   DataPar.h MError.h

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:	clean MFMshared 


MFMshared:   	$(MFMLIB_O) 
		$(CXX) $(MFMLIB_O) -shared -o libMFM.so  
		@echo "$@ done"
		
		
clean :  
		@rm -rf *.o *.a *.so *.xml html latex   *.exe mfmfile.dat

info:   
		@echo "--------ARCH------------"
		@echo $(ARCH)     
		@echo "--------MFMLIB_O------------"
		@echo $(MFMLIB_O)
		@echo "-------CXX------------"
		@echo $(CXX)
		@echo "---------------------------------"

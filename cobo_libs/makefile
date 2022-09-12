#!/bin/tcsh
###########################################################################
# 
#            ***    ***   ***      ***   ***   ***      *** 
#            ***    ***   ****     ***   ***    ***    *** 
#            ***    ***   *****    ***           ***  *** 
#            ***    ***   ******   ***   ***      ****** 
#            ***    ***   *** ***  ***   ***       **** 
#            ***    ***   ***  *** ***   ***       **** 
#            ***    ***   ***   ******   ***      ******  
#            ***    ***   ***    *****   ***     ***  *** 
#             *********   ***     ****   ***    ***    *** 
#               *******   ***      ***   ***   ***      *** 
#
############################################################################ 


DEBUG = -g             
COMPILAT = g++
LINK = g++

RACINE = $(PWD)/
LIBDIR = $(RACINE)lib/
EXEDIR = $(RACINE)
SRCDIR = $(RACINE)src/
INCDIR = $(RACINE)inc/
MFMDIR = $(RACINE)libMFM/


CC = $(COMPILAT) $(CFLAGS) -O3 `root-config --cflags` -Wno-write-strings -Wno-deprecated -fpermissive -std=c++17 -c -I$(INCDIR)  -I$(MFMDIR) -o  $@ -fopenmp
Cso= $(COMPILAT) $(CFLAGS) -O3 `root-config --cflags` -Wno-write-strings -Wno-deprecated -fpermissive -std=c++17 -I$(INCDIR) -I$(MFMDIR) -o  $@
LD = $(LINK) -o $@ -lm -lpthread
LD_EXT_LIBS = `root-config --glibs` -L$(MFMDIR) -lSpectrum -ltinyxml -lgomp 

ifeq ($(SITE),CCIN2P3)
CFLAGS     += -D CC_IN2P3
endif


OBJETS = 	$(LIBDIR)Analyse.o \
		$(LIBDIR)MUnpacker.o \
		$(LIBDIR)MExperiment.o \
		$(LIBDIR)MVisu.o \
		$(LIBDIR)MFile.o \
		$(LIBDIR)MEvent.o \
		$(LIBDIR)MCoboAsad.o \
		$(LIBDIR)MChannel.o \
		$(LIBDIR)TreatBaseline.o \
		$(LIBDIR)MyDict.o 
		
SOURCES = $(SRCDIR)Analyse.cpp \
		$(SRCDIR)MChannel.cpp \
		$(SRCDIR)MCoboAsad.cpp \
		$(SRCDIR)MEvent.cpp \
		$(SRCDIR)TreatBaseline.cpp \
		$(SRCDIR)MExperiment.cpp \
		$(SRCDIR)MFile.cpp \
		$(SRCDIR)MUnpacker.cpp \
		$(SRCDIR)MVisu.cpp \
		$(SRCDIR)MyDict.C	
		
		
all :  	$(EXEDIR)Analyse.exe $(LIBDIR)libACTARshared\


$(EXEDIR)Analyse.exe :	$(OBJETS)
	$(LD) $(DEBUG) $(OBJETS) $(MFMDIR)libMFM.so $(LD_EXT_LIBS)
	
$(LIBDIR)Analyse.o :	$(SRCDIR)Analyse.cpp \
				$(INCDIR)MExperiment.h 
	$(CC) $(DEBUG) $(SRCDIR)Analyse.cpp 


$(LIBDIR)MUnpacker.o :	$(SRCDIR)MUnpacker.cpp \
				$(INCDIR)MUnpacker.h \
				$(INCDIR)MEvent.h 
	$(CC) $(DEBUG) $(SRCDIR)MUnpacker.cpp


$(LIBDIR)MExperiment.o :	$(SRCDIR)MExperiment.cpp \
				$(INCDIR)MExperiment.h \
				$(INCDIR)MUnpacker.h \
				$(INCDIR)MFile.h \
				$(INCDIR)MEvent.h \
				$(INCDIR)MVisu.h 
	$(CC) $(DEBUG) $(SRCDIR)MExperiment.cpp

	
$(LIBDIR)MVisu.o :	$(SRCDIR)MVisu.cpp \
				$(INCDIR)MVisu.h 
	$(CC) $(DEBUG) $(SRCDIR)MVisu.cpp


$(LIBDIR)MFile.o :	$(SRCDIR)MFile.cpp \
				$(INCDIR)MFile.h 
	$(CC) $(DEBUG) $(SRCDIR)MFile.cpp
	

$(LIBDIR)MEvent.o :	$(SRCDIR)MEvent.cpp \
				$(INCDIR)MEvent.h \
				$(INCDIR)MEventReduced.h \
				$(INCDIR)MCoboAsad.h 
	$(CC) $(DEBUG) $(SRCDIR)MEvent.cpp


$(LIBDIR)TreatBaseline.o : $(SRCDIR)TreatBaseline.cpp \
				$(INCDIR)MEvent.h 
	$(CC) $(DEBUG) $(SRCDIR)TreatBaseline.cpp


$(LIBDIR)MCoboAsad.o :	$(SRCDIR)MCoboAsad.cpp \
				$(INCDIR)MCoboAsad.h \
				$(INCDIR)MChannel.h 
	$(CC) $(DEBUG) $(SRCDIR)MCoboAsad.cpp


$(LIBDIR)MChannel.o :	$(SRCDIR)MChannel.cpp \
				$(INCDIR)MChannel.h \
				$(INCDIR)Parameters.h 
	$(CC) $(DEBUG) $(SRCDIR)MChannel.cpp


$(LIBDIR)MyDict.o :	$(SRCDIR)MyDict.C 	
	$(CC) $(DEBUG) $(SRCDIR)MyDict.C	


$(LIBDIR)libACTARshared:  
		$(Cso)  $(DEBUG) -shared -o libACTARshared.so -fPIC $(SOURCES)
		@echo "$@ done"

.PHONY : clean
clean :
		@rm $(OBJETS) $(SRCDIR)MyDict.C $(SRCDIR)MyDict.h 2>/dev/null

$(SRCDIR)MyDict.C: $(INCDIR)MEvent.h 
	@echo "Generating dictionary ..."
	@rootcint -f $(SRCDIR)MyDict.C -c $(INCDIR)MEventReduced.h $(INCDIR)MTreeStructureLinkDef.h

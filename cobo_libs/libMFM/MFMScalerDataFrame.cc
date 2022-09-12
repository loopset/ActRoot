/*
 MFMScalerDataFrame.cc
 Copyright Acquisition group, GANIL Caen, France
 */

#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <tinyxml.h>
using namespace std;

#include "MFMScalerDataFrame.h"

//_______________________________________________________________________________
MFMScalerDataFrame::MFMScalerDataFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize,
		int itemSize, int nItems) {
	/// Constructor of frame with a memory space\n
	/// fill header information : unitBlock_size,dataSource,....
	SetPointers();
}

//_______________________________________________________________________________
MFMScalerDataFrame::MFMScalerDataFrame() {
	/// Constructor of a empty frame object
}
//_______________________________________________________________________________
MFMScalerDataFrame::~MFMScalerDataFrame() {
	///Destructor
}
//_______________________________________________________________________________
void MFMScalerDataFrame::SetBufferSize(int size, bool ifinferior) {
	/// Do memory allocation or a reallacation for frame\n
	/// if ifinferior==true the allocaton is forced to size event if the acutal size is bigger\n
	MFMBasicFrame::SetBufferSize(size, ifinferior);
	MFMScalerDataFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMScalerDataFrame::SetPointers(void * pt) {
	/// Initialize pointers of frame\n
	/// if pt==NULL initialization is with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt\n
	/// pData must be the reference;
	MFMBasicFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMScalerDataFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBasicFrame::SetAttributs(pt);

}
//_______________________________________________________________________________
string MFMScalerDataFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss;
	string display("");
	display = ss.str();
	ss << MFMBasicFrame::GetHeaderDisplay(infotext);
	ss << "   TS = " << GetTimeStamp();ss << "   EN = " << GetEventNumber();
	ss << endl;
	display = ss.str();
	return display;
}

//_______________________________________________________________________________
int MFMScalerDataFrame::GetEventNumber() {
	/// Compute and return scaker number
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);
	fEventNumber
			= ((MFM_ScalerData_header*) pHeader)->ScalerData_Info.eventIdx;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (eventNumber), 4);

	return fEventNumber;
}
//_______________________________________________________________________________
int MFMScalerDataFrame::GetEventNumberAttibut() {
	/// Return scaker number without computing it
	return fEventNumber;
}

//_______________________________________________________________________________
void MFMScalerDataFrame::SetEventNumber(uint32_t eventnumber) {
	/// set frame scaker number
	((MFM_ScalerData_header*) pHeader)->ScalerData_Info.eventIdx
			= eventnumber;
}
//_______________________________________________________________________________

uint64_t MFMScalerDataFrame::GetTimeStamp() {
	// Compute and return Time Stamp
	fTimeStamp = 0;
	uint64_t * timeStamp = &(fTimeStamp);
	memcpy(((char*) (&fTimeStamp)),
			((MFM_ScalerData_header*) pHeader)->ScalerData_Info.eventTime, 6);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt64((timeStamp), 6);
	return fTimeStamp;
}
//_______________________________________________________________________________
uint64_t MFMScalerDataFrame::GetTimeStampAttribut() {
	// return Time Stamp attribut without computing
	return fTimeStamp;
}
//_______________________________________________________________________________
void MFMScalerDataFrame::SetTimeStamp(uint64_t timestamp) {
	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	memcpy(((MFM_ScalerData_header*) pHeader)->ScalerData_Info.eventTime, pts, 6);
}
//_______________________________________________________________________________
void MFMScalerDataFrame::GetValues(int i, uint32_t* label, uint64_t* count,
		uint64_t* frequency, int32_t* status, uint64_t* tics, int32_t* acqstatus) {
	/// Compute and return the couple information of label /value of the i-th item
	GetValuesByItem((MFM_ScalerData_Item *) GetItem(i), label, count,
			frequency, status, tics, acqstatus);
}

//_______________________________________________________________________________
void MFMScalerDataFrame::GetValuesByItem(MFM_ScalerData_Item *item,
		uint32_t * label, uint64_t *count, uint64_t *frequency,
		int32_t * status, uint64_t * tics,int32_t * acqstatus ) {
	/// Compute and return the couple information of label /value of  item

	if (fLocalIsBigEndian != fFrameIsBigEndian) {

		uint32_t tmp32;
		uint64_t tmp64;

		tmp32 = item->Label;
		SwapInt32(&tmp32);
		*label = tmp32;
		tmp64 = item->Count;
		SwapInt64(&tmp64);
		*count = tmp64;
		tmp64 = item->Frequency;
		SwapInt64(&tmp64);
		*frequency = tmp64;
		tmp32 = item->Status;
		SwapInt32(&tmp32);
		*status = tmp32;
		tmp64 = item->Tics;
		SwapInt64(&tmp64);
		*tics = tmp64;
		tmp32 = item->AcqStatus;
		SwapInt32(&tmp32);
		*acqstatus = tmp32;
	} else {
		*label = item->Label;
		*count = item->Count;
		*frequency = item->Frequency;
		*status = item->Status;
		*tics = item->Tics;
		*acqstatus = item->AcqStatus;
	}
}

//_______________________________________________________________________________
void MFMScalerDataFrame::SetValues(int i, uint32_t label, uint64_t count,
		uint64_t frequency, int32_t status, uint64_t tics,int32_t acqstatus) {
	/// set i-th item if frame with the couple information of label /value
	SetValuesByItem((MFM_ScalerData_Item *) GetItem(i), label, count,
			frequency, status, tics,acqstatus);
}

//_______________________________________________________________________________
void MFMScalerDataFrame::SetValuesByItem(MFM_ScalerData_Item *item,
		uint32_t label, uint64_t count, uint64_t frequency, int32_t status,
		uint64_t tics,int32_t acqstatus) {
	/// set "item" with the couple information of label /value
	item->Label = label;
	item->Count = count;
	item->Frequency = frequency;
	item->Status = status;
	item->Tics = tics;
	item->AcqStatus = acqstatus;
}

//_______________________________________________________________________________
void MFMScalerDataFrame::FillScalerWithRamdomConst(int nbitem,uint64_t timestamp,uint32_t enventnumber) {
	/// Fill frame items with ramdom values
	   int hsize = SCALER_DATA_HEADERSIZE;
		int oldframesize = GetFrameSize();
	  	int ubs= SCALER_STD_UNIT_BLOCK_SIZE;
	  	int framesize_up =0;
		int framesize_min = nbitem*sizeof(MFM_ScalerData_Item)+hsize ;

		if (framesize_min%ubs ==0) framesize_up= framesize_min;
		else framesize_up = ((framesize_min /ubs)+1)*ubs;
	    if (oldframesize != framesize_up){
		//TODO reagencement de la frame si oldframesize != framesize

	    	SetFrameSize(framesize_up/ubs);
	    	SetBufferSize(framesize_up,false);
	    	SetAttributs();
	    	 std::cout << "ReSizing!------------- " << framesize_up <<"\n";
	    }
	 std::cout << "SCALER_DATA_HEADERSIZE " << hsize <<"\n";
	 SetNbItem((int)nbitem);
	 std::cout << "SetNbItem " << nbitem <<"\n";
	 std::cout << "GetNbItems " << GetNbItems() <<"\n";
	float randval;
	uint16_t i = 0;
	if (GetNbItems() > 0)
		SetValues(0, 1, 1, 1, 1, 1, 1);
	for (i = 1; i < GetNbItems(); i++) {
		 //std::cout << "SetNbItemi " << i <<"\n";
		randval = random();
		uint16_t uivalue = (uint16_t) (65536 * randval / RAND_MAX);
		SetValues(i, i + 1, uivalue, uivalue, 1, uivalue,1);
	}
	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
}
//_______________________________________________________________________________
void MFMScalerDataFrame::FillScalerWithVector(uint64_t timestamp,uint32_t EventCounter,int64_t *fVector,int sizeofvector){

	int nbOfParaInItem =6;
	uint32_t Label;
	uint64_t Count;
	uint64_t Frequency;
	int32_t Status;
	uint64_t Tics;
	int32_t acqStatus;
	uint16_t i = 0;
	uint32_t nbitem=0;

	if (sizeofvector%nbOfParaInItem !=0) cout <<" error de taille de vecteur\n";
	int oldframesize = GetFrameSizeAttribut();
	 nbitem = (uint32_t)(sizeofvector/nbOfParaInItem);
	int framesize = nbitem*sizeof(MFM_ScalerData_Item)+SCALER_DATA_HEADERSIZE ;
;
    if (oldframesize != framesize){
	//TODO reagencement de la frame si oldframesize != framesize

    	int ubs= SCALER_STD_UNIT_BLOCK_SIZE;
    	if (framesize%ubs==0)
    		framesize=framesize/ubs ;
    	else
    		framesize=framesize/ubs +1;
    	SetFrameSize(framesize);
    	SetBufferSize(framesize*ubs,false);
    	SetAttributs();
    }
    SetNbItem((int)nbitem);
    int64_t value;
	for (i = 0; i < GetNbItems(); i++) {
			value = fVector[i*nbOfParaInItem];
			Label = (uint32_t)  value;
			value =fVector[i*nbOfParaInItem+1];
			Count = (uint64_t )value;
			value =fVector[i*nbOfParaInItem+2];
			Frequency= (uint64_t)value;
			value =fVector[i*nbOfParaInItem+3];
			Status = (int32_t)value;
			value =fVector[i*nbOfParaInItem+4];
			Tics = (uint64_t)value;
			value =fVector[i*nbOfParaInItem+5];
			acqStatus = (int32_t)value;

			SetValues(i, Label, Count, Frequency, Status,Tics,acqStatus);
		}
	SetEventNumber(EventCounter);
	SetTimeStamp(timestamp);

}
//_______________________________________________________________________________

void MFMScalerDataFrame::GenerateAScalerExample(uint64_t timestamp,uint32_t eventnumber,int nbChannels) {
	/// Generate a example of frame containing random value\n
	/// usable for tests.

	uint32_t unitBlock_size = 0;
	uint32_t itemsize = 0;
	uint32_t nbitem = 0;
	uint32_t framesize = 0;
	uint32_t revision = 1;
	uint32_t headersize = 0;
	uint16_t source = 0xff; // standard value when produced by a desktop computer
	unitBlock_size = SCALER_STD_UNIT_BLOCK_SIZE;

	itemsize = sizeof(MFM_ScalerData_Item);
	nbitem = nbChannels;
	headersize = SCALER_DATA_HEADERSIZE;
	uint16_t type = MFM_SCALER_DATA_FRAME_TYPE;

	framesize = headersize + nbitem * itemsize;
	revision = 1;
	// generation of MFM header , in this case, MFM is same for all MFM frames
	MFM_make_header(unitBlock_size, source, type, revision, (int) (framesize
			/ unitBlock_size), (headersize / unitBlock_size), itemsize, nbitem);
	FillScalerWithRamdomConst(nbitem,timestamp,eventnumber);
}
//____________________________________________________________________________
string MFMScalerDataFrame::GetDumpTextData() {
stringstream ss;
	string display("");
	ss << "|label|  count   | frequency|statu|   tics   |statu| \n";
			display = ss.str();
			return display;
	}
//____________________________________________________________________________
void MFMScalerDataFrame::DumpTextData() {
	cout<<GetDumpTextData();
}

//____________________________________________________________________________
void MFMScalerDataFrame::DumpData(char mode, bool nozero) {
	cout<<GetDumpData(mode, nozero);
}
//____________________________________________________________________________
string MFMScalerDataFrame::GetDumpData(char mode, bool nozero) {
	// Dump parameter Label and parameter value of the current event.
	// if enter parameter is true (default value), all zero parameter of event aren't dumped
	// mode = 'd' for decimal, 'b' for binary, 'h' for hexa, 'o' for octal

	stringstream ss;
	string display("");

	int i, j, maxbin, presentation = 0, max_presentation = 1;
	char tempo[255];
	char Bin[255];
	char Bin2[255];
	uint64_t count;
	uint32_t label;
	int32_t status;
	uint64_t frequency;
	uint64_t tics;
	int32_t acqstatus;
	int reste;
	char one = '1', zero = '0';
	int DecNumber = 0;
	if (mode == 'b')
		max_presentation = 3;
	if (GetEventNumber() == -1) {
		ss << "Nb of Event <0 , so no event dump. Get a new event frame";
	} else {
		for (i = 0; i < GetNbItems(); i++) {
			label = 0;
			count = 0;
			status = 0;
			frequency = 0;
			tics = 0;
			GetValues(i, &label, &count, &frequency, &status, &tics, &acqstatus);
			if ((count != 0xFFFF)) {
				switch (mode) {
				case 'd':// decimal display
					sprintf(tempo, "|%5d|%10ld|%10ld|%5d|%10ld|%5d", label, count,
							frequency, status, tics, acqstatus);
					break;
				case 'o':// octal display
					sprintf(tempo, "|%5d|%10ld|%10ld|%5d|%10ld|%5d", label, count,
							frequency, status, tics, acqstatus);
					break;
				case 'h':// hexa display
					sprintf(tempo, "|%5d|%10ld|%10ld|%5d|%10ld|%5d", label, count,
							frequency, status, tics, acqstatus);
					break;
				case 'b': // binary display
					DecNumber = (int) count;
					Bin[0] = '\0';
					Bin[1] = zero;
					Bin2[1] = '\0';
					Bin2[0] = zero;
					j = 1;
					if (DecNumber != 0) {
						while (DecNumber >= 1) {
							reste = DecNumber % 2;
							DecNumber -= reste;
							DecNumber /= 2;
							if (reste)
								Bin[j] = one;
							else
								Bin[j] = zero;
							j++;
						}
						j--;
						maxbin = j;
						while (j >= 0) {
							Bin2[j] = Bin[maxbin - j];
							j--;
						}
					}
					sprintf(tempo, "|%5d = %16s", label, Bin2);
					break;
				}
				ss << tempo;
				presentation++;
			}
			if (presentation == max_presentation) {
				ss << "|\n";
				presentation = 0;
			}
		}
		if (presentation != 0)
			ss << "|\n";
	}
	display = ss.str();
	return display;
}
//____________________________________________________________________________
//______________________________________________________________________________
/*
class GParaCaliXml :public GBase{

 private:

  TDatime *my_time;//!
  TXMLEngine* fXmlFile; //!

  XMLNodePointer_t fXmlMain; //!
  XMLDocPointer_t  fXmlDoc;//!

 protected:

  Int_t fStatus; // Internal status ( to have a satus whithout a returned status which noise the standard output of Cint )
  Float_t fVersion;// version of this class


 public:

  TString fFileName; // Xml File Name



  // liste of methods
}
ClassImp(GParaCaliXml);
//______________________________________________________________________________
GParaCaliXml::GParaCaliXml() {

	// Constructor/initialisator of Acquisition object
	TString tempo;


	fVersion =1.2;

	gROOT->cd();
	fVerbose = 0;

	my_time = new TDatime();

	fFileName.Form("%s/calimero/", gSystem->HomeDirectory());
	if (gSystem->OpenDirectory(fFileName.Data())==0)
		fFileName.Form("%s/", gSystem->HomeDirectory());

	tempo.Form("Cali_Coefficient_%d_%d.xml", (int)my_time->GetDate(),
			(int)(my_time->GetTime()));
	fFileName.Append(tempo);
	fXmlFile =NULL;fXmlMain=NULL;fXmlDoc=NULL;
}

//_____________________________________________________________________________

GParaCaliXml::~GParaCaliXml() {
	gROOT->cd();
	///if (fWorkingDirectory) {
		//delete [] fWorkingDirectory;
		//fWorkingDirectory=NULL;
	}
	if (my_time) {
		delete my_time;
		my_time =NULL;
	}
	if (fXmlFile)
		delete fXmlFile;
	fXmlFile =NULL;

}

//______________________________________________________________

void MFMScalerDataFrame::InitXml(bool write_or_read, TString commentaire) {
	// Initialisation , Create an XML file or to read a xml file
	// if write_or_read = true  => write configuration => commentaire is a comment in the header of file
	// if write_or_read = false => read  configuration => commentaire is the name file to open
	// for matacq xml file have this format.
	//<?xml version="1.0"?>

	string tempos,tempos2,entete;
	if (write_or_read) {
		fError.Infos( "Init xml  file");
		//balise entete
		tempos="";
		entete = "Scaler information";

		// First create engine
		fXmlFile= new TXMLEngine;
		// Create main node of document tree
		fXmlMain = fXmlFile->NewChild(0, 0, "root");

		XMLNodePointer_t child = fXmlFile->NewChild(fXmlMain, 0, "Entete", "");
		tempos +=(Float_t)fVersion;

		fXmlFile->NewChild(child, 0, "Version", tempos);
		tempos2="";
		tempos2.Form("%d  %d ", (int)my_time->GetDate(),
				(int)(my_time->GetTime()));
		tempos.Form (" %s %s ",commentaire.Data(),tempos2.Data());
		fError.TreatError(0,0,tempos);
		fXmlFile->NewChild(child, 0, "Date", tempos2);
		fXmlFile->NewChild(child, 0, "Note", commentaire);

	} else {

		// Read file
		fXmlFile = new TXMLEngine;
		fFileName=commentaire;
		tempos.Form ("Read Calibration Coefficient file %s ",commentaire.Data());
		fError.TreatError(0,0,tempos);
		fXmlDoc = fXmlFile->ParseFile(commentaire.Data());
		if (fXmlDoc==0) {
			fError.TreatError(2, -1, "erreur de lecture du fichier xml");
			delete fXmlFile;
			fXmlFile =NULL;
			return;
		}

		// take access to main node
		fXmlMain = fXmlFile->DocGetRootElement(fXmlDoc);
		XMLNodePointer_t XmlEntete  = fXmlFile->GetChild(fXmlMain);
		XMLNodePointer_t XmlVersion = fXmlFile->GetChild(XmlEntete);
		XMLNodePointer_t XmlDate    = fXmlFile->GetNext(XmlVersion);
		XMLNodePointer_t XmlNote    = fXmlFile->GetNext(XmlDate);

		TString Version = fXmlFile->GetNodeContent(XmlVersion);
		TString Date    = fXmlFile->GetNodeContent(XmlDate);
		TString Note    = fXmlFile->GetNodeContent(XmlNote);

       tempos.Form ("Version : %s  Date : %s Note : %s",Version.Data(),Date.Data(),Note.Data());
       fError.TreatError(0,0,tempos);

	  // fXmlFile->FreeDoc(fXmlDoc);

	}
}

//______________________________________________________________
void GParaCaliXml::CloseXml() {
	XMLDocPointer_t fXmlDoc = fXmlFile->NewDoc();
	fXmlFile->DocSetRootElement(fXmlDoc, fXmlMain);
	fXmlFile->SaveDoc(fXmlDoc, fFileName.Data(), 2);
	// Release memory before exit
	fXmlFile->FreeDoc(fXmlDoc);
}

//______________________________________________________________
void GParaCaliXml::SaveParameter(int no, const char* ParaName, int nb_values,
		float* Values_Gene, float Etalon, float Gene_a1, float Gene_a2,
		float Gene_a3, int Mate, int voie, int telescope, float* pics,
		int nbpics) {
	SaveParameter(&no, ParaName, &nb_values, Values_Gene, &Etalon, &Gene_a1,
			&Gene_a2, &Gene_a3, &Mate, &voie, &telescope, pics, &nbpics);
}
//______________________________________________________________
void GParaCaliXml::SaveParameter(int* no, const char* ParaName, int* nb_values,
		float* Values_Gene, float* Etalon, float* Gene_a1, float* Gene_a2,
		float* Gene_a3, int* Mate, int* voie, int* telescope, float* pics,
		int *nbpics) {
	//save Parameter in a XML file
	TString tempo;
	char tempa[MAX_CARACTERES];
	strcpy(tempa, "");
	tempo= "";
	XMLNodePointer_t child = fXmlFile->NewChild(fXmlMain, 0, "Valeur", "");
	tempo= "";
	sprintf(tempa, " %d  %s", *no, ParaName);
	tempo = tempa;
	XMLNodePointer_t child2 = fXmlFile->NewChild(child, 0, "Entree ", tempo);

	tempo= "";
	tempo += *Etalon;
	fXmlFile->NewChild(child2, 0, "Etalon", tempo);
	tempo= "";
	tempo += *Gene_a1;
	fXmlFile->NewChild(child2, 0, "Gene_a1", tempo);
	tempo= "";
	tempo += *Gene_a2;
	fXmlFile->NewChild(child2, 0, "Gene_a2", tempo);
	tempo= "";
	tempo += *Gene_a3;
	fXmlFile->NewChild(child2, 0, "Gene_a3", tempo);
	tempo= "";
	tempo += *Mate;
	fXmlFile->NewChild(child2, 0, "Mate", tempo);
	tempo= "";
	tempo += *voie;
	fXmlFile->NewChild(child2, 0, "Voie", tempo);
	tempo= "";
	tempo += *telescope;
	fXmlFile->NewChild(child2, 0, "Telescope", tempo);

	tempo= "";
	strcpy(tempa, "");
	sprintf(tempa, " %d ", *nbpics);
	tempo += tempa;
	for (int i=0; i<*nbpics; i++) {
		sprintf(tempa, " %f ", pics[i]);
		tempo += tempa;
	}
	fXmlFile->NewChild(child2, 0, "Pics", tempo);

	tempo= "";
	strcpy(tempa, "");
	sprintf(tempa, " %d ", *nb_values);
	tempo += tempa;
	for (int i=0; i<*nb_values; i++) {
		sprintf(tempa, " %f ", Values_Gene[i]);
		tempo += tempa;
	}

	fXmlFile->NewChild(child2, 0, "Values gene", tempo);
}

//______________________________________________________________

void GParaCaliXml::GetParameter(int* no, int* Piedestal, float* Etalon,
		float* Gene_a1, float* Gene_a2, float* Gene_a3, int* Mate, int* voie,
		int* telescopes) {
	// get calibration coefficients from XML file.

	// to finish

}

//______________________________________________________________

void GParaCaliXml::UpdateFromXML(TXMLEngine* xml, XMLNodePointer_t node) {//
	XMLNodePointer_t child= xml->GetChild(node);
	TString name = xml->GetNodeContent(child);
	child = xml->GetNext(child);
	TString source = xml->GetNodeContent(child);
	child = xml->GetNext(child);
	TString sourceType = xml->GetNodeContent(child);
	child = xml->GetNext(child);
	TString sourceName = xml->GetNodeContent(child);
	child = xml->GetNext(child);
	TString family = xml->GetNodeContent(child);

}
//______________________________________________________________

void GParaCaliXml::SaveCoefMatacq(TH1F** Etalonnage, Float_t* DeltaT,
		Float_t* VernierMin, Float_t* VernierMax, Int_t nb, Int_t longchannel) {
	//save Parameter in a XML file
	// Array of histograms containning coefficients
	//nb : nb channel
	//longchannel long of channel

	TString tempo;
	char tempa[MAX_CARACTERES];
	//XMLNodePointer_t child2;
	for (int i = 0; i< nb; i++) {
		strcpy(tempa, "");
		tempo= "";
		sprintf(tempa, " %d ", i);
		tempo=tempa;
		XMLNodePointer_t child = fXmlFile->NewChild(fXmlMain, 0, "Channel",
				tempa);
		tempo.Form("%f ", DeltaT[i]);
		//child2 = fXmlFile->NewChild(child, 0, "DetaT", tempo);
		fXmlFile->NewChild(child, 0, "DetaT", tempo);
		tempo.Form("%f ", VernierMin[i]);
		fXmlFile->NewChild(child, 0, "VernierMin", tempo);
		tempo.Form("%f ", VernierMax[i]);
		fXmlFile->NewChild(child, 0, "VernierMax", tempo);
		for (int j = 0; j< longchannel; j++) {
			tempo.Form(" %d   %f", j, Etalonnage[i]->GetBinContent(j+1));
			fXmlFile->NewChild(child, 0, "Sample", tempo);
		}

	}
}

//______________________________________________________________

void GParaCaliXml::GetCoefMatacq(TH1F** Etalonnage, Float_t* DeltaT,
		Float_t* VernierMin, Float_t* VernierMax, Int_t nb, Int_t longchannel) {
	//Get calibration coefficients and put them in  histogram  ' Etalonnage'

	TString Content, NodeName, copyContent,tempos;
	int channeli, comptage, comptagechannel, nblecturenormale, nosample, index;
	Float_t etalon;
	XMLNodePointer_t sample;
	if (fXmlFile==NULL) {
		fError.TreatError(2,0,"GParaCaliXml::GetCoefMatacq : No XML file");
		return;
	}
	XMLNodePointer_t channel= fXmlFile->GetChild(fXmlMain);
	nblecturenormale=longchannel;
	comptage=0;
	comptagechannel=0;
	channeli=0;

	channel = fXmlFile->GetNext(channel);

	while (channel>0) {
			comptage=0;

			nblecturenormale=longchannel;
			NodeName = fXmlFile->GetNodeName(channel);
			Content = fXmlFile->GetNodeContent(channel);
			comptagechannel++;
			if (NodeName== "Channel") {
				channeli =Content.Atoi();
			} else {
				tempos.Form("Not a channel, Node Name = %s and content = %s",NodeName.Data(),Content.Data());
				fError.TreatError(2, 0, tempos);
			}

			sample = fXmlFile->GetChild(channel);

			while (sample>0) {
				NodeName = fXmlFile->GetNodeName(sample);
				Content  = fXmlFile->GetNodeContent(sample);

				Content.Remove(TString::kBoth, ' ');
				if (NodeName== "DetaT") {
					nblecturenormale++;
					comptage++;
					DeltaT[channeli]= (Float_t)(Content.Atof());
				}

				if (NodeName== "VernierMin") {
					nblecturenormale++;
					comptage++;
					VernierMin[channeli]= (Float_t)(Content.Atof());
				}

				if (NodeName== "VernierMax") {
					nblecturenormale++;
					comptage++;
					VernierMax[channeli]= (Float_t)(Content.Atof());
				}

				if (NodeName== "Sample") {
					comptage++;
					copyContent = Content;
					index =Content.Index(" ");
					if (index<0){
						tempos.Form("GParaCaliXml::GetCoefMatacq Impossible to read one coefficient");
										fError.TreatError(3, 0, tempos);
					}else{

					nosample =(Int_t)  (TString(Content.Data(),index)).Atoi();
					etalon  = (Float_t)((copyContent.Remove(0,index)).Atof());
					Etalonnage[channeli]->SetBinContent(1+nosample, etalon);
					}
				}

				if (nblecturenormale<comptage) {
					tempos.Form ("Number of read samples in xml file too large %d" ,comptage);
					fError.TreatError(2, 0, tempos);
					break;
				}
				sample = fXmlFile->GetNext(sample);
			}
			channel = fXmlFile->GetNext(channel);

			if (nb<comptagechannel) {
				tempos.Form (" Number of reads channels in xml file too large %d",comptagechannel);
				fError.TreatError(2, 0, tempos);
				break;
			}
		}

}
*/
//______________________________________________________________

/*
 MFMXmlDataDescriptionFrame.cc
 Copyright Acquisition group, GANIL Caen, France
 */

#include <iostream>
#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
using namespace std;

#include "MFMXmlDataDescriptionFrame.h"

//_______________________________________________________________________________
MFMXmlDataDescriptionFrame::MFMXmlDataDescriptionFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize) {
	/// Constructor for a exogam frame . the header is filled with unitblock_size, data source , frame type , revision , frame, size and header size value
	SetPointers();
	NUM_INDENTS_PER_SPACE=2;
}

//_______________________________________________________________________________
MFMXmlDataDescriptionFrame::MFMXmlDataDescriptionFrame() {
	/// Constructor for a empty exogam frame
	NUM_INDENTS_PER_SPACE=2;

}
//_______________________________________________________________________________
MFMXmlDataDescriptionFrame::~MFMXmlDataDescriptionFrame() {
/// destructor of Exogam frame
}
//_______________________________________________________________________________
void MFMXmlDataDescriptionFrame::SetBufferSize(int size, bool ifinferior) {
	MFMBlobFrame::SetBufferSize(size, ifinferior);
	MFMXmlDataDescriptionFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMXmlDataDescriptionFrame::SetPointers(void * pt) {
	MFMBlobFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMXmlDataDescriptionFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBlobFrame::SetAttributs(pt);
}
//_______________________________________________________________________________

uint64_t MFMXmlDataDescriptionFrame::GetTimeStamp() {
	/// do nothing and return 0;

	return 0;
}
//_______________________________________________________________________________
uint64_t MFMXmlDataDescriptionFrame::GetTimeStampAttribut() {
	/// Return attibut of time stamp
	return 0;
}

//_______________________________________________________________________________

int MFMXmlDataDescriptionFrame::GetEventNumber() {
	/// dp nothing and return 0
	return 0;
}

//_______________________________________________________________________________
int MFMXmlDataDescriptionFrame::GetEventNumberAttribut() {
	/// Return attibut of event number
	return 0;
}
//_______________________________________________________________________________
void MFMXmlDataDescriptionFrame::SetTimeStamp(uint64_t timestamp) {
	/// Set value of Time Stamp in frame

}
//_______________________________________________________________________________
void MFMXmlDataDescriptionFrame::SetEventNumber(int eventnumber) {
	/// Set Event Number of frame

}

//_______________________________________________________________________________
string MFMXmlDataDescriptionFrame::FillExampleOfText() {

	/// Fill text fied wiht a example
	string text;
text = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n";
text.append("<data_description>\n");
text.append("		<scaler_description>\n");
text.append("		  <cpu>\n");
text.append("		    <nom>ganlo0</nom>\n");
text.append("		    <module>\n");
text.append("		      <nom>TGV</nom>\n");
text.append("		      <voie>\n");
text.append("		        <nom>TGV_Echelle_0</nom>\n");
text.append("		        <label>131073</label>\n");
text.append("		        <nom>TGV_Echelle_1</nom>\n");
text.append("		      </voie>\n");
text.append("		    </module>\n");
text.append("		  </cpu>\n");
text.append("		</scaler_description>\n");

text.append("		<parameter_description>\n");
text.append("		  <channels>\n");
text.append("		    <nbbits> 14\n");
text.append("		    <deph> 1\n");
text.append("		      <channel>\n");
text.append("		        <name>GMT_GATCONF </name>\n");
text.append("		        <label>1</label>\n");
text.append("		        <nbits> 15 </nbits>\n");
text.append("		      </channel>\n");
text.append("		      <channel>\n");
text.append("		        <name>parameter1</name>\n");
text.append("		        <label>2</label>\n");
text.append("		      </channel>\n");
text.append("		      <channel>\n");
text.append("		        <name>parameter2</name>\n");
text.append("		        <label>2</label>\n");
text.append("		      </channel>\n");
text.append("		      <channel>\n");
text.append("		        <name>parameter3</name>\n");
text.append("		        <label>2</label>\n");
text.append("		      </channel>\n");
text.append("		      <channel>\n");
text.append("		        <name>parameter4</name>\n");
text.append("		        <label>2</label>\n");
text.append("		      </channel>\n");
text.append("		    </channels>\n");
text.append("		  </parameter_description>\n");
text.append("/<data_description>\n");
return text;
}

//_______________________________________________________________________________

int  MFMXmlDataDescriptionFrame::InitXml(bool write_or_read, string commentaire) {
	// Initialisation , Create an XML file or to read a xml file
	// if write_or_read = true  => write configuration => commentaire is a comment in the header of file
	// if write_or_read = false => read  configuration => commentaire is the name file to open

	string tempos,tempos2,entete;
/*
	if (write_or_read) {
		fError.Infos( "Init of write XML file");
		//balise entete
		tempos="";
		entete = "Calibration coefficient2";

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
		TiXmlElement* msg;
		fFileName=commentaire;
		tempos.Form ("Read XML file %s ",fFileName.data());
				fError.TreatError(0,0,tempos);
		MyTinyDoc = new TiXmlDocument(fFileName);
		if(!MyTinyDoc->LoadFile()){
		    cerr << "erreur lors du chargement" << endl;
		    cerr << "error #" << doc.ErrorId() << " : " << doc.ErrorDesc() << endl;
		    delete MyTinyDoc;
		    MyTinyDoc = NULL;
		    return 1;

		}


		fXmlDoc = fXmlFile->ParseFile(commentaire.Data());
		if (fXmlDoc==0) {
			fError.TreatError(2, -1, "erreur de lecture du fichier xml");
			delete fXmlFile;
			fXmlFile =NULL;
			return 1;
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
	*/
	return 0;
}












/*

  TDatime *my_time;//!
  TXMLEngine* fXmlFile; //!

  XMLNodePointer_t fXmlMain; //!
  XMLDocPointer_t  fXmlDoc;//!

 protected:

  Int_t fStatus; // Internal status ( to have a satus whithout a returned status which noise the standard output of Cint )
  Float_t fVersion;// version of this class


 public:

  TString fFileName; // Xml File Name



MFMXmlDataDescriptionFrame::MFMXmlDataDescriptionFrame() {

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

MFMXmlDataDescriptionFrame::~MFMXmlDataDescriptionFrame() {
	gROOT->cd();
	//if (fWorkingDirectory) {
		//delete [] fWorkingDirectory;
		//fWorkingDirectory=NULL;
	//}
	if (my_time) {
		delete my_time;
		my_time =NULL;
	}
	if (fXmlFile)
		delete fXmlFile;
	fXmlFile =NULL;

}

//______________________________________________________________

void MFMXmlDataDescriptionFrame::InitXml(bool write_or_read, TString commentaire) {
	// Initialisation , Create an XML file or to read a xml file
	// if write_or_read = true  => write configuration => commentaire is a comment in the header of file
	// if write_or_read = false => read  configuration => commentaire is the name file to open
	// for matacq xml file have this format.
	//<?xml version="1.0"?>
	//<root>
	//<Entete>
	//   <Version>        1</Version>
	//   <Date>170121264</Date>
	//   <Note>Etalonnage_Matacq</Note>
	//</Entete>
	//<Channel> 0
	//   <Sample > 0 -2147483648 </Sample >
	//   <Sample > 1 -2147483648 </Sample >
	//</Channel>
	//<Channel> 1 .....
	//   <Sample > 0 -23851854156 </Sample >
	TString tempos,tempos2,entete;
	if (write_or_read) {
		fError.Infos( "Init of Calibration Coefficient file");
		//balise entete
		tempos="";
		entete = "Calibration coefficient2";

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
void MFMXmlDataDescriptionFrame::CloseXml() {
	XMLDocPointer_t fXmlDoc = fXmlFile->NewDoc();
	fXmlFile->DocSetRootElement(fXmlDoc, fXmlMain);
	fXmlFile->SaveDoc(fXmlDoc, fFileName.Data(), 2);
	// Release memory before exit
	fXmlFile->FreeDoc(fXmlDoc);
}

//______________________________________________________________
void MFMXmlDataDescriptionFrame::SaveParameter(int no, const char* ParaName, int nb_values,
		float* Values_Gene, float Etalon, float Gene_a1, float Gene_a2,
		float Gene_a3, int Mate, int voie, int telescope, float* pics,
		int nbpics) {
	SaveParameter(&no, ParaName, &nb_values, Values_Gene, &Etalon, &Gene_a1,
			&Gene_a2, &Gene_a3, &Mate, &voie, &telescope, pics, &nbpics);
}
//______________________________________________________________
void MFMXmlDataDescriptionFrame::SaveParameter(int* no, const char* ParaName, int* nb_values,
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

void MFMXmlDataDescriptionFrame::GetParameter(int* no, int* Piedestal, float* Etalon,
		float* Gene_a1, float* Gene_a2, float* Gene_a3, int* Mate, int* voie,
		int* telescopes) {
	// get calibration coefficients from XML file.

	// to finish

}

//______________________________________________________________

void MFMXmlDataDescriptionFrame::UpdateFromXML(TXMLEngine* xml, XMLNodePointer_t node) {//
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

void MFMXmlDataDescriptionFrame::SaveCoefMatacq(TH1F** Etalonnage, Float_t* DeltaT,
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

void MFMXmlDataDescriptionFrame::GetCoefMatacq(TH1F** Etalonnage, Float_t* DeltaT,
		Float_t* VernierMin, Float_t* VernierMax, Int_t nb, Int_t longchannel) {
	//Get calibration coefficients and put them in  histogram  ' Etalonnage'

	TString Content, NodeName, copyContent,tempos;
	int channeli, comptage, comptagechannel, nblecturenormale, nosample, index;
	Float_t etalon;
	XMLNodePointer_t sample;
	if (fXmlFile==NULL) {
		fError.TreatError(2,0,"MFMXmlDataDescriptionFrame::GetCoefMatacq : No XML file");
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
						tempos.Form("MFMXmlDataDescriptionFrame::GetCoefMatacq Impossible to read one coefficient");
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
//_______________________________________________________________________________
const char * MFMXmlDataDescriptionFrame::getIndent( unsigned int numIndents )
{
	static const char * pINDENT="                                      + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}
//_______________________________________________________________________________
// same as getIndent but no "+" at the end
const char * MFMXmlDataDescriptionFrame::getIndentAlt( unsigned int numIndents )
{
	static const char * pINDENT="                                        ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}
//_______________________________________________________________________________
int MFMXmlDataDescriptionFrame::dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
{
	if ( !pElement ) return 0;

	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent=getIndent(indent);
	printf("\n");
	while (pAttrib)
	{
		printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

	//	if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    printf( " int=%d", ival);
	//	if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;
}
//_______________________________________________________________________________
void MFMXmlDataDescriptionFrame::dump_to_stdout( TiXmlNode* pParent, unsigned int indent )
{
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;

	switch ( t )
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		printf( "Document" );
		break;

	case TiXmlNode::TINYXML_ELEMENT:
		printf( "Element [%s]", pParent->Value() );
		num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
		switch(num)
		{
			case 0:  printf( " (No attributes)"); break;
			case 1:  printf( "%s1 attribute", getIndentAlt(indent)); break;
			default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
		}
		break;

	case TiXmlNode::TINYXML_COMMENT:
		printf( "Comment: [%s]", pParent->Value());
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		printf( "Unknown" );
		break;

	case TiXmlNode::TINYXML_TEXT:
		pText = pParent->ToText();
		printf( "Text: [%s]", pText->Value() );
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		printf( "Declaration" );
		break;
	default:
		break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
	{
		dump_to_stdout( pChild, indent+1 );
	}
}
//_______________________________________________________________________________
// load the named file and dump its structure to STDOUT
void MFMXmlDataDescriptionFrame::dump_to_stdout(const char* pFilename)
{
//	TiXmlDocument doc(pFilename);
//	bool loadOkay = doc.LoadFile();
//	if (loadOkay)
	{
		printf("\n%s:\n", pFilename);
	//	dump_to_stdout( &doc ); // defined later in the tutorial
	}
//	else
	{
	//	printf("Failed to load file \"%s\"\n", pFilename);
	}
}
//_______________________________________________________________________________
// ----------------------------------------------------------------------
// main() for printing files named on the command line
// ----------------------------------------------------------------------

//int main(int argc, char* argv[])
//{
//	for (int i=1; i<argc; i++)
//	{
//		dump_to_stdout(argv[i]);
//	}
//	return 0;
//}



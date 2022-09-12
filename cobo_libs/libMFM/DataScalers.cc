// Author: $Author: patois $ modified by Legeard
/***************************************************************************
 //                        DataScalers.cpp  -  Scalers name handling
 //////////////////////////////////////////////////////////////////////////
 //
 //
 // DataScalers
 //
 // Handle Scalers names and related offsets in the event buffer.
 //
 /////////////////////////////////////////////////////////////////////////
 //

 // ---------------------------------------------------------------------------
 // ***************************************************************************
 // *                                                                         *
 // *   This program is free software; you can redistribute it and/or modify  *
 // *   it under the terms of the GNU General Public License as published by  *
 // *   the Free Software Foundation; either version 2 of the License, or     *
 // *   (at your option) any later version.                                   *
 // *                                                                         *
 // ***************************************************************************/

#include "DataScalers.h"
#include "stdio.h"
#include "stdlib.h"

//______________________________________________________________________________
DataScalers::DataScalers(void) {
	//Default constructor. Don't create anything yet.

	// when we'll delete the tree itself
	fLabelToIndex = NULL;
}

//______________________________________________________________________________
DataScalers::~DataScalers(void) {
	if (fLabelToIndex) {
		delete[] fLabelToIndex;
		fLabelToIndex = NULL;
	}
    fList.clear();
}

//_____________________________________________________________________________

int DataScalers::Fill(const char *buffParam) {
	// Data from the Scaler buffer (buffParam) have been read from disk
	//and parsed by this method to create the list of labels.
	//We work on a copy of buffParam because strtok_r modify his Scalers

	char workString[strlen(buffParam)];
	strcpy(workString, buffParam);

	while (param != NULL) {

		// scrute dans XML


		DataScal *scal = new DataScal ( name, label);
		fList.push_back(*par);
	}
	fNb_Scalers = fList.size();
	creatLabelToIndexVectors();
	return (fNb_Scaler);
}

//______________________________________________________________________________
int DataScalers::FillFromXmlFile(const char *xmlfile) {
	// Data from the Scaler buffet (buffParam) have been read from disk and is
	// parsed by this routine who create the list of labels and corresponding
	// label number

	if (OpenXmlFile(const char* pFilename))
		dump_to_stdout( &fTiXmlDocument );




   // Get Frame of data desctription.

  //  Fill(const char *buffParam);



	return (fNb_Scalers);

}
//______________________________________________________________________________
DataScal* DataScalers::FindObject(const string parName) {

	DataScal *par = NULL;
	for (unsigned int i = 0; i < fList.size(); i++) {
		par = (&fList[i]);
		int comp = ((par->Name()).compare(parName));
		if (comp == 0) {
			break;
		} else {
			par = NULL;
		}
	}
	return par;
}
//______________________________________________________________________________
int DataScalers::GetLabel(const string scalName) {
	// Return the label number corresponding to a text label.
	DataScal *par = FindObject(parName);
	string message;

	if (!par) {
		message = "Scaler  ";
		message += parName;
		message += " not found! (DataScalers::GetLabel)";
		fError.TreatError(2, -1, &message);
		return (-1);
	}
	return (par->Label());
}

//______________________________________________________________________________
int DataScalers::GetLabel(const int index) {
	// Return the Label number corresponding to a text label.
	DataScal *par = NULL;
	par = (DataScal*) (&fList[(unsigned int) index]);
	string message;

	int retour = 0;
	if (!par) {
		message = "Scaler  ";
		message += index;
		message += " not found! (DataScalers::GetLabel(const int index))";
		fError.TreatError(2, -1, &message);
		return (-1);

	}
	retour = par->Label();
	return (retour);
}

//______________________________________________________________________________
int DataScalers::GetNbitsFromLabel(const int label) {
	// Return  number of bit corresponding to index.
	return (GetNbitsFromIndex(GetIndex(label)));
}

//______________________________________________________________________________
const char* DataScalers::GetParNameFromIndex(const int index) {
	// Return the text label name corresponding to an index.

	DataScal *par = (DataScal*) &(fList[index]);
	string message;

	if (!par) {
		message = "Scaler order index  ";
		message += index;
		message += " not found! (DataScalers::GetParName) ";
		fError.TreatError(2, -1, &message);

		return ("");
	}
	return ((par->Name()).data());
}
//______________________________________________________________________________
const char* DataScalers::GetParNameFromLabel(const int label) {
	// Return the text label name corresponding to an index.
	return (GetParNameFromIndex(GetIndex(label)));
}
//______________________________________________________________________________
void DataScalers::creatLabelToIndexVectors() {
	// Creation of LabelToIndex Vector and fUserLabelToIndex Vector
	string message;
	DataScal *par;
	int i_label;
	uint32_t index = 0;
	int max_label = 0;
	int current_label;

	// research of Max value of label
	max_label = GetMaxLabel();

	message = "Error in creatLabelToIndexVector max_label =0";
	if (max_label == 0) {
		fError.TreatError(2, -1, &message);
		return;
	}
	fLabelToIndex = new uint32_t[max_label + 1];
	fLabelToUserLabel = new uint32_t[max_label + 1];
	for (i_label = 0; i_label <= max_label; i_label++) {
		fLabelToIndex[i_label] = 0;
		fLabelToUserLabel[i_label] = 0;
	}

	for (index = 0; (int) index < fNb_Scalers; index++) {
		par = (DataScal*) &(fList[(int) index]);
		current_label = par->Label();
		fLabelToIndex[current_label] = (uint32_t) index;
	}

}

//______________________________________________________________________________
int* DataScalers::creatLabelToExist(int* max_label) {
	// Creation of LabelToIndex Int Vector
	// use for example for statistics
	//

	string message;
	DataScal *par;
	int i_label;
	int index = 0;
	*max_label = 0;
	int current_label;
	int* LabelToIndex;

	*max_label = GetMaxLabel();
	if (*max_label == 0) {
		fError.TreatError(2, -1, "Error in creatLabelToIndexVectorInt");
		return (NULL);
	}
	LabelToIndex = new int[(*max_label) + 1];
	for (i_label = 0; i_label <= *max_label; i_label++)
		LabelToIndex[i_label] = 0;
	for (index = 0; index < fNb_Scalers; index++) {
		par = (DataScal*) &(fList[(int) index]);
		current_label = par->Label();
		LabelToIndex[current_label] = 1;
	}
	return (LabelToIndex);
}
//______________________________________________________________________________
int DataScalers::GetIndex(const int label) {
	// Return the order index number

	return (fLabelToIndex[label]);
}
//______________________________________________________________________________
int DataScalers::GetMaxLabel() {
	// Return the order index number
	int i,current_label, indexmax=fList.size();
	DataScal *par;
	int max_label =0;
	for ( i=0;i<indexmax;i++) {
			par = (DataScal*) &(fList[i]);
			if (!par) {
				break;
			}
			current_label = (int) par->Label();
			if (max_label < current_label)
				max_label = current_label;

		}
	return (max_label);
}
//______________________________________________________________________________
void DataScalers::DumpScalerName() {
	// Dump Scaler names and infos.

	int presentation = 0;
	cout << "--------- DUMPING ScalerS  ----------------------------"
			<< endl;

	for (int i = 0; i < (int) (fList.size()); i++) { // WARNING : index start at one, the first value is boggus
		cout << "Index order :" << i << " " << GetParNameFromIndex(i) << "  Index : "
				<< GetIndex(GetParNameFromIndex(i)) << "  " << "  Label : " << GetLabel(
				GetParNameFromIndex(i)) << "  ";
		//					<< "  Index : " << GetIndex(i) <<"  ";
		if (presentation++ == 0) {
			cout << "\n";
			presentation = 0;
		}
	}
	if (presentation != 0)
		cout << "\n";
	cout << "--------- END           ----------------------------" << endl;
}

//______________________________________________________________________________
int DataScalers::GetUserLabelFromLabel(const int label) {
	// Return the order index number

	return (fLabelToUserLabel[label]);
}
//______________________________________________________________________________
int DataScalers::GetUserLabelFromIndex(const int index) {
	return (fLabelToUserLabel[GetLabel(index)]);
}
//______________________________________________________________________________
void DataScalers::SetUserLabelFromIndex(const int index, int userlabel) {
	DataScal *par;
	par = (DataScal*) &(fList[(int) index]);
	if (par > 0) {
		par->SetUserLabel(userlabel);
	} else {
		fError.TreatError(2, -1, "Error in setting user label");
	}
}
//______________________________________________________________________________
int DataScalers::GetUserLabel(const string parName) {
	return (fLabelToUserLabel[GetLabel(parName)]);
}

//______________________________________________________________________________
void DataScalers::DumpListPara() {
	// Dump Scalers
	//DataScal *par;
	for (int i = 0; i < fNb_Scalers; i++) {
		cout << GetParNameFromIndex((int) i) << "   Index = " << (int) i << "  Label :"
				<< (int) GetLabel((int) i) << "   Bits:" << (int) GetNbitsFromIndex(
				(int) i) << "   Depth:" << (int) GetDepthFromIndex((int) i) << "   " << "\n";
	}
}
//______________________________________________________________________________


bool DataScalers::IsAActionFile(const char *actionfile) {

	FILE *fp = fopen(actionfile, "r");

	char line[MAX_CAR];
	char* tmp = fgets(line, (MAX_CAR - 1), fp);
    if (!tmp)fError.TreatError(2, -1, "IsAActionFile");
	char* token;

    token = strtok(&line[0], " ");
	if (token == NULL)
		return false;

	token = strtok(NULL, " ");
	if (token == NULL or atoi(token) == 0)
		return false;

	token = strtok(NULL, " ");
	if (token == NULL or atoi(token) == 0)
		return false;

	token = strtok(NULL, " ");
	if (token != NULL and atoi(token) == 0)
		return false;

	token = strtok(NULL, " ");
	if (token != NULL)
		return false;

	fclose(fp);

	return true;
}

//______________________________________________________________________________
int DataScalers::GetIndex(const string parName) {
	// Return the order index number

	return (GetIndex(GetLabel(parName)));
}
//______________________________________________________________________________
void DataScalers::TestScalers(){
char *name;
int label,nbb,depth;
	cout <<" Test Paramater list"<<endl;
 for (int i =0; i<fNb_Scalers; i++){

	name = (char*) GetParNameFromIndex(i);
	label =GetLabel(i);
	nbb = GetNbitsFromIndex(i);
	depth = GetDepthFromIndex(i);
    cout <<"------------------------------------------------------------------\n";
	cout << "From Index : "<<i<< " GetParNameFromIndex(index)="<<name<< " GetLabel(index)="<<label<<" GetNbitsFromIndex(index)="<<nbb
			<<" GetDepthFromIndex(index)="<<depth<<" GetUserLabel(index)="<<GetUserLabelFromIndex(i)<<"\n";
	cout << "From Name : "<<name<< " GetLabel(Name)="<<GetLabel(name)<<" GetIndex(name)="<<GetIndex(name)<<" GetNbits(name)"<<GetNbits(name)<<
			" GetDepth(Name)="<<GetDepth(name)<<"\n";
	cout << "From Label : "<<label<<" GetParNameFromLabel()="<<GetParNameFromLabel(label)<<" GetIndex(label)="<<GetIndex(name)<<
			" GetNbitsFromLabel(label)"<<GetNbitsFromLabel(label)<<" GetDepthFomLabel(Name)="<<GetDepthFromLabel(label)<<"\n";
 }
 }
//______________________________________________________________________________
//_______________________________________________________________________________
const char * DataScalers::getIndent( unsigned int numIndents )
{
	static const char * pINDENT="                                      + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}
//_______________________________________________________________________________
// same as getIndent but no "+" at the end
const char * DataScalers::getIndentAlt( unsigned int numIndents )
{
	static const char * pINDENT="                                        ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}
//_______________________________________________________________________________
int DataScalers::dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
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

		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    printf( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;
}
//_______________________________________________________________________________
void DataScalers::dump_to_stdout( TiXmlNode* pParent, unsigned int indent )
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
	case TiXmlNode::TINYXML_DECLARATION:
		printf( "Declaration" );
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
	case TiXmlNode::TINYXML_TEXT:
		pText = pParent->ToText();
		printf( "Text: [%s]", pText->Value() );
		break;

	case TiXmlNode::TINYXML_COMMENT:
		if (fVerbose)
			printf( "Comment: [%s]", pParent->Value());
		break;
	case TiXmlNode::TINYXML_UNKNOWN:
		fError.TreatError(1,0, "TiXmlNode Unknown");
		printf( "Unknown" );
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
// load the named file
TiXmlDocument * DataScalers::OpenXmlFile(const char* pFilename)
{
	fTiXmlDocument  = new TiXmlDocument(pFilename);
	bool loadOkay = fTiXmlDocument.LoadFile();
	if (loadOkay)
	{
		dump_to_stdout( &doc ); // defined later in the tutorial
	}
	else
	{
		fError.TreatError(2,0,"Failed to load file  DataScalers::OpenXmlFile : ", pFilename);
		delete fTiXmlDocument;
		fTiXmlDocument =NULL;
	}
	return fTiXmlDocument;

}

// Author: $Author: patois $ modified by Legeard
/***************************************************************************
 //                        DataParameters.cpp  -  Parameters name handling
 //////////////////////////////////////////////////////////////////////////
 //
 //
 // DataParameters
 //
 // Handle parameters data names and related offsets in the event buffer.
 // Parameter on GANIL Tapes have three specification :
 //  - Name of parameter ( char)
 //  - Label : (int) it is the number witch is notified in electronique
 //  - Nbit  : (int) number of bits of parameter
 //  - Index : (int) it is the order number inside array.
 //   Note , often, we can find that index and labels are equal
 // This class handle conversion between those two representation.
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

#include "DataParameters.h"
#include "stdio.h"
#include "stdlib.h"

//______________________________________________________________________________
DataParameters::DataParameters(void) {
	//Default constructor. Don't create anything yet.

	// when we'll delete the tree itself
	fLabelToIndex = NULL;
	fLabelToUserLabel = NULL;
}

//______________________________________________________________________________
DataParameters::~DataParameters(void) {
	if (fLabelToUserLabel){
		delete [] fLabelToUserLabel;
		fLabelToUserLabel=0;
	}

	if (fLabelToIndex) {
		delete[] fLabelToIndex;
		fLabelToIndex = NULL;
	}
    fList.clear();
}

//_____________________________________________________________________________

int DataParameters::Fill(const char *buffParam) {
	// Data from the parameter buffer (buffParam) have been read from disk
	//and parsed by this method to create the list of labels.

	//We work on a copy of buffParam because strtok_r modify his parameters
	char workString[strlen(buffParam)];
	strcpy(workString, buffParam);

	char nomParam[20];
	int numPar, nbBits, depth;

	char *param, *paramElem;

	char *savepointer1, *savepointer2;

	param = strtok_r(workString, " \x0d", &savepointer1);
	while (param != NULL) {
		depth = 1; //Default value
		if (!(strncmp(param, "    ", 4)) or !(strncmp(param, "!", 1))) {
			break;
		}
		paramElem = strtok_r(param, " ,", &savepointer2);

		strcpy(nomParam, paramElem);

		paramElem = strtok_r(NULL, " ,", &savepointer2);
		//We convert a string to a int, ex : "25" to 25
		numPar = atoi(paramElem);

		paramElem = strtok_r(NULL, " ,", &savepointer2);
		nbBits = atoi(paramElem);

		paramElem = strtok_r(NULL, " ,", &savepointer2);

		if (paramElem != NULL) {
			depth = atoi(paramElem);
		}

		param = strtok_r(NULL, " \x0d", &savepointer1);
		DataPar *par = new DataPar(nomParam, numPar, nbBits, depth);
		fList.push_back(*par);
	}
	fNb_Parameters = fList.size();
	creatLabelToIndexVectors();
	return (fNb_Parameters);
}

//______________________________________________________________________________
int DataParameters::FillFromActionFile(const char *actionfile) {
	// Data from the parameter buffet (buffParam) have been read from disk and is
	// parsed by this routine who create the list of labels and corresponding
	// label number

    string test;
 	FILE *fp = fopen(actionfile, "r");
    if (fp == nullptr) {
    	cout << "Action File Not Found" << endl;
    	return 0;
    }
	char line[MAX_CAR];
	char parametre[MAX_CAR];
	int label, nbbits, depth;
    int lengthstring=0;
	while (fgets(line, (MAX_CAR- 1), fp)) {
		test=line;
		lengthstring= test.length();
		if (lengthstring<5) break;
		depth = 1; //Default value
		char *token;
		token = strtok(&line[0], " ");
		strcpy(parametre, token);
		token = strtok(NULL, " ");
		label = atoi(token);
		token = strtok(NULL, " ");
		nbbits = atoi(token);
		token = strtok(NULL, " ");
		if (token != NULL) {
			depth = atoi(token);
		}
		if (depth <= 0)
			depth = 1;
		DataPar par(parametre, label, nbbits, depth);
		fList.push_back(par);
	}

	fclose(fp);
	fNb_Parameters = fList.size();
	creatLabelToIndexVectors();
	return (fNb_Parameters);

}
//______________________________________________________________________________
DataPar* DataParameters::FindObject(const string parName) {

	DataPar *par = NULL;
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
int DataParameters::GetLabel(const string parName) {
	// Return the label number corresponding to a text label.
	DataPar *par = FindObject(parName);
	string message;

	if (!par) {
		message = "Parameter  ";
		message += parName;
		message += " not found! (DataParameters::GetLabel)";
		fError.TreatError(2, -1, &message);
		return (-1);
	}
	return (par->Label());
}

//______________________________________________________________________________
int DataParameters::GetLabel(const int index) {
	// Return the Label number corresponding to a text label.
	DataPar *par = NULL;
	par = (DataPar*) (&fList[(unsigned int) index]);
	string message;

	int retour = 0;
	if (!par) {
		message = "Parameter  ";
		message += index;
		message += " not found! (DataParameters::GetLabel(const int index))";
		fError.TreatError(2, -1, &message);
		return (-1);

	}
	retour = par->Label();
	return (retour);
}
//______________________________________________________________________________
int DataParameters::GetNbits(const string parName) {
	// Return number of bit corresponding to a text label.
	DataPar *par = FindObject(parName);
	string message;

	if (!par) {
		message = "Parameter  ";
		message += parName;
		message += " not found! (DataParameters::GetNbits)";
		fError.TreatError(2, -1, &message);
		return (-1);
	}
	return (par->Nbits());
}

//______________________________________________________________________________
int DataParameters::GetNbitsFromIndex(const int index) {
	// Return  number of bit corresponding to index.
	DataPar *par = (DataPar*) (&fList[index]);
	string message;

	int retour = 0;

	if (!par) {
		message = "Parameter  ";
		message += index;
		message += " not found! (DataParameters::GetNbits(const int index))";
		fError.TreatError(2, -1, &message);
		return (-1);
	}
	retour = par->Nbits();
	return (retour);
}
//______________________________________________________________________________
int DataParameters::GetNbitsFromLabel(const int label) {
	// Return  number of bit corresponding to index.
	return (GetNbitsFromIndex(GetIndex(label)));
}
//______________________________________________________________________________
int DataParameters::GetDepth(const string parName) {
	// Return number of bit corresponding to a text label.
	DataPar *par = FindObject(parName);
	string message;

	if (!par) {
		message = "Parameter  ";
		message += parName;
		message += " not found! (DataParameters::GetDepth)";
		fError.TreatError(2, -1, &message);
		return (-1);
	}
	return (par->Depth());
}
//______________________________________________________________________________
int DataParameters::GetDepthFromIndex(const int index) {
	// Return  number of bit corresponding to index.

	DataPar *par = (DataPar*) &(fList[index]);
	string message;

	int retour = 0;
	if (!par) {
		message = "Parameter  ";
		message += index;
		message += " not found! (DataParameters::GetDepthFromIndex(const int index))";
		fError.TreatError(2, -1, &message);
		return (-1);
	}
	retour = par->Depth();
	return (retour);
}
//______________________________________________________________________________
int DataParameters::GetDepthFromLabel(const int label) {
	// Return  number of bit corresponding to label.
	return (GetDepthFromIndex(GetIndex(label)));
}
//______________________________________________________________________________
const char* DataParameters::GetParNameFromIndex(const int index) {
	// Return the text label name corresponding to an index.

	DataPar *par = (DataPar*) &(fList[index]);
	string message;

	if (!par) {
		message = "Parameter order index  ";
		message += index;
		message += " not found! (DataParameters::GetParName) ";
		fError.TreatError(2, -1, &message);

		return ("");
	}
	return ((par->Name()).data());
}
//______________________________________________________________________________
const char* DataParameters::GetParNameFromLabel(const int label) {
	// Return the text label name corresponding to an index.
	return (GetParNameFromIndex(GetIndex(label)));
}
//______________________________________________________________________________
void DataParameters::creatLabelToIndexVectors() {
	// Creation of LabelToIndex Vector and fUserLabelToIndex Vector
	string message;
	DataPar *par;
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

	for (index = 0; (int) index < fNb_Parameters; index++) {
		par = (DataPar*) &(fList[(int) index]);
		current_label = par->Label();
		fLabelToIndex[current_label] = (uint32_t) index;
	}

}

//______________________________________________________________________________
int* DataParameters::creatLabelToExist(int* max_label) {
	// Creation of LabelToIndex Int Vector
	// use for example for statistics
	//

	string message;
	DataPar *par;
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
	for (index = 0; index < fNb_Parameters; index++) {
		par = (DataPar*) &(fList[(int) index]);
		current_label = par->Label();
		LabelToIndex[current_label] = 1;
	}
	return (LabelToIndex);
}
//______________________________________________________________________________
int DataParameters::GetIndex(const int label) {
	// Return the order index number

	return (fLabelToIndex[label]);
}
//______________________________________________________________________________
int DataParameters::GetMaxLabel() {
	// Return the order index number
	int i,current_label, indexmax=fList.size();
	DataPar *par;
	int max_label =0;
	for ( i=0;i<indexmax;i++) {
			par = (DataPar*) &(fList[i]);
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
void DataParameters::DumpParameterName() {
	// Dump Parameter names and infos.

	int presentation = 0;
	cout << "--------- DUMPING PARAMETERS  ----------------------------"
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
int DataParameters::GetUserLabelFromLabel(const int label) {
	// Return the order index number

	return (fLabelToUserLabel[label]);
}
//______________________________________________________________________________
int DataParameters::GetUserLabelFromIndex(const int index) {
	return (fLabelToUserLabel[GetLabel(index)]);
}
//______________________________________________________________________________
void DataParameters::SetUserLabelFromIndex(const int index, int userlabel) {
	DataPar *par;
	par = (DataPar*) &(fList[(int) index]);
	if (par != nullptr) {
		par->SetUserLabel(userlabel);
	} else {
		fError.TreatError(2, -1, "Error in setting user label");
	}
}
//______________________________________________________________________________
int DataParameters::GetUserLabel(const string parName) {
	return (fLabelToUserLabel[GetLabel(parName)]);
}

//______________________________________________________________________________
void DataParameters::DumpListPara() {
	// Dump parameters
	//DataPar *par;
	for (int i = 0; i < fNb_Parameters; i++) {
		cout << GetParNameFromIndex((int) i) << "   Index = " << (int) i << "  Label :"
				<< (int) GetLabel((int) i) << "   Bits:" << (int) GetNbitsFromIndex(
				(int) i) << "   Depth:" << (int) GetDepthFromIndex((int) i) << "   " << "\n";
	}
}
//______________________________________________________________________________


bool DataParameters::IsAActionFile(const char *actionfile) {

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
int DataParameters::GetIndex(const string parName) {
	// Return the order index number

	return (GetIndex(GetLabel(parName)));
}
//______________________________________________________________________________
void DataParameters::TestParameters(){
char *name;
int label,nbb,depth;
	cout <<" Test Paramater list"<<endl;
 for (int i =0; i<fNb_Parameters; i++){

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

#include <iostream>
using namespace std;
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "MFMBasicFrame.h"
#include "MFMBlobFrame.h"
#include "MFMExogamFrame.h"
#include "MFMEbyedatFrame.h"
#include "MFMMergeFrame.h"
#include "MFMCoboFrame.h"
#include "MFMOscilloFrame.h"
#include "MFMScalerDataFrame.h"
#include "MFMMutantFrame.h"
#include "MFMRibfFrame.h"
#include "MFMXmlDataDescriptionFrame.h"
#include "MFMXmlFileHeaderFrame.h"
#include "DataParameters.h"
#include "MError.h"
#include "Cobo.h"
#include <cstring>
#include <cstdlib>
#include <time.h>

#define ADONIS_MSK 0x3FFF

int main(int atgc, char **argv) {
	MError Error;
	char* tempo[128];
	bool log_iter = true;

	if ((atgc == 1) or ((atgc == 2) and (strcmp(argv[1], "help") == 0))) {
		cout
				<< "\n MFMtest.exe : utility to generate and use MFM frame in different formats \n";
		cout << " The generated frames are stored in a file \"mfmfile.dat\" \n";
		cout << " Usage  : \n";
		cout << "         MFMtest.exe [help]      : print this help \n";
		cout
				<< "         MFMtest.exe write       : run with menu to choose format\n";
		cout
				<< "         MFMtest.exe write 2     : run with a choosen format = 2 and default nb of frames = 20 \n";
		cout
				<< "         MFMtest.exe write 2 100 : run with a choosen format = 2 and with 100 generated frames\n";
		cout << "         MFMtest.exe read        : read the mfmfile.dat\n";
		cout << "         MFMtest.exe read run.dat: read \"run.dat\" file \n";
	}
	//_____________________ WRITE ____________________________________________________________
	if ((atgc >= 2) and ((strcmp(argv[1], "write") == 0))) {
		int answer = 0;
		int nbframes = 20;
		if ((atgc >= 3))
			answer = atoi(argv[2]);
		if ((atgc >= 4))
			nbframes = atoi(argv[3]);

		const char * filename = "mfmfile.dat";
		cout << " Generation of " << nbframes << " MFMFrame of format "
				<< answer << " in a file named " << filename << "\n";

		uint64_t timestamp = 0;
		uint32_t eventnumber = 0;
		uint32_t unitBlock_size = 0;
		uint32_t itemsize = 0;
		uint32_t nbitem = 0;
		uint32_t framesize = 0;
		uint32_t revision = 0;
		uint32_t headersize = 0;

		int fLun; // Logical Unit Number
		fLun = open("./mfmfile.dat", (O_RDWR | O_CREAT | O_TRUNC), 0644);

		int verif;
		int nbframesq = 0;
		if (answer == 0) {
			while ((answer < 1) or (answer > 200)) {
				cout << "Which Frame you want to generate?\n";
				cout << "   1  Cobo\n";
				cout << "   2  Cobo Full\n";
				cout << "   3  Exogam Energy\n";
				cout << "   4  Exogam Oscilloscope\n";
				cout << "   5  NADA Raw \n";
				cout << "   6  NADA Compressed  \n";
				cout << "   7  Ebyedat EventNumber\n";
				cout << "   8  Ebyedat TimeStamp\n";
				cout << "   9  Ebyedat EventNumberTimeStamp\n";
				cout << "  10  Scaler data Frame\n";
				cout << "  11  Mutant data Frame\n";
				cout << "  12  RIBF data Frame\n";
				cout << "\n";
				cout << " 100  Merge of Ebyedats in EventNumber\n";
				cout << " 101  Merge of Ebyedats in TimeStamp\n";
				cout << " 102  Merge of Cobo in EventNumber\n";
				cout << " 103  Scaler data Frame\n";
				cout << "\n";
				cout << " 200  Conversion Adonis in Ebyedat\n";
				cout << " >>";
				cin >> answer;
			}
			while (nbframesq == 0) {
				cout << "How many Frame you want to generate?\n";
				cin >> nbframesq;
			}

			nbframes = nbframesq;
		}

		switch (answer) {
		//_____________________Cobo____________________________________________________________
		case 1: {
			int type = MFM_COBO_FRAME_TYPE;
			MFMCoboFrame * coboframe = new MFMCoboFrame();
			coboframe->InitStat();
			for (int i = 0; i < nbframes; i++) {
				coboframe->GenerateACoboExample(type, i, eventnumber
						% COBO_NB_ASAD);
				coboframe->HeaderDisplay((char*) "-- Header of Cobo Frame --");
				coboframe->FillStat();
				cout << "   TimeStamp = " << coboframe->GetTimeStamp() << "\n";
				cout << "   EventNumber = " << coboframe->GetEventNumber()
						<< "\n";
				coboframe->DumpRaw(256);
				framesize = coboframe->GetFrameSize();

				verif = write(fLun, coboframe->GetPointHeader(), framesize);
				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");
				eventnumber++;

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			coboframe->PrintStat((char*) "Cobo");
			cout << "-------------------END------------------------" << endl;
			delete (coboframe);
			break;

		}
		case 2: {
			int type = MFM_COBOF_FRAME_TYPE;
			MFMCoboFrame * coboframe = new MFMCoboFrame();
			coboframe->InitStat();
			for (int i = 0; i < nbframes; i++) {
				coboframe->GenerateACoboExample(type, i, eventnumber
						% COBO_NB_ASAD);
				coboframe->FillStat();
				coboframe->HeaderDisplay((char*) "-- Header of Cobof Frame --");
				cout << "   TimeStamp = " << coboframe->GetTimeStamp() << "\n";
				cout << "   EventNumber = " << coboframe->GetEventNumber()
						<< "\n";
				coboframe->DumpRaw(256);
				framesize = coboframe->GetFrameSize();

				verif = write(fLun, coboframe->GetPointHeader(), framesize);
				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");
				//cout << " Error of write\n";
				eventnumber++;

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			coboframe->PrintStat((char*) "Cobo");
			cout << "-------------------END------------------------" << endl;
			delete (coboframe);
			break;

		}
			//_____________________Exogam____________________________________________________________
		case 3: {
			MFMExogamFrame * exoframe = new MFMExogamFrame();
			exoframe->InitStat();
			unitBlock_size = 4;
			framesize = EXO_FRAMESIZE;
			revision = 1;

			// generation of MFM header , in this case, MFM is same for all MFM frames
			exoframe->MFM_make_header(unitBlock_size, 0, MFM_EXO2_FRAME_TYPE,
					revision, (int) (framesize / unitBlock_size), true);

			exoframe->HeaderDisplay((char*) "-- Header of Exogam Frame --");
			cout << "\n";

			// generation of nbframes frame
			for (int i = 0; i < nbframes; i++) {
				timestamp = i;
				eventnumber = i;
				exoframe->FillEventRandomConst(timestamp, eventnumber);
				exoframe->FillStat();
				cout << " pointheader = " << exoframe->GetPointHeader() << "\n";
				exoframe->DumpRaw(52, 0);

				verif = write(fLun, exoframe->GetPointHeader(), framesize);

				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");

				eventnumber++;
				timestamp = eventnumber;

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			exoframe->PrintStat((char*) "Exogam");
			cout << "-------------------END------------------------" << endl;
			delete (exoframe);
			break;
		}
			//_____________________ Exogam Oscilloscope____________________________________________________________
		case 4: {
			MFMOscilloFrame * osciframe = new MFMOscilloFrame();
			osciframe->InitStat();
			itemsize = 4;
			int datasize;
			revision = 1;
			unitBlock_size = 2;
			itemsize = 2;
			nbitem = 128;
			headersize = EBYEDAT_EN_HEADERSIZE;
			framesize = headersize + nbitem * itemsize;
			revision = 1;

			// generation of MFM header , in this case, MFM is same for all MFM frames
			osciframe->MFM_make_header(unitBlock_size, 0, MFM_OSCI_FRAME_TYPE,
					revision, (int) (framesize / unitBlock_size), (headersize
							/ unitBlock_size), itemsize, nbitem);

			osciframe->HeaderDisplay(
					(char*) "-- Header of Oscilloscope Frame --");
			cout << "\n";

			// generation of nbframes frame
			for (int i = 0; i < nbframes; i++) {
				timestamp = i;
				eventnumber = i;

				osciframe->FillEventWithRamp(1, 1);
				osciframe->FillStat();
				cout << " pointheader = " << osciframe->GetPointHeader()
						<< "\n";
				osciframe->DumpRaw(256, 0);

				verif = write(fLun, osciframe->GetPointHeader(), framesize);

				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");

				eventnumber++;
				timestamp = eventnumber;

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			osciframe->PrintStat((char*) "Osciframe");
			cout << "-------------------END------------------------" << endl;
			delete (osciframe);
			break;

		}
			//_____________________ NADA Raw____________________________________________________________
		case 5: {
			cout << "This case not already treated! ... Soon \n";
			break;
		}
			//_____________________ NADA Compressed____________________________________________________________
		case 6: {
			cout << "This case not already treated! ... Soon \n";
			exit(0);
		}
			//_____________________ Ebyedat event number____________________________________________________________
		case 7: {
			int type = MFM_EBY_EN_FRAME_TYPE;
			MFMEbyedatFrame * ebyframe = new MFMEbyedatFrame();
			ebyframe->InitStat();
			for (int i = 0; i < nbframes; i++) {
				ebyframe->GenerateAEbyedatExample(type, i);
				ebyframe->FillStat();
				ebyframe->HeaderDisplay(
						(char*) "-- Header of Ebyedat event number Frame --");
				cout << "TimeStamp = " << ebyframe->GetTimeStamp() << "\n";
				cout << "EbventNumber = " << ebyframe->GetEventNumber() << "\n";
				framesize = ebyframe->GetFrameSize();
				ebyframe->DumpRaw(framesize, 0);
				verif = write(fLun, ebyframe->GetPointHeader(), framesize);
				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			ebyframe->PrintStat( "EbyEDat");
			cout << "-------------------END------------------------" << endl;
			delete (ebyframe);
			break;
		}

			//_____________________ Ebyedat Time stamp ____________
		case 8: {

			int type = MFM_EBY_TS_FRAME_TYPE;
			MFMEbyedatFrame * ebyframe = new MFMEbyedatFrame();
			ebyframe->InitStat();
			for (int i = 0; i < nbframes; i++) {
				ebyframe->GenerateAEbyedatExample(type, i);
				ebyframe->FillStat();
				ebyframe->HeaderDisplay(
						(char*) "-- Header of Ebyedat time stamp Frame --");
				cout << "TimeStamp = " << ebyframe->GetTimeStamp() << "\n";
				cout << "EbventNumber = " << ebyframe->GetEventNumber() << "\n";
				framesize = ebyframe->GetFrameSize();
				ebyframe->DumpRaw(framesize, 0);
				verif = write(fLun, ebyframe->GetPointHeader(), framesize);
				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			ebyframe->PrintStat("EbyEDat");
			cout << "-------------------END------------------------" << endl;
			delete (ebyframe);
			break;

		}
			//_____________________ Ebyedat event number and time stamp______________________________________________________
		case 9: {
			int type = MFM_EBY_EN_TS_FRAME_TYPE;
			MFMEbyedatFrame * ebyframe = new MFMEbyedatFrame();
			ebyframe->InitStat();
			for (int i = 0; i < nbframes; i++) {
				ebyframe->GenerateAEbyedatExample(type, i);
				ebyframe->FillStat();
				ebyframe->HeaderDisplay(
						(char*) "-- Header Ebyedat of envent number and time stamp Frame --");
				cout << "TimeStamp = " << ebyframe->GetTimeStamp() << "\n";
				cout << "EbventNumber = " << ebyframe->GetEventNumber() << "\n";
				framesize = ebyframe->GetFrameSize();
				ebyframe->DumpRaw(framesize, 0);
				verif = write(fLun, ebyframe->GetPointHeader(), framesize);
				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			ebyframe->PrintStat("EbyEDat");
			cout << "-------------------END-----------------------" << endl;
			delete (ebyframe);
			break;
		}
			//_____________________ Scaler data frame______________________________________________________
		case 10: {
			int type = MFM_SCALER_DATA_FRAME_TYPE;
			MFMScalerDataFrame * scalerframe = new MFMScalerDataFrame();
			scalerframe->InitStat();
			time_t seconds_past_epoch;
			uint64_t ts;
			int nb_item=20;
			for (int i = 0; i < nbframes; i++) {
				seconds_past_epoch = time(0);
				ts=(uint64_t)seconds_past_epoch ;
				scalerframe->GenerateAScalerExample(ts,i,nb_item);
				scalerframe->FillStat();
				scalerframe->HeaderDisplay(
						(char*) "-- Header Ebyedat of envent number and time stamp Frame --");
				cout << "EbventNumber = " << scalerframe->GetEventNumber()
						<< "\n";
				framesize = scalerframe->GetFrameSize();
				scalerframe->DumpRaw(framesize, 0);
				verif = write(fLun, scalerframe->GetPointHeader(), framesize);
				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			scalerframe->PrintStat( "Scaler");
			cout << "-------------------END------------------------" << endl;
			delete (scalerframe);
			break;
		}
		case 11: {
			MFMMutantFrame * mutframe = new MFMMutantFrame();
			mutframe->InitStat();
			unitBlock_size = 2;
			framesize = MUT_FRAMESIZE;
			revision = 0;

			// generation of MFM header , in this case, MFM is same for all MFM frames
			mutframe->MFM_make_header(unitBlock_size, 0, MFM_MUTANT_FRAME_TYPE,
					revision, (int) (framesize / unitBlock_size), true);

			mutframe->HeaderDisplay((char*) "-- Header of Mutant Frame --");
			cout << "\n";

			// generation of nbframes frame
			for (int i = 0; i < nbframes; i++) {
				timestamp = i;
				eventnumber = i;
				mutframe->FillEventRandomConst(timestamp, eventnumber);
				mutframe->FillStat();
				cout << " pointheader = " << mutframe->GetPointHeader() << "\n";
				mutframe->DumpRaw(MUT_FRAMESIZE, 0);

				verif = write(fLun, mutframe->GetPointHeader(), framesize);

				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");

				eventnumber++;
				timestamp = eventnumber;

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			mutframe->PrintStat( "Mutant");
			cout << "-------------------END------------------------" << endl;
			delete (mutframe);
			break;
		}
		case 12: {
			MFMRibfFrame * myframe = new MFMRibfFrame();
			myframe->InitStat();
			unitBlock_size = 2;
			framesize = 64;
			revision = 0;

			// generation of MFM header , in this case, MFM is same for all MFM frames
			myframe->MFM_make_header(unitBlock_size, 0,
					MFM_RIBF_DATA_FRAME_TYPE, revision, (int) (framesize
							/ unitBlock_size), true);
			myframe->HeaderDisplay((char*) "-- Header of RIBF Frame --");
			myframe->SetAttributs();
			cout << "\n";
			// generation of nbframes frame
			for (int i = 0; i < nbframes; i++) {
				timestamp = i;
				eventnumber = i;
				myframe->FillEventRandomConst(timestamp, eventnumber);
				myframe->FillStat();
				cout << " pointheader = " << myframe->GetPointHeader() << "\n";
				myframe->DumpRaw(framesize, 0);

				verif = write(fLun, myframe->GetPointHeader(), framesize);

				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");
				eventnumber++;
				timestamp = eventnumber;
			}
			cout << "-----------------STATISTICS-------------------" << endl;
			myframe->PrintStat( "RIBF");
			cout << "-------------------END------------------------" << endl;
			delete (myframe);
			break;
		}

			//_____________________Merge of  Ebyedat in event number_____________________________________________________

		case 100: {
			MFMMergeFrame * mergeframe = new MFMMergeFrame();
			mergeframe->InitStat();
			unitBlock_size = 2;
			itemsize = 0;
			nbitem = 2;
			eventnumber = 0;
			headersize = MERGE_EN_HEADERSIZE;
			framesize = 0;
			revision = 1;

			int type = MFM_EBY_EN_TS_FRAME_TYPE;
			MFMEbyedatFrame * ebyframe2p[nbitem];
			for (int j = 0; j < nbitem; j++) {
				ebyframe2p[j] = new MFMEbyedatFrame();
				ebyframe2p[j]->InitStat();
			}

			// generation of 5 frame contents
			for (int i = 0; i < nbframes; i++) {
				eventnumber++;
				framesize = MERGE_EN_HEADERSIZE;
				for (int j = 0; j < nbitem; j++) {
					ebyframe2p[j]->GenerateAEbyedatExample(type, eventnumber);
					ebyframe2p[j]->FillStat();
					cout << " ---- inside frame ---- ";
					cout << " timestamp = " << ebyframe2p[j]->GetTimeStamp();
					cout << " event number = "
							<< ebyframe2p[j]->GetEventNumber() << "\n";
					framesize += ebyframe2p[j]->GetFrameSize();
					ebyframe2p[j]->DumpRaw(
							ebyframe2p[j]->GetFrameSizeAttribut(), 0);
					cout << "  Total framesize = " << framesize << "\n";
				}
				cout << "-- Layered Frame (Merge Frame) -------\n";
				mergeframe->MFM_make_header(unitBlock_size, 0,
						MFM_MERGE_EN_FRAME_TYPE, revision, (int) (framesize
								/ unitBlock_size),
						(headersize / unitBlock_size), 0, nbitem);
				mergeframe->SetAttributs();
				mergeframe->HeaderDisplay(
						(char*) "-- Header of Ebyedat Merge Frame --");
				mergeframe->ResetAdd();
				for (int j = 0; j < nbitem; j++) {
					mergeframe->AddFrame(ebyframe2p[j]);
				}
				mergeframe->SetEventNumber(eventnumber);
				mergeframe->FillStat();
				cout << "TimeStamp = " << mergeframe->GetTimeStamp() << "\n";
				cout << "EbventNumber = " << mergeframe->GetEventNumber();
				mergeframe->DumpRaw(framesize, 0);
				verif = write(fLun, mergeframe->GetPointHeader(), framesize);
				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");
			}
			cout << "-----------------STATISTICS-------------------" << endl;
			mergeframe->PrintStat( "Merge");
			for (int j = 0; j < nbitem; j++)
				ebyframe2p[j]->PrintStat( "EbyEDat");
			cout << "-------------------END------------------------" << endl;
			for (int j = 0; j < nbitem; j++)
				delete ebyframe2p[j];
			delete (mergeframe);
			break;
		}
			//_____________________ Merge of  Ebyedat in  time stamp______________________________________________________
		case 101: {
			MFMMergeFrame * mergeframe = new MFMMergeFrame();
			unitBlock_size = 2;
			itemsize = 0;
			nbitem = 2;
			eventnumber = 0;
			headersize = MERGE_TS_HEADERSIZE;
			framesize = 0;
			revision = 1;

			int type = MFM_EBY_EN_TS_FRAME_TYPE;
			MFMEbyedatFrame * ebyframe2p[nbitem];
			for (int j = 0; j < nbitem; j++) {
				ebyframe2p[j] = new MFMEbyedatFrame();
			}

			// generation of 5 frame contents
			for (int i = 0; i < nbframes; i++) {
				eventnumber++;
				framesize = MERGE_EN_HEADERSIZE;
				for (int j = 0; j < nbitem; j++) {
					ebyframe2p[j]->GenerateAEbyedatExample(type, eventnumber);
					cout << " ---- inside frame ----";
					cout << "timestamp = " << ebyframe2p[j]->GetTimeStamp();
					cout << " event number  = "
							<< ebyframe2p[j]->GetEventNumber() << "\n";
					framesize += ebyframe2p[j]->GetFrameSize();
					ebyframe2p[j]->DumpRaw(
							ebyframe2p[j]->GetFrameSizeAttribut(), 0);
					cout << "   total framesize = " << framesize << "\n";
				}
				cout << "-- Layered Frame (Merge Frame) -------\n";
				mergeframe->MFM_make_header(unitBlock_size, 0,
						MFM_MERGE_TS_FRAME_TYPE, revision, (int) (framesize
								/ unitBlock_size),
						(headersize / unitBlock_size), 0, nbitem);
				mergeframe->SetAttributs();
				mergeframe->HeaderDisplay((char*) "-- Header of Merge Frame --");
				for (int j = 0; j < nbitem; j++) {
					mergeframe->AddFrame(ebyframe2p[j]);
				}
				mergeframe->SetEventNumber(eventnumber);
				mergeframe->FillStat();
				cout << " timestamp = " << mergeframe->GetTimeStamp() << "\n";
				cout << " event number = " << mergeframe->GetEventNumber();
				mergeframe->DumpRaw(framesize, 0);
				verif = write(fLun, mergeframe->GetPointHeader(), framesize);
				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			mergeframe->PrintStat( "Merge");
			for (int j = 0; j < nbitem; j++)
				ebyframe2p[j]->PrintStat( "EbyEDat");
			cout << "-------------------END------------------------" << endl;
			for (int j = 0; j < nbitem; j++)
				delete ebyframe2p[j];
			int fLun; // Logical Unit Number
			fLun = open("./mfmfile.dat", (O_RDWR | O_CREAT | O_TRUNC), 0644);

			delete (mergeframe);
			break;
		}

		case 102: {
			MFMMergeFrame * mergeframe = new MFMMergeFrame();
			unitBlock_size = 2;
			itemsize = 0;
			nbitem = 4;
			eventnumber = 0;
			headersize = MERGE_EN_HEADERSIZE;
			framesize = 0;
			revision = 1;

			int type = MFM_COBO_FRAME_TYPE;
			MFMCoboFrame * coboframe2p[nbitem];
			for (int j = 0; j < nbitem; j++) {
				coboframe2p[j] = new MFMCoboFrame();
			}

			// generation of nbframes frame contents
			for (int i = 0; i < nbframes; i++) {
				eventnumber++;
				framesize = MERGE_EN_HEADERSIZE;
				for (int j = 0; j < nbitem; j++) {
					coboframe2p[j]->GenerateACoboExample(type, eventnumber,
							nbitem % COBO_NB_ASAD);
					cout << " ---- inside frame ----";
					cout << " timestamp = " << coboframe2p[j]->GetTimeStamp();
					cout << " eventnumber = "
							<< coboframe2p[j]->GetEventNumber() << "\n";
					framesize += coboframe2p[j]->GetFrameSize();
					int dumpsize = 16;
					dumpsize = coboframe2p[j]->GetFrameSizeAttribut();

					if (dumpsize > 128)
						dumpsize = 128;
					coboframe2p[j]->DumpRaw(dumpsize, 0);
					cout << "   total framesize = " << framesize << "\n";
				}
				cout << "-- Layered Frame (Merge Frame) -------\n";
				mergeframe->MFM_make_header(unitBlock_size, 0,
						MFM_MERGE_EN_FRAME_TYPE, revision, (int) (framesize
								/ unitBlock_size),
						(headersize / unitBlock_size), 0, nbitem);
				mergeframe->SetAttributs();
				mergeframe->HeaderDisplay((char*) "-- Header of Merge Frame --");
				for (int j = 0; j < nbitem; j++) {
					mergeframe->AddFrame(coboframe2p[j]);
				}
				mergeframe->SetEventNumber(eventnumber);
				cout << " timestamp = " << mergeframe->GetTimeStamp() << "\n";
				cout << " event number = " << mergeframe->GetEventNumber();

				verif = write(fLun, mergeframe->GetPointHeader(), framesize);
				if (verif != framesize)
					Error.TreatError(2, 0, "Error of write");

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			mergeframe->PrintStat( "Merge");
			for (int j = 0; j < nbitem; j++)
				coboframe2p[j]->PrintStat( "Cobo");
			cout << "-------------------END------------------------" << endl;
			for (int j = 0; j < nbitem; j++)
				delete coboframe2p[j];
			delete (mergeframe);
			break;
		}
		case 200: {
			int sizeoftrace = 4048;
			int readsize;
			string filename;
			char* vector, *vector1, *vector2;
			char ** pvector;
			vector = (char*) (malloc(sizeoftrace));
			pvector = &vector;
			vector1 = (char*) (malloc(sizeoftrace));
			vector2 = (char*) (malloc(sizeoftrace));
			int type = MFM_EBY_EN_FRAME_TYPE;
			int fLun2;
			filename = "mfmfile2.dat";
			MFMEbyedatFrame * ebyframe = new MFMEbyedatFrame();
			cout << "File name to convert in buffer of " << sizeoftrace
					<< " points?\n";
			//cin >> filename;
			cout << "filename  == " << (filename.data()) << "\n";
			fLun2 = open(filename.data(), (O_RDONLY));
			if (fLun <= 0) {
				Error.TreatError(2, 0, "Error open file :", filename.data());
				exit(0);
			}
			uint32_t unitBlock_size = 0;
			uint32_t itemsize = 0;
			uint32_t nbitem = 0;
			uint32_t framesize = 0;
			uint32_t revision = 1;
			uint32_t headersize = EBYEDAT_EN_HEADERSIZE;
			uint16_t label = 0;
			uint16_t value = 0;
			unitBlock_size = 2;
			itemsize = 4;
			int eventnumber = 0;
			cout << " begin of transformation\n";
			while (true) {
				readsize = read(fLun2, (void*) (vector), sizeoftrace);
				cout << " readsize = " << readsize << " " << sizeoftrace
						<< "\n";
				if (readsize > 0) {
					cout << "." << flush;
					nbitem = readsize;
					ebyframe->MFM_make_header(unitBlock_size, 0, type,
							revision, readsize, (headersize / unitBlock_size),
							itemsize, nbitem);
					ebyframe->SetEventNumber(eventnumber++);
					for (int i = 0; i < nbitem; i++) {
						label = ((char*) vector)[i] >> 14;
						value = ((char*) vector)[i] & ADONIS_MSK;
						ebyframe->EbyedatSetParameters(i, label, value);
						cout << "label = " << label << " value= " << value
								<< "\n";
					}
					if (true) {
						ebyframe->HeaderDisplay(
								(char*) "-- Header of Ebyedat event number Frame --");
						cout << "TimeStamp = " << ebyframe->GetTimeStamp()
								<< "\n";
						cout << "EbventNumber = " << ebyframe->GetEventNumber()
								<< "\n";
						framesize = ebyframe->GetFrameSize();
						ebyframe->DumpRaw(64, 0);
					}
					verif = write(fLun, ebyframe->GetPointHeader(), framesize);
					if (verif != framesize)
						Error.TreatError(2, 0, "Error of write");
				}
				if (readsize != sizeoftrace)
					break;

			}
			cout << "-----------------STATISTICS-------------------" << endl;
			ebyframe->PrintStat( "EbyEDat");
			cout << "-------------------END-----------------------" << endl;
			delete (ebyframe);
			break;
		}
		default:
			cout << "This case of chose is not understood\n";
			break;

		}// end of swich
		close(fLun);

	}

	//_____________________ READ____________________________________________________________

	if ((atgc >= 2) and ((strcmp(argv[1], "read") == 0))) {

		cout << "---- READ MFMFrame from file----------\n";
		int initebydone = false;
		int maxdump = 128;
		int fLun; // Logical Unit Number
		int dumpsize = 16;
		char filename[256];

		int count_elseframe = 0;
		// usage only for conversion of coborun in a extraction of only one asad
		bool conversion = false;
		char filenameout[256];
		int choosen_frame = 0; // 0-3 => cobo0  4-7=> cobo1
		int fLunout = 0; // Logical Unit Number
		if (atgc >= 4) {
			conversion = true;
			choosen_frame = atoi(argv[3]);
			sprintf(filenameout, "fil_cobo%d_asad%d.dat", (int) (choosen_frame
					/ 4), (int) (choosen_frame % 4));
			fLunout = open(filenameout, (O_RDWR | O_CREAT | O_TRUNC), 0644);
		}

		DataParameters datapar;
		if (atgc >= 3)
			strcpy(filename, argv[2]);
		else
			strcpy(filename, "./mfmfile.dat");
		fLun = open(filename, (O_RDONLY));
		if (fLun <= 0) {
			Error.TreatError(2, 0, "Error open file :", filename);

		}

		int minsizeheader = MFM_BLOB_HEADER_SIZE; // =8
		int cont = 0;
		int nbofevent = 0;

		MFMCommonFrame * frame = new MFMCommonFrame();
		MFMCoboFrame * coboframe = new MFMCoboFrame();
		MFMExogamFrame * exoframe = new MFMExogamFrame();
		MFMEbyedatFrame * ebyframe = new MFMEbyedatFrame();
		MFMCommonFrame* insideframe = new MFMCommonFrame();
		MFMMergeFrame * mergeframe = new MFMMergeFrame();
		MFMOscilloFrame * oscilloframe = new MFMOscilloFrame();
		MFMScalerDataFrame * scalerframe = new MFMScalerDataFrame();
		MFMRibfFrame * ribfframe = new MFMRibfFrame();
		MFMMutantFrame * mutframe = new MFMMutantFrame();
		MFMXmlDataDescriptionFrame * datadescriptionframe = new MFMXmlDataDescriptionFrame();
		MFMXmlFileHeaderFrame      * headerframe = new MFMXmlFileHeaderFrame();
		char* vector;
		char ** pvector;
		int vectorsize = minsizeheader;
		vector = (char*) (malloc(vectorsize));
		pvector = &vector;
		int framesize = 0;

		scalerframe->InitStat();
		coboframe->InitStat();
		exoframe->InitStat();
		ebyframe->InitStat();
		mergeframe->InitStat();
		scalerframe->InitStat();
		oscilloframe->InitStat();
		insideframe->InitStat();
		ribfframe->InitStat();
		mutframe->InitStat();
		datadescriptionframe->InitStat();
		headerframe->InitStat();

		while (true) {
			framesize = frame->ReadInFile(&fLun, pvector, &vectorsize);
			if (framesize <= 0)
				break;
			if (log_iter) {
				cout << "\n-- Read frame in file : nb = " << nbofevent
						<< " -------------------------------------------------------\n";
			}
			nbofevent++;
			cout << "before att set" << endl;
			frame->SetAttributs();
			
			int type = frame->GetFrameTypeAttribut();
			int headersize = frame->GetHeaderSizeAttribut();
			// part of use of frame
			// example
			cout << "type: " << type << endl;
			switch (type) {
			case MFM_COBO_FRAME_TYPE: {
				coboframe->SetAttributs(frame->GetPointHeader());
				coboframe->FillStat();
				coboframe->HeaderDisplay((char*) "--Cobo Frame--");
				if (framesize < maxdump)
					dumpsize = framesize;
				else
					dumpsize = maxdump;
				frame->DumpRaw(dumpsize, 0);
				break;
			}
			case MFM_COBOF_FRAME_TYPE: {
				coboframe->SetAttributs(frame->GetPointHeader());
				coboframe->FillStat();
				if (log_iter) {
					coboframe->HeaderDisplay((char*) "--CoboFull Frame--");
					if (framesize < maxdump)
						dumpsize = framesize;
					else
						dumpsize = maxdump;
					frame->DumpRaw(dumpsize, 0);
				}
				break;
			}
			case MFM_EXO2_FRAME_TYPE: {
				exoframe->SetAttributs(frame->GetPointHeader());
				exoframe->FillStat();
				if (log_iter) {
					exoframe->HeaderDisplay();
					if (framesize < maxdump)
						dumpsize = framesize;
					else
						dumpsize = maxdump;
					frame->DumpRaw(dumpsize, 0);
				}
				break;
			}
			case MFM_EBY_EN_FRAME_TYPE:
			case MFM_EBY_TS_FRAME_TYPE:
			case MFM_EBY_EN_TS_FRAME_TYPE: {
				ebyframe->SetAttributs(frame->GetPointHeader());
				ebyframe->FillStat();
				if (log_iter) {
					if (framesize < maxdump)
						dumpsize = framesize;
					else
						dumpsize = maxdump;
					frame->DumpRaw(dumpsize, 0);
					if (!initebydone) {
						initebydone = true;
						//cout << "nb of parameters = :"<<datapar.FillFromActionFile("ACTIONS_test.CHC_PAR")<<"\n";
						//datapar.DumpListPara();
						//datapar.TestParameters();
					}

					ebyframe->HeaderDisplay();
					cout << " Couples of label/value : \n"
							<< (ebyframe->DumpData());
				}

				break;
			}
			case MFM_SCALER_DATA_FRAME_TYPE: {
				scalerframe->SetAttributs(frame->GetPointHeader());
				scalerframe->FillStat();
				if (log_iter) {
					if (framesize < maxdump)
						dumpsize = framesize;
					else
						dumpsize = maxdump;
					scalerframe->HeaderDisplay();
					frame->DumpRaw(dumpsize, 0);

					if (!initebydone) {
						initebydone = true;
					}
					cout <<scalerframe-> GetDumpTextData()
							<< (scalerframe->GetDumpData());
				}
				break;
			}
			case MFM_RIBF_DATA_FRAME_TYPE: {
				ribfframe->SetAttributs(frame->GetPointHeader());
				ribfframe->FillStat();
				if (log_iter) {
					if (framesize < maxdump)
						dumpsize = framesize;
					else
						dumpsize = maxdump;
					frame->DumpRaw(dumpsize, 0);
					if (!initebydone) {
						initebydone = true;
					}
					ribfframe->HeaderDisplay();
				}
				break;
			}
			case MFM_MUTANT_FRAME_TYPE: {
				mutframe->SetAttributs(frame->GetPointHeader());
				mutframe->FillStat();
				if (log_iter) {
					if (framesize < maxdump)
						dumpsize = framesize;
					else
						dumpsize = maxdump;
					frame->DumpRaw(dumpsize, 0);

					if (!initebydone) {
						initebydone = true;

					}
					mutframe->HeaderDisplay();
					cout << (mutframe->DumpData());
				}

				break;
			}
			case MFM_MERGE_EN_FRAME_TYPE:
			case MFM_MERGE_TS_FRAME_TYPE: {
				mergeframe->SetAttributs(frame->GetPointHeader());
				int nbinsideframe = 0;
				nbinsideframe = mergeframe->GetNbItemsAttribut();
				mergeframe->HeaderDisplay();
				mergeframe->FillStat();
				if (framesize < maxdump)
					dumpsize = framesize;
				else
					dumpsize = maxdump;
				frame->DumpRaw(dumpsize, 0);
				mergeframe->ResetReadInMem();
				for (int i = 0; i < nbinsideframe; i++) {
					mergeframe->ReadInFrame(insideframe);
					insideframe->FillStat();
					if ((insideframe->GetFrameType() == MFM_COBO_FRAME_TYPE)
							or (insideframe->GetFrameType()
									== MFM_COBOF_FRAME_TYPE)) {
						coboframe->SetAttributs(insideframe->GetPointHeader());
						coboframe->HeaderDisplay((char*) "--Inside Frame--");
						cout << " ---Coboframe Noevent--"
								<< coboframe->GetEventNumber() << "\n";
						int cobo = coboframe->CoboGetCoboIdx();
						int asad = coboframe->CoboGetAsaIdx();
						if (conversion) {
							if (((int) (choosen_frame / 4) == cobo)
									and ((int) (choosen_frame % 4) == asad)
									and (framesize > headersize)) {
								int verif = write(fLunout,
										coboframe->GetPointHeader(), framesize);
								if (verif != framesize)
									Error.TreatError(2, 0, "Error of write");
							}
						}
					}

					if (dumpsize > 64)
						dumpsize = 64;
					insideframe->DumpRaw(dumpsize, 0);
				}
				break;
			}
			case MFM_XML_DATA_DESCRIPTION_FRAME_TYPE: {
							datadescriptionframe->SetAttributs(frame->GetPointHeader());
							datadescriptionframe->FillStat();
							if (log_iter) {
								if (framesize < maxdump)
									dumpsize = framesize;
								else
									dumpsize = maxdump;
								datadescriptionframe->DumpRaw(dumpsize, 0);
								if (!initebydone) {
									initebydone = true;
								}
								datadescriptionframe->HeaderDisplay();
							}
							break;
						}
			case MFM_XML_FILE_HEADER_FRAME_TYPE: {
							headerframe->SetAttributs(frame->GetPointHeader());
							headerframe->FillStat();
							if (log_iter) {
								if (framesize < maxdump)
									dumpsize = framesize;
								else
									dumpsize = maxdump;
								frame->DumpRaw(dumpsize, 0);
								if (!initebydone) {
									initebydone = true;
								}
								headerframe->HeaderDisplay();
							}
							break;
						}

			default: {
				count_elseframe++;
				frame->HeaderDisplay();
				if (framesize < maxdump)
					dumpsize = framesize;
				else
					dumpsize = maxdump;
				frame->DumpRaw(dumpsize, 0);
				break;
			}
			}
		}
		cout << endl;
		cout << "----------------------------------------------" << endl;
		cout << "|                STATISTICS                  |" << endl;
		cout << "----------------------------------------------" << endl;
		if (coboframe->GetCountFrame() != 0) {
			coboframe->PrintStat("Cobo");
			cout << "----------------------------------------------" << endl;
		}
		if (ebyframe->GetCountFrame() != 0) {
			ebyframe->PrintStat( "EbyEdat");
			cout << "----------------------------------------------" << endl;
		}
		if (exoframe->GetCountFrame() != 0) {
			exoframe ->PrintStat("Exogam");
			cout << "----------------------------------------------" << endl;
		}
		if (scalerframe->GetCountFrame() != 0) {
			string text ="Scaler";
			scalerframe->
			PrintStat(text);
			cout << "----------------------------------------------" << endl;
		}
		if (ribfframe->GetCountFrame() != 0) {
			ribfframe->
			PrintStat( "RIBF");
			cout << "----------------------------------------------" << endl;
		}
		if (mutframe->GetCountFrame() != 0) {
			mutframe->PrintStat( "MUTANT");
			cout << "----------------------------------------------" << endl;
		}
		if (mergeframe->GetCountFrame() != 0) {
			mergeframe->PrintStat("Merge");
			insideframe->PrintStat( "InsideFrame");
		}
		if (coboframe->GetCountFrame() != 0) {
			coboframe->PrintStat( "Cobo");
			cout << "----------------------------------------------" << endl;
		}
		if (datadescriptionframe->GetCountFrame() != 0) {
			datadescriptionframe->PrintStat( "Data Description");
			cout << "----------------------------------------------" << endl;
		}
		if (headerframe->GetCountFrame() != 0) {
					headerframe->PrintStat( "Data Description");
					cout << "----------------------------------------------" << endl;
				}
		cout << "----------------------------------------------" << endl;
		cout << "Number of ElseFrame  read :" << count_elseframe << endl;
		cout << "-------------------End------------------------" << endl;

		if (frame)
			delete (frame);
		if (coboframe)
			delete (coboframe);
		if (exoframe)
			delete (exoframe);
		if (ebyframe)
			delete (ebyframe);
		if (insideframe)
			delete (insideframe);
		if (mergeframe)
			delete (mergeframe);
		if (oscilloframe)
			delete (oscilloframe);
		if (scalerframe)
			delete (scalerframe);
		if (ribfframe)
			delete (ribfframe);
		if (mutframe)
			delete (mutframe);
		if (datadescriptionframe)
			delete (datadescriptionframe);
		if (headerframe)
			delete (headerframe);
		free(vector);

	}
	cout << "\n";
}

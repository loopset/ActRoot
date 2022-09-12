#include <MExperiment.h>
#include <iostream>

using namespace std;


MExperiment::MExperiment(int argc,char **argv)
{
	if(atoi(argv[1])) runf=atoi(argv[1]);
	else
	{
		cout << "usage: >Analyse.exe first_run [last_run] [-options]" << endl;
		exit(1);
	}
	if(atoi(argv[2])) runl=atoi(argv[2]);
	else runl=runf;

	Base_Path="/home/roger/ACTAR_DATA/";
	
	VisuOpt=false;
	TreeOpt=false;
	FullTreeOpt=false;
	SingleFile=false;
	IsFastPeak=false;
	SplitRuns=-1;
  Event.hasSpecificTreatment=false;

//	Event=new MEvent();
	Unpacker=new MUnpacker();
	
	int opt;// = getopt(argc, argv, "P:V:T:F");
	while ((opt = getopt(argc, argv, "VT:FP:f:SB:"))!=-1)
	{
		switch(opt)
		{ 
			case 'V':                                        // visu mode
				VisuOpt=true;
				if(Visu==NULL) Visu=new MVisu(argc,argv);
				// Visu->SetListToDraw(optarg);
				break;
				
			case 'P':                                        // data path change
				if(optarg[0]=='-')
				{
					cout << "P option requires path name" << endl;
					exit(1);
				}
				if(optarg[0]=='+')
				{
					for(int cc=1;cc<sizeof(optarg)-1;cc++)
						Base_Path+=optarg[cc];
					char* pch=strstr(optarg,"CoBo");
					if(pch!=NULL) SplitRuns=strtol(pch+4,NULL,10);
				}
				else Base_Path=optarg;
				break;
				
			case 'T':                                        // tree writing mode
				if(!TreeOpt) TreeOpt=true;
				Tree=NULL;
				Tree_Base_Path=optarg;
				break;
				
			case 'F':                                        // full tree mode: all samples
				if(TreeOpt) FullTreeOpt=true;
				break;
			
			case 'f':                                        // single file treatment (for debug mainly)
				SingleFileName=optarg;
				SingleFile=true;
				break;
				
			case 'S':                                        // simple charge treatment (basic max search. Much faster)
				IsFastPeak=true;
				cout << "Simple charge treatment option ON" << endl;
				break;

			case 'B':                                        // simple charge treatment (basic max search. Much faster)
				Event.hasSpecificTreatment=true;
        Event.SpecificTreatmentFile=optarg;
				cout << "Specific treatment ON from file: " << optarg << endl;
				break;
			
			default:
				exit(1);
		}
	}
	
	
	cout << "Analyzing run(s): " << runf << " " << runl << " in path " << Base_Path << endl;
	
	
	if(!SingleFile) File=new MFile(Base_Path,runf,runl);
	else File=new MFile(SingleFileName);
}


MExperiment::~MExperiment()
{
	delete Unpacker;
//	delete Event;
	delete File;
}


void MExperiment::OpenNewTree(long run_number)
{
	TString TreeName;
	TreeName.Form("Tree_Run_%04d",run_number);
	Tree=new TTree("ACTAR_TTree","1st level Tree",0);
	TString TreeNameFile;
	TreeNameFile.Form("%s%s.root",Tree_Base_Path.Data(),TreeName.Data());
	if(FullTreeOpt) cout << "Full ";
	cout << "TTree " << TreeNameFile.Data() << " will be written..." << endl;
	TreeFile=new TFile(TreeNameFile.Data(),"recreate");
	Tree->Branch("data","MEventReduced",&Event.ReducedEvent,16000,0);
	Tree->Branch("TimeStamp",&Event.ReducedEvent.timestamp,"TimeStamp/L");
}

void MExperiment::CloseTree()
{
	if(Tree!=NULL)
	{
		Tree->Write();
		TreeFile->Close();
		printf("\nWriting tree in file of size = %.1f Mo\n",TreeFile->GetSize()/1024./1024.);
		delete Tree;
		Tree=NULL;
		if(TreeFile!=NULL)
		{
			delete TreeFile;
			TreeFile=NULL;
		}
	}
}

void MExperiment::TreatRuns()
{
	cout << "Treating " << File->List.size() << " Files" << endl;
	for(TString run_file :File->List)
	{
		long run_number=File->OpenNext(run_file);
		if(File->isNewRun)
		{
			if(TreeOpt) CloseTree();
			if(TreeOpt) OpenNewTree(run_number);
			Event.GetAndFillCalibCoeffs();
      Event.run_number=run_number;
		}
		
		long int total_size=0;
		MFMCommonFrame * frame = new MFMCommonFrame();

		do
		{
			bool isOK=false;

//cout << "treating event" << endl;
			gettimeofday(&tv1, NULL);

			long int fsize=0;
			fsize =  File->GetNextBuffer(frame);
//	cout << "buffer was read" << endl;
		total_size+= fsize;
			gettimeofday(&tv2, NULL);

			tdiff1 += (double) (tv2.tv_usec - tv1.tv_usec) + (double) ((tv2.tv_sec - tv1.tv_sec)*1000000 );
			tdiff1 /=2;
			
			isOK=TreatEvent(frame);
//cout << "buffer was treated" << endl;
			gettimeofday(&tv1, NULL);

			tdiff2 += (double) (tv1.tv_usec - tv2.tv_usec) + (double) ((tv1.tv_sec - tv2.tv_sec)*1000000 );
			tdiff2 /=2;


			if(total_size/(1024*1024)%100==0)
			{

			/*  cout <<  endl << "Mean  GetNextBuffer time us " <<  tdiff1  
			       << " " << "Mean TreatEvent " << tdiff2 
				<< " " << "Mean Unpacker " << tdiff3 
				<< " " << "Mean TreatBaseline " << tdiff4 
			       << endl;
			  tdiff1 = tdiff2 = tdiff3 = tdiff4 = 0;*/
        cout << "total read size = " << total_size/(1024*1024) << " Mo      Good data: " << Event.stat_good_frame << " Corrupted data stat: " << Event.stat_bad_frame << " Recovered data stat: " << Event.stat_recovered_frame << "\r";
 				cout.flush();
			}
		}
		while(!File->EOFreached);//(total_size/(1024*1024)<100);
		delete frame;
	}
}


bool MExperiment::TreatEvent(MFMCommonFrame * frame)
{
// 	gettimeofday(&tv3, NULL);

	
	Event.CoboAsad.clear();
	int EN=Unpacker->Unpack(frame,&Event);
  if(EN==-1) cerr << "Error Frame run " << Event.run_number << endl;

	Event.TreatBaseline(FullTreeOpt,IsFastPeak);
	if(VisuOpt) Visu->Draw(&Event,100);
	if(TreeOpt) Tree->Fill();

	Event.ReducedEvent.CoboAsad.clear();


	return(true);
}


void MExperiment::EndOfExperiment()
{
	if(TreeOpt)
	{
		Tree->Write();
		TreeFile->Close();
		printf("\nWriting tree in file of size = %.1f Mo\n",TreeFile->GetSize()/1024./1024.);
	}
	if(VisuOpt)
	{
		Visu->End();
	}
}

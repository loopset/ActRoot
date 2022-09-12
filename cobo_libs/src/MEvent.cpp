 #include <MEvent.h>


MEvent::MEvent()
{
	hasConfigFile=false;
	hasCalBaseline=false;
	Gsig=new TGraph(NB_SAMPLES);
	ffitSig=new TF1("ffitSig",SignalFitFunction,0,511,2);
	stat_good_frame=stat_bad_frame=0;
	CoboAsad.resize(65);
	CoboAsad.shrink_to_fit();
}


MEvent::~MEvent()
{
	delete Gsig;
	delete ffitSig;
}



void MEvent::GetAndFillCalibCoeffs()
{	
	int co, as, ag, ch;
	float val[2];
	FILE* fcal;
	TString calfile={"cal/cal_gen_.dat"};

	cout << "No config file found -> try to find cal/cal_gen.dat file..." << endl;
	fcal=fopen(calfile.Data(),"r");
	if(fcal==NULL)
	{
		cout << "Generic calibration file not found. Working with raw data" << endl;
		for(int co=0;co<NB_COBO;co++) for(int as=0;as<NB_ASAD;as++) for(int ag=0;ag<NB_AGET;ag++) for(int ch=0;ch<NB_CHANNEL;ch++)
		{
			CalibCoefs[co][as][ag][ch][0]=1.;
			CalibCoefs[co][as][ag][ch][1]=0.;
		}
	}
	else
	{
 	cout << "Using generic calibration coefficients" << endl;
		while(fscanf(fcal,"%d %d %d %d %f %f",&co,&as,&ag,&ch,&val[0],&val[1])!=EOF)
		{
			if(co<NB_COBO && as<NB_ASAD && ch<NB_CHANNEL)
				for(int i=0;i<2;i++)
					CalibCoefs[co][as][ag][ch][i]=val[i];
			else cout << "calibration file corrupted: cobo/asad/aget/channel exceeds max value of 15/3/3/67" << endl;
		}
		fclose(fcal);
	}
	
	fcal=fopen("cal/BL_.dat","r");
	if(fcal==NULL)
  {
    cout << "Baseline calibration file not found -> No Baseline subtraction." << endl;
    for(int co=0;co<NB_COBO;co++) for(int as=0;as<NB_ASAD;as++) for(int ag=0;ag<NB_AGET;ag++) for(int ch=0;ch<NB_CHANNEL;ch++) for(int bu=0;bu<NB_SAMPLES;bu++)
  	  BaselineCal[co][as][ag][ch][bu]=0;
  }
  else
	{
		cout << "Baseline subtraction ON from cal/BL.dat" << endl;
		hasCalBaseline=true;
		int co, as, ag, ch;
		while(fscanf(fcal,"%d %d %d %d",&co,&as,&ag,&ch)!=EOF)
		{
			if(co<NB_COBO && as<NB_ASAD && ag<NB_AGET && ch<NB_CHANNEL)
			{
				for(int bu=0;bu<NB_SAMPLES;bu++)
					fscanf(fcal,"%f",&BaselineCal[co][as][ag][ch][bu]);
			}
			else cout << "Wrong global channel number for baseline calibration: " << co << " " << as << " " << ag << " " << ch << endl;
		}
		fclose(fcal);
	}
  
  
  if(hasSpecificTreatment)
  {
    for(int co=0;co<NB_COBO;co++) for(int as=0;as<NB_ASAD;as++) for(int ag=0;ag<NB_AGET;ag++) for(int ch=0;ch<NB_CHANNEL;ch++)
      SpecificTreatment[co][as][ag][ch]=0;
    fcal=fopen(SpecificTreatmentFile,"r");
    int algonum;
    if(fcal==NULL)
    {
      hasSpecificTreatment=false;
      cout << "Warning: specific treatment file : " << SpecificTreatmentFile << " does not exist" << endl;
    }
    else while(fscanf(fcal,"%d %d %d %d %d",&co,&as,&ag,&ch, &algonum)!=EOF);
		  SpecificTreatment[co][as][ag][ch]=algonum;
  }
}



void MEvent::InitVXIParameters(char* ActionFName,std::vector<TString> ParNames,std::vector<int> Numbers)
{
	TString PName;
	parametersVXI.FillFromActionFile(ActionFName);
	
	for(int i=0;i<16384;i++)
		labelVXI[i]=-1;
	
	for(int it=0;it<ParNames.size();it++)
	{
		if(Numbers[it])
		{
			for(int noc=0;noc<Numbers[it];noc++)
			{
				PName.Form("%s%d",ParNames[it].Data(),noc);
				labelVXI[parametersVXI.GetLabel(PName.Data())]=it*1000+noc;
			}
		}
		else labelVXI[parametersVXI.GetLabel(ParNames[it].Data())]=it*1000;
	}
}



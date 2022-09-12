///////////////////////////////////////
//                                   //
// T. Roger - GANIL 2015             //
// email: roger@ganil.fr             //
//                                   //
// MVisu class file:                 //
//                                   //
///////////////////////////////////////

#include <MVisu.h>

using namespace std;

MVisu::MVisu(int argc,char **argv)
{
	App=new TApplication("MyApp",&argc, argv);
	App->ProcessFile("src/rootlogon.C");
	
	Canv=new TCanvas("Canv","Canv",800,500);
	PadSummary=new TH2F("PadSummary","PadSummary",68*4*4*16,0,68*4*4*16,1024,0,4096);
	for(int i=0;i<2;i++) hBLShape[i]=new TH2S("BLShape","BLShape",512,0,512,400,-100,4000);
}

MVisu::~MVisu()
{
	delete Canv;
// 	for(int i=0;i<68*4/**4*16*/;i++) delete hBLShape[i];
	delete PadSummary;
// 	delete BLMSummary;
// 	delete BLSSummary;
	delete App;
}

void MVisu::SetListToDraw(char* fileName)
{	
	FILE* fList=fopen(fileName,"r");
	if(!fList)
	{
		cout << "Visu list file: " << fileName << " does not exist" << endl;
	}
	else
	{
		fclose(fList);
	}
}


void MVisu::Draw(MEvent* Event, int Update)
{	
	for(MCoboAsad CoAs : Event->CoboAsad)
	{
		int co = CoAs.cobo_number;
		int as = CoAs.asad_number;

		for(unsigned short Ch : CoAs.hit_pattern)
		{
			int ag = (int)(Ch)/NB_CHANNEL;
			int ch = (int)(Ch)%NB_CHANNEL;

			if(CoAs.Channel[Ch].Sample_Number.size()>0)//==NB_SAMPLES)
			{
				if(ch<NB_CHANNEL && ag<NB_AGET && co<NB_COBO && as<NB_ASAD) for(unsigned short Bu : CoAs.Channel[Ch].Sample_Number) if(Bu<NB_SAMPLES)
				{
					if(co==14 && as==2 && ag==0 && ch==55) hBLShape[0]->Fill((Bu)%512,CoAs.Channel[Ch].Raw_Sample[Bu]-Event->BaselineCal[co][as][ag][ch][Bu]);
					if(co==3 && as==0 && ag==0 && ch==67) hBLShape[1]->Fill((Bu)%512,CoAs.Channel[Ch].Raw_Sample[Bu]-Event->BaselineCal[co][as][ag][ch][Bu]);
					
				}
			}
			
		}
	}
	
// 	for(ReducedData EvtRed : Event->ReducedEvent->CoboAsad)
// 	{
// 		int co=EvtRed.globalchannelid>>11;
// 		int as=(EvtRed.globalchannelid - (co<<11))>>9;
// 		int ag=(EvtRed.globalchannelid - (co<<11)-(as<<9))>>7;
// 		int ch=EvtRed.globalchannelid - (co<<11)-(as<<9)-(ag<<7);
// 		
// 		if(ch<68 && ag<4 && co<16 && as<4)
// 		{
// // 			cout << co << " " << as << " " << ag << " " << ch << "  " << EvtRed.globalchannelid << " " << EvtRed.peakheight[0] << endl;
// 			PadSummary->Fill(EvtRed.globalchannelid,EvtRed.peakheight[0]);
// 		}
// 	}
	
}


void MVisu::End()
{
// 	FILE* f=fopen("utils/test/PulseShape_co1as3ag1ch3_3.dat","w");
// // 	for(int co=0;co<16;co++) for(int as=0;as<4;as++) for(int ag=0;ag<4;ag++) for(int ch=0;ch<68;ch++)
// // 	{
// // 		fprintf(f,"%d %d %d %d\t\t",co,as,ag,ch);
// 		for(int bin=1;bin<=512;bin++)
// 		{
// 			fprintf(f,"%.1f ",hBLShape[0]->ProjectionY("",bin,bin)->GetMean());
// 		}
// 		fprintf(f,"\n");
// // 	}
// 	fclose(f);
// 	
// 	f=fopen("utils/test/PulseShape_co1as3ag1ch4_3.dat","w");
// // 	for(int co=0;co<16;co++) for(int as=0;as<4;as++) for(int ag=0;ag<4;ag++) for(int ch=0;ch<68;ch++)
// // 	{
// // 		fprintf(f,"%d %d %d %d\t\t",co,as,ag,ch);
// 		for(int bin=1;bin<=512;bin++)
// 		{
// 			fprintf(f,"%.1f ",hBLShape[1]->ProjectionY("",bin,bin)->GetMean());
// 		}
// 		fprintf(f,"\n");
// // 	}
// 	fclose(f);
	
	Canv->Divide(2,1);
	Canv->cd(1);
     hBLShape[0]->Draw("colz");
	Canv->cd(2);
	hBLShape[1]->Draw("colz");
	Canv->Update();
	App->Run();

}

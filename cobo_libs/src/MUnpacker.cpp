#include <iostream>
#include <MUnpacker.h>
#include <MEvent.h>

using namespace std;

MUnpacker::MUnpacker()
{
	insideframe=new MFMCommonFrame();
	coboframe=new MFMCoboFrame();
	mutantframe=new MFMMutantFrame();
	ebyedatframe=new MFMEbyedatFrame();
	mergeframe=new MFMMergeFrame();
}

MUnpacker::~MUnpacker()
{
	delete insideframe;
	delete mergeframe;
	delete coboframe;
	delete mutantframe;
	delete ebyedatframe;
}


long int MUnpacker::Unpack(MFMCommonFrame* frame, MEvent* Event)
{
	frame->SetAttributs();
	int type=frame->GetFrameTypeAttribut();
	int headersize = frame->GetHeaderSizeAttribut();

	if(type == MFM_XML_FILE_HEADER_FRAME_TYPE) return(0);
	if(type == MFM_COBOT_FRAME_TYPE)
	{
//		printf("Topology Frame:\n");
//		frame->HeaderDisplay(NULL);
//		frame->DumpRaw(256);
//		cout << endl;
		return(0);
	}
		
	else if((type == MFM_MERGE_EN_FRAME_TYPE) || (type == MFM_MERGE_TS_FRAME_TYPE))
	{
		mergeframe->SetAttributs(frame->GetPointHeader());
		
		int nbinsideframe=mergeframe->GetNbItemsAttribut();
    long int Merge_EN = mergeframe->GetEventNumber();
    long int Merge_TS = mergeframe->GetTimeStamp();
		Event->EN = Merge_EN;
		Event->TS = Merge_TS;
		mergeframe->ResetReadInMem();

		for(int i1=0;i1<nbinsideframe;i1++)
		{
			mergeframe->ReadInFrame(insideframe);
			long int inside_EN=Unpack(insideframe,Event);
      if(inside_EN!=Merge_EN) cout << "Bad Merged Frame with EN = " << inside_EN << " instead of Merged header EN = " << Merge_EN << endl;
		}
		if(Event->CoboAsad.size()==nbinsideframe) return(nbinsideframe);
		else return(0);
	}
	
	else if ((type==MFM_COBO_FRAME_TYPE) || (type==MFM_COBOF_FRAME_TYPE))
	{	
		coboframe->SetAttributs(frame->GetPointHeader());
		
		nbitems=coboframe->GetNbItems();
		
		MCoboAsad TheCoboAsad;

		TheCoboAsad.cobo_number=coboframe->CoboGetCoboIdx();
		TheCoboAsad.asad_number=coboframe->CoboGetAsaIdx();
		TheCoboAsad.global_asad_number=TheCoboAsad.cobo_number*COBO_NB_ASAD+TheCoboAsad.asad_number;
		TheCoboAsad.EN=Event->EN=coboframe->GetEventNumber();
		TheCoboAsad.TS=Event->TS=coboframe->GetTimeStamp();
		 
		for(int ag=0;ag<4;ag++)
		{
			for(int j=0;j<4;j++)
      {
        TheCoboAsad.hit_pattern_tab[ag*NB_CHANNEL + 3-j]=(((int)(*(coboframe->CoboGetHitPat(ag)))>>j)&1);
				if(((int)(*(coboframe->CoboGetHitPat(ag)))>>j)&1)
					TheCoboAsad.hit_pattern.push_back(ag*NB_CHANNEL + 3-j);
			}
      for(int i=1;i<9;i++)
        for(int j=0;j<8;j++)
        {
          TheCoboAsad.hit_pattern_tab[ag*NB_CHANNEL + (i-1)*8+11-j]=(((int)(*(coboframe->CoboGetHitPat(ag)+i))>>j)&1);
					if(((int)(*(coboframe->CoboGetHitPat(ag)+i))>>j)&1)
						TheCoboAsad.hit_pattern.push_back(ag*NB_CHANNEL + (i-1)*8+11-j);
			  }
			
			TheCoboAsad.last_cell[ag]=((*((uint16_t*)(coboframe->CoboGetLastCell(ag))))&511);
			TheCoboAsad.multiplicity[ag]=*((uint16_t*)(coboframe->CoboGetMultip(ag)));
// 			cout << TheCoboAsad.EN << " -  ";
// 			for(int ih=0;ih<2;ih++) for(int jh=0;jh<8;jh++) cout <<(((int)(*(coboframe->CoboGetLastCell(ag)+ih))>>jh)&1) << " " ;
// 			cout << "     " << TheCoboAsad.last_cell[ag] << endl;
		}	
		
		
		short iChan=0;
		short iBuck=0;
		short iAget=0;
		
		
		for(int i2=0;i2<nbitems;i2++)
		{
			coboframe->CoboGetParameters(i2,&sample, &buckidx,&chanidx,&agetidx);

			if(agetidx>3 || chanidx>67 || buckidx>511)
			{
 // 				cout << "WARNING: Corrupted Data with cobo " <<  TheCoboAsad.cobo_number << " - asad " << TheCoboAsad.asad_number << " - aget " << agetidx << " - channel " << chanidx << " - bucket " << buckidx << "  with value " << sample << "  at event #" << TheCoboAsad.EN << "  with frame size " << nbitems << endl;
				if(!CheckAndRepairCorruptedFrame(frame,TheCoboAsad)) Event->stat_bad_frame++;
				else Event->stat_recovered_frame++;
				break;			
			}
			
			else if(type==MFM_COBO_FRAME_TYPE)
			{
				Event->stat_good_frame++;
        if(Event->hasSpecificTreatment)
        {
          if(sample>Event->SpecificTreatment[coboframe->CoboGetCoboIdx()][coboframe->CoboGetAsaIdx()][agetidx][chanidx])
          {
            TheCoboAsad.Channel[agetidx*NB_CHANNEL+chanidx].Raw_Sample[buckidx]=sample;
				    TheCoboAsad.Channel[agetidx*NB_CHANNEL+chanidx].Sample_Number.push_back(buckidx);
          }
				}
        else
        {
          TheCoboAsad.Channel[agetidx*NB_CHANNEL+chanidx].Raw_Sample[buckidx]=sample;
				  TheCoboAsad.Channel[agetidx*NB_CHANNEL+chanidx].Sample_Number.push_back(buckidx);
        }
        if(!TheCoboAsad.hit_pattern_tab[agetidx*NB_CHANNEL + chanidx])
        {
          TheCoboAsad.hit_pattern.push_back(agetidx*NB_CHANNEL + chanidx);
          TheCoboAsad.hit_pattern_tab[agetidx*NB_CHANNEL + chanidx]=1;
			  }
      }

			else if(type==MFM_COBOF_FRAME_TYPE)
			{							
				Event->stat_good_frame++;
        if(sample>Event->SpecificTreatment[coboframe->CoboGetCoboIdx()][coboframe->CoboGetAsaIdx()][iAget][iChan])
        {
				  TheCoboAsad.Channel[agetidx*NB_CHANNEL+iChan].Raw_Sample[iBuck]=sample;
				  TheCoboAsad.Channel[agetidx*NB_CHANNEL+iChan].Sample_Number.push_back(iBuck);
				}
        if(!TheCoboAsad.hit_pattern_tab[iAget*NB_CHANNEL + iChan])
        {
          TheCoboAsad.hit_pattern.push_back(iAget*NB_CHANNEL + iChan);
          TheCoboAsad.hit_pattern_tab[iAget*NB_CHANNEL + iChan]=1;
        }
				iChan++;
        
				if(i2%2==1)
				{
					iAget++;
					iChan-=2;
				}
				if(iAget>=NB_AGET)
				{
					iAget=0;
					iChan+=2;
				}
				if(iChan>=NB_CHANNEL)
				{
					iBuck++;
					iChan=0;
				}
			}
		}
		
		Event->CoboAsad.push_back(TheCoboAsad);		

		return(TheCoboAsad.EN);
	}
	
	else if(type == MFM_MUTANT_FRAME_TYPE)
	{
		mutantframe->SetAttributs(frame->GetPointHeader());
//  		event = mutantframe->GetEventNumber();
// 		timestamp = mutantframe->GetTimeStamp();
		return(mutantframe->GetEventNumber());
		
	}
	
	else if(type == MFM_EBY_EN_TS_FRAME_TYPE)
	{
		ebyedatframe->SetAttributs(frame->GetPointHeader());
		
		MCoboAsad TheCoboAsad;
		
		TheCoboAsad.hit_pattern.push_back(0);
		
		TheCoboAsad.cobo_number=31;
		TheCoboAsad.asad_number=TheCoboAsad.global_asad_number=0;
		TheCoboAsad.global_asad_number=TheCoboAsad.cobo_number*COBO_NB_ASAD+TheCoboAsad.asad_number;
		TheCoboAsad.EN=Event->EN=ebyedatframe->GetEventNumber();
		TheCoboAsad.TS=Event->TS=ebyedatframe->GetTimeStamp();

		uint16_t label, value;
		for(int i=0;i<ebyedatframe->GetNbItems();i++)
		{
			ebyedatframe->EbyedatGetParameters(i,&label,&value);
			TheCoboAsad.Channel[(int)(i/NB_SAMPLES)].Raw_Sample[i%NB_SAMPLES]=value+(label<<16);
			TheCoboAsad.Channel[(int)(i/NB_SAMPLES)].Sample_Number.push_back(i%NB_SAMPLES);
//			TheCoboAsad.Channel[0].Raw_Sample[i]=value+(label<<16);
//			TheCoboAsad.Channel[0].Sample_Number.push_back(i);
// 			if(TheCoboAsad.EN==203788) cout << TheCoboAsad.EN << " " << label << " " << value << "   " << i << " " << TheCoboAsad.Channel[0].Raw_Sample[i] << " " << TheCoboAsad.Channel[0].Sample_Number.back() << endl;
		}

		Event->CoboAsad.push_back(TheCoboAsad);
		
		return(ebyedatframe->GetEventNumber());
	}

	
	else if (type != 0xFF00)
	{
		printf("\nUnknown frame of type %d (0x%x) and size %d (0x%x)\n",type,type,frame->GetFrameSize(),frame->GetFrameSize());
		frame->HeaderDisplay(NULL);
		frame->DumpRaw(128,0);
		cout << endl;
		return(-1);
	}
}


bool MUnpacker::CheckAndRepairCorruptedFrame(MFMCommonFrame* frame, MCoboAsad& TheCoboAsad)
{
	std::vector <short> aget_channel_present={};
	coboframe->CoboGetParameters(0,&sample, &buckidx,&chanidx,&agetidx);
	TheCoboAsad.Channel[agetidx*NB_CHANNEL+chanidx].Raw_Sample[buckidx]=sample;
	TheCoboAsad.Channel[agetidx*NB_CHANNEL+chanidx].Sample_Number.push_back(buckidx);
	aget_channel_present.push_back(agetidx*NB_CHANNEL+chanidx);
	unsigned short first_bucket=buckidx;
	int index=0;
	for(int i2=1;i2<nbitems;i2++)
	{
		coboframe->CoboGetParameters(i2,&sample, &buckidx,&chanidx,&agetidx);
		index++;
		if(buckidx==first_bucket) aget_channel_present.push_back(agetidx*NB_CHANNEL+chanidx);
		if(chanidx>67) chanidx=aget_channel_present[index%aget_channel_present.size()]%NB_CHANNEL;
		else if(agetidx>3) return(false);
		else if(buckidx>NB_SAMPLES-1) return(false);
		TheCoboAsad.Channel[agetidx*NB_CHANNEL+chanidx].Raw_Sample[buckidx]=sample;
		TheCoboAsad.Channel[agetidx*NB_CHANNEL+chanidx].Sample_Number.push_back(buckidx);
	}
	return(true);
}


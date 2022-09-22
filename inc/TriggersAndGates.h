#ifndef TRIGGERSANDGATES_H
#define TRIGGERSANDGATES_H

struct TriggersAndGates
{
	int INCONF {};
	int GATCONF {};
	int TVAL_CFA_HF    {};
	int TVAL_CFA_CATD4 {};
	int TVAL_CFA_CATD6 {};

	unsigned long DT_CLK_UP_UP {};
	unsigned long DT_CLK_UP {};
	unsigned long DT_CLK {};
	unsigned long DT_GET_UP_UP {};
	unsigned long DT_GET_UP {};
	unsigned long DT_GET {};
	unsigned long DT_VXI_UP_UP {};
	unsigned long DT_VXI_UP {};
	unsigned long DT_VXI {};

	unsigned long CTR_TIMEH_UP {};
	unsigned long CTR_TIMEH {};
	unsigned long CTR_TIMEML_UP {};
	unsigned long CTR_TIMEML {};
	unsigned long CTR_EVT_UP {};
	unsigned long CTR_EVT {};

	unsigned long SCA_TiD3_UP {};
	unsigned long SCA_TiD3 {};	
	unsigned long SCA_CATSD4_UP {};
	unsigned long SCA_CATSD4 {};	
	unsigned long SCA_CATSD5_UP {};
	unsigned long SCA_CATSD5 {};	
	unsigned long SCA_CFA_UP {};
	unsigned long SCA_CFA {};	
	unsigned long SCA_SiBeam1_UP {};
	unsigned long SCA_SiBeam1 {};
	unsigned long SCA_SiBeam2_UP {};
	unsigned long SCA_SiBeam2 {};	
	unsigned long SCA_Si0_UP {};
	unsigned long SCA_Si0 {};
	unsigned long SCA_SiS_UP {};
	unsigned long SCA_SiS {};
	unsigned long SCA_L1ok_UP {};
	unsigned long SCA_L1ok {};

	TriggersAndGates() = default;
	~TriggersAndGates() = default;
};

#endif

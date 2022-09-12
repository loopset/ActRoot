#define MFM_MIN_TYPE  0x01

#define MFM_COBO_FRAME_TYPE     0x01 /// Cobo card frame
#define MFM_COBOF_FRAME_TYPE    0x02 /// Cobo card frame full signals
#define MFM_COBOT_FRAME_TYPE    0x07 /// Cobo topology frame
#define MFM_MUTANT_FRAME_TYPE   0x08 /// Mutant Data Frame
#define MFM_MUTANT1_FRAME_TYPE  0x09 /// Mutant Data Frame Reserved
#define MFM_MUTANT2_FRAME_TYPE  0x0A /// Mutant Data Frame Reserved
#define MFM_MUTANT3_FRAME_TYPE  0x0B /// Mutant Data Frame Reserved

#define MFM_EXO2_FRAME_TYPE     0x10 /// numexo2 card frame
#define MFM_OSCI_FRAME_TYPE     0x11 /// Oscilloscope data frame use in Numexo2
#define MFM_RAWD_FRAME_TYPE     0x12 /// Raw data frame use in NEDA
#define MFM_COMP_FRAME_TYPE     0x13 /// Compressed data frame use in NEDA
#define MFM_DIAMANT_FRAME_TYPE  0x14 /// Diamant Frame

#define MFM_EBY_EN_FRAME_TYPE   0x20 /// Ganil data frame with event number
#define MFM_EBY_TS_FRAME_TYPE   0x21 /// Ganil data frame with time stamp
#define MFM_EBY_EN_TS_FRAME_TYPE    0x22 /// Ganil data frame with time stamp and event number
#define MFM_MATACQ_FRAME_TYPE       0x23 /// Mataq card frame
#define MFM_SCALER_DATA_FRAME_TYPE  0x24 /// Ganil Scaler data frame

#define MFM_RIBF_DATA_FRAME_TYPE    0x30 /// RIBF data frame

#define MFM_FAZIA_DATA_FRAME_TYPE   0x40 /// FAZIA data frame

#define MFM_CHIMERA_DATA_FRAME_TYPE 0x60 /// CHIMERA data frame

#define MFM_HELLO_FRAME_TYPE  0xFF00     /// Hello Frame
#define MFM_MERGE_EN_FRAME_TYPE  0xFF01  /// Merge frame in envent number
#define MFM_MERGE_TS_FRAME_TYPE  0xFF02  /// Merge frame in time stamp

#define MFM_XML_DATA_DESCRIPTION_FRAME_TYPE 0xFF10  /// Data description of few data
#define MFM_XML_FILE_HEADER_FRAME_TYPE      0xFF11  /// Frame of run description
#define MFM_MAX_TYPE  						0xFF11

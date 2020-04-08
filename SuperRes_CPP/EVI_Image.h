#pragma once
#define MAX_STRING_SIZE_FOR_EVI_HEADER 200

#define EVI_ERC_SUCCESS  1
#define EVI_ERC_BAD_FILENAME  0
#define EVI_ERC_WRONG_DIMENSION  -1
#define EVI_ERC_WRONG_HEADER_SIZE  -2  //sequence or frame header size
#define EVI_ERC_WRONG_ENERGY_MODE  -3  
#define EVI_ERC_WRONG_ENERGY_TYPE  -4 
#define EVI_ERC_WRONG_IMAGE_TYPE  -5 
#define EVI_ERC_WRONG_FRAME_RATE  -6 
#define EVI_ERC_WRONG_PIXEL_TYPE  -7 
#define EVI_ERC_WRONG_NUM_BOARDS  -8 
#define EVI_ERC_PROBLEM_WRITING_TO_FILE  -9
#define EVI_ERC_BAD_SEQUENCE_HEADER  -10

#define EVI_TEXT_HEADER_BYTES 2048

struct EVI_Header
{
	char Image_Type[MAX_STRING_SIZE_FOR_EVI_HEADER];  //Hydra, Thor, etc
	int Width;
	int Height;
	int Total_Height; //if TDS is used then we need to know the actual height after frames are stitched together
	int Nr_of_images;
	int Acquisition_Time_us;
	int Sequence_Header_Bytes; //can be 0
	int Gap_between_images_in_bytes; //frame descriptor
	int Frame_Header_Bytes; //same as Gap_between_iamges_in_bytes
	unsigned int Offset_To_First_Image;
	float Frame_Rate_HZ;
	int PlainText_Header_Bytes;
	int Frame_Bytes; 
	char Energy_type[MAX_STRING_SIZE_FOR_EVI_HEADER]; //TOTAL_ENERGY or HIGH_ENERGY
	char Energy_Mode[MAX_STRING_SIZE_FOR_EVI_HEADER]; // SINGLE_ENERGY or DUAL_ENERGY
	char Guid[100];
	int Active_Detector_Rows; //sensor height
	int Active_Detector_Cols; //detector active width
	char Sw_Program[MAX_STRING_SIZE_FOR_EVI_HEADER];
	char Sw_Version[MAX_STRING_SIZE_FOR_EVI_HEADER];
	char EmbSw_Version[MAX_STRING_SIZE_FOR_EVI_HEADER];
	char Fw_Version[MAX_STRING_SIZE_FOR_EVI_HEADER];
	int Low_Thresholds_KV;
	int High_Thresholds_KV;
	char Start_Time_Stamp[MAX_STRING_SIZE_FOR_EVI_HEADER];
	char Stop_Time_Stamp[MAX_STRING_SIZE_FOR_EVI_HEADER];
	char Anti_Coincidence[10]; //ON or OFF
	int Binning; //1, 2, 4 or 8
	int Nr_of_Pixels_Per_Bin;
	char Device_Type[MAX_STRING_SIZE_FOR_EVI_HEADER];
	char Device_Serial_Number[MAX_STRING_SIZE_FOR_EVI_HEADER];
	int Bits_per_Pixel; //8, 16, 32
	char Tds[10] ; //True or False
	char Tds_Truncate_to_015_TE[10]; //True or False
	char Tds_Truncate_to_015_HE[10]; //True or False
	int Tds_Step;
	int Tds_Prescaler_step;
	int Sum_Step;
	int Scan_Frame_Count;
	int Time_After_Startup_s;
	int Time_After_HvOn_s;
	int Time_After_Prev_Seq_s;
	int HV_T1;
	int HV_T2;
	int HV_TC;
	int Number_of_boards;
	int Number_of_board_rows;
	int Position_of_Master;
	int Number_of_asics;
	int PlaintextHeader_Version;
	float TUBE_VOLTAGE_KV;
	float TUBE_CURRENT_MA;
	char FILTER_MATERIAL[MAX_STRING_SIZE_FOR_EVI_HEADER];
	float FILTER_THICKNESS_MM;
	float SID_CM;
	float SOD_CM;
	float SPEED_MM_PER_S;
	int PULSE_LENGTH_MS;
	char CCR_Type[MAX_STRING_SIZE_FOR_EVI_HEADER];
	char KV_Conversion_Site[MAX_STRING_SIZE_FOR_EVI_HEADER];
};

int ReadImageHeader_EVI(const char *filename, EVI_Header *pHeader);
unsigned char* ReadImage_EVI(const char *filename, bool bIncludeFrameHeader);
unsigned char* ReadSequenceHeader_EVI(const char *filename);
int WriteImage_EVI(const char *filename, unsigned char *pSequenceHeader, unsigned char *pData, const EVI_Header *pHeader); 

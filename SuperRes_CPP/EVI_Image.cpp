
#include "stdafx.h"
#include "EVI_Image.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <string>
#include <locale>
//-----------------------------------------------------
std::string & ltrim(std::string & str)
{
  auto it2 =  std::find_if( str.begin() , str.end() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( str.begin() , it2);
  return str;   
}

//-------------------------------------------------------------------------
std::string & rtrim(std::string & str)
{
  auto it1 =  std::find_if( str.rbegin() , str.rend() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( str.begin() + (  str.rend() - it1 ) , str.end() );
  return str;   
}
//----------------------------------------------------
//trim strig
//--------------------------------------------------------------
std::string & TrimString(std::string & str)
{
   return ltrim(rtrim(str));
}

//------------------------------------------------------------------
bool ExtractIntFromString_2(const std::string str, std::string tag, int *pVal)
{
	bool bRes = true;
	int iPos = (int)str.find(tag);
	if ( iPos <0 ) return false;
	int iPos2 = str.find("\r", iPos);
	int iPos3 = str.find("\n", iPos);
	if ( iPos2 < 0 && iPos3 < 0 ) return false;
	if ( iPos3 > iPos2) iPos2 = iPos3;
	std::string sub = str.substr(iPos+(int)+tag.size(), iPos2-iPos-tag.size());
	*pVal = atoi(sub.c_str());
	return bRes;
}

//------------------------------------------------------------------
bool ExtractIntFromString(const char *str, const char *tag, int *pVal)
{
	return ExtractIntFromString_2(str, tag, pVal);
}

//------------------------------------------------------------------
bool ExtractStringFromString(const std::string str, std::string tag, char *pVal)
{
	bool bRes = true;
	int iPos = (int)str.find(tag);
	if ( iPos <0 ) return false;
	int iPos2 = str.find("\r", iPos);
	int iPos3 = str.find("\n", iPos);
	if ( iPos2 > iPos && iPos3 > iPos )
	{
		if ( iPos2 > iPos3 ) iPos2 = iPos3;
	}
	else if ( iPos3 > iPos ) iPos2 = iPos3;
	else if ( iPos2 > iPos ) iPos2 = iPos2;
	else return false;
	//if ( iPos2 < 0 && iPos3 < 0 ) return false;
	if ( iPos2 < iPos) return false;
	
	std::string sub = str.substr(iPos+(int)+tag.size(), iPos2-iPos-tag.size());
	sub = TrimString(sub);
	memcpy(pVal, sub.c_str(), (int)sub.size());
	pVal[(int)sub.size()] = '\0';
	//*pVal = (float)atof(sub.c_str());
	return bRes;
}

//------------------------------------------------------------------
bool ExtractFloatFromString(const std::string str, std::string tag, float *pVal)
{
	
	bool bRes = true;
	int iPos = (int)str.find(tag);
	if ( iPos <0 ) return false;
	int iPos2 = str.find("\r", iPos);
	int iPos3 = str.find("\n", iPos);
	if ( iPos2 < 0 && iPos3 < 0 ) return false;
	if ( iPos3 > iPos2) iPos2 = iPos3;
	std::string sub = str.substr(iPos+(int)+tag.size(), iPos2-iPos-tag.size());
	
	*pVal = (float)atof(sub.c_str());
	return bRes;
}


//------------------------------------------------------------------------------------------
int ReadImageHeader_EVI(const char *filename, EVI_Header *pHeader)
{
	const int iMaxTextHeader = 8192;
	int bRes = 1;
	FILE *fin = nullptr;
	fopen_s(&fin, filename, "rb");
	if ( fin == nullptr ) 
	{
		printf("Problem reading file %s", filename);
		return 0;
	}

	fseek(fin, 0L, SEEK_END);
	long sz = ftell(fin);
	
	//text header is at least 1024
	if ( sz < 2048 ) {bRes=EVI_ERC_WRONG_DIMENSION; goto MAIN_EXIT;}


	char strTextHeader[iMaxTextHeader];
	fseek(fin, 0, SEEK_SET);
	fread(strTextHeader, 1, 2048, fin);
	pHeader->PlainText_Header_Bytes = 2048;
	int iVal = -1;
	
	if ( ExtractIntFromString_2(strTextHeader, "PlainText_Header_Bytes", &iVal) )
	{
		if ( iVal > 0 ) pHeader->PlainText_Header_Bytes = iVal;
		else {bRes=EVI_ERC_WRONG_HEADER_SIZE; goto MAIN_EXIT;}
	}

	//reload the text header with the given byte size
	if ( pHeader->PlainText_Header_Bytes < iMaxTextHeader && pHeader->PlainText_Header_Bytes < sz)
	{
		fseek(fin, 0, SEEK_SET);
		fread(strTextHeader, 1, pHeader->PlainText_Header_Bytes, fin);
	}
	else{bRes=EVI_ERC_WRONG_HEADER_SIZE; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Offset_To_First_Image", &iVal) && iVal > 0 ) pHeader->Offset_To_First_Image = iVal;
	else{bRes=-1; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Width", &iVal) && iVal > 0 ) pHeader->Width = iVal;
	else{bRes=EVI_ERC_WRONG_DIMENSION; goto MAIN_EXIT;}


	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Height", &iVal) && iVal > 0 ) pHeader->Height = iVal;
	else{bRes=EVI_ERC_WRONG_DIMENSION; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Nr_of_images", &iVal) && iVal > 0 ) pHeader->Nr_of_images = iVal;
	else{bRes=EVI_ERC_WRONG_DIMENSION; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Gap_between_iamges_in_bytes", &iVal) && iVal > 0 ) pHeader->Gap_between_images_in_bytes = iVal;
	else if ( ExtractIntFromString_2(strTextHeader, "Gap_between_images_in_bytes", &iVal) && iVal > 0 ) pHeader->Gap_between_images_in_bytes = iVal;
	else {bRes=EVI_ERC_WRONG_HEADER_SIZE; goto MAIN_EXIT;}


	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Sequence_Header_Bytes", &iVal) && iVal > 0 ) pHeader->Sequence_Header_Bytes = iVal;
	else{bRes=EVI_ERC_WRONG_HEADER_SIZE; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Frame_Header_Bytes", &iVal) && iVal > 0 ) pHeader->Frame_Header_Bytes = iVal;
	else{bRes=EVI_ERC_WRONG_HEADER_SIZE; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Frame_Bytes", &iVal) && iVal > 0 ) pHeader->Frame_Bytes = iVal;
	else{bRes=EVI_ERC_WRONG_DIMENSION; goto MAIN_EXIT;}


	if ( ! ExtractStringFromString(strTextHeader, "Energy_type", pHeader->Energy_type))  {bRes=EVI_ERC_WRONG_ENERGY_TYPE; goto MAIN_EXIT;}
	if ( ! ExtractStringFromString(strTextHeader, "Energy_Mode", pHeader->Energy_Mode))  {bRes=EVI_ERC_WRONG_ENERGY_MODE; goto MAIN_EXIT;}
	if ( ! ExtractStringFromString(strTextHeader, "Image_Type", pHeader->Image_Type))  {bRes=EVI_ERC_WRONG_IMAGE_TYPE; goto MAIN_EXIT;}

	float fVal = -1.0f;
	if ( ExtractFloatFromString(strTextHeader, "Frame_Rate_HZ", &fVal) && fVal > 0.0f ) pHeader->Frame_Rate_HZ = fVal;
	else if ( ExtractFloatFromString(strTextHeader, "Frame_Rate_Hz", &fVal) && fVal > 0.0f ) pHeader->Frame_Rate_HZ = fVal;
	else{bRes=EVI_ERC_WRONG_FRAME_RATE; goto MAIN_EXIT;}


	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Bits_per_Pixel", &iVal) && iVal > 0 ) pHeader->Bits_per_Pixel = iVal;
	else
	{
		//older EVI did not have this paramete
		if ( strcmp(pHeader->Image_Type,"16-bit Unsigned")== 0 )
			pHeader->Bits_per_Pixel = 16;
		else if ( strcmp(pHeader->Image_Type,"32-bit Unsigned")== 0 )
			pHeader->Bits_per_Pixel = 32;
		else if ( strcmp(pHeader->Image_Type,"Single")== 0 )
			pHeader->Bits_per_Pixel = 32;
		else{bRes=EVI_ERC_WRONG_PIXEL_TYPE; goto MAIN_EXIT;}

	}
	//else{bRes=EVI_ERC_WRONG_PIXEL_TYPE; goto MAIN_EXIT;}


	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Binning", &iVal) && iVal > 0 ) pHeader->Binning = iVal;
	else{bRes=-1; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Acquisition_Time_us", &iVal) && iVal > 0 ) pHeader->Acquisition_Time_us = iVal;
	else{bRes=-1; goto MAIN_EXIT;}

//Tds True
	char strTemp[100];
	if ( ! ExtractStringFromString(strTextHeader, "Tds", pHeader->Tds))  {bRes=-1; goto MAIN_EXIT;}
	iVal = -100000;
	if ( ExtractIntFromString_2(strTextHeader, "Tds_Step", &iVal) && iVal > -100000 ) pHeader->Tds_Step = iVal;


	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Scan_Frame_Count", &iVal) && iVal > 0 ) pHeader->Scan_Frame_Count = iVal;
	else{bRes=-1; goto MAIN_EXIT;}

	if ( strcmp(pHeader->Tds, "True")==0 && pHeader->Tds_Step!=0 )
		pHeader->Total_Height = pHeader->Scan_Frame_Count * pHeader->Height;
	else
		pHeader->Total_Height = pHeader->Height;

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "HV_T1", &iVal) && iVal > 0 ) pHeader->HV_T1 = iVal;
	else{bRes=-1; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "HV_T2", &iVal) && iVal > 0 ) pHeader->HV_T2 = iVal;
	else{bRes=-1; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "HV_TC", &iVal) && iVal > 0 ) pHeader->HV_TC = iVal;
	else{bRes=-1; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "TUBE_VOLTAGE_KV", &iVal) && iVal > 0 ) pHeader->TUBE_VOLTAGE_KV = iVal;
	else{bRes=-1; goto MAIN_EXIT;}

	fVal = -1.0f;
	if ( ExtractFloatFromString(strTextHeader, "TUBE_CURRENT_MA", &fVal) && fVal > 0.0f ) pHeader->TUBE_CURRENT_MA = fVal;
	else{bRes=-1; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Number_of_boards", &iVal) && iVal > 0 ) pHeader->Number_of_boards = iVal;
	else{bRes=EVI_ERC_WRONG_NUM_BOARDS; goto MAIN_EXIT;}


	fVal = -1.0f;
	if ( ExtractFloatFromString(strTextHeader, "FILTER_THICKNESS_MM", &fVal) && fVal >= 0.0f ) pHeader->FILTER_THICKNESS_MM = fVal;
	else{bRes=-1; goto MAIN_EXIT;}

	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "Low_Thresholds_KV", &iVal) && iVal >= 0 ) pHeader->Low_Thresholds_KV = iVal;
	iVal = -1;
	if ( ExtractIntFromString_2(strTextHeader, "High_Thresholds_KV", &iVal) && iVal >= 0 ) pHeader->High_Thresholds_KV = iVal;
	
	//char strTemp[100];
	if ( ! ExtractStringFromString(strTextHeader, "Device_Type", pHeader->Device_Type))  {bRes=-1; goto MAIN_EXIT;}

	if ( ! ExtractStringFromString(strTextHeader, "CCR_Type", pHeader->CCR_Type))  
	{
		//old detectors did not have this parameter
	}
	if ( ! ExtractStringFromString(strTextHeader, "Device_Serial_Number", pHeader->Device_Serial_Number))  {bRes=-1; goto MAIN_EXIT;}

	if ( strcmp(pHeader->Tds,"True")==0 || strcmp(pHeader->Tds,"TRUE")==0 )
		pHeader->Total_Height = pHeader->Height*pHeader->Nr_of_images;
	else
		pHeader->Total_Height = pHeader->Height; //invidual frames 

	ExtractStringFromString(strTextHeader, "Guid", pHeader->Guid);
	ExtractStringFromString(strTextHeader, "Start_Time_Stamp", pHeader->Start_Time_Stamp);
	ExtractStringFromString(strTextHeader, "Stop_Time_Stamp", pHeader->Stop_Time_Stamp);



MAIN_EXIT:
	fclose(fin);
	return bRes;
}

//--------------------------------------------------------------
unsigned char* ReadImage_EVI(const char *filename,  bool bIncludeFrameHeader)
{
	FILE *fin = NULL;	
	unsigned char *pData = NULL;

	try
	{
		EVI_Header evi;
		if ( ReadImageHeader_EVI(filename, &evi) != 1 ) return NULL;

		int bits = 2;
		if ( strcmp(evi.Image_Type,"Single")==0 ) bits = 4;
		else if ( strcmp(evi.Image_Type,"32-bit Unsigned")==0 ) bits = 4;
		long iFrameSize = evi.Width*evi.Height;
		long iFrameByteSize = evi.Width*evi.Height * bits;
		if ( bIncludeFrameHeader )
			iFrameByteSize += evi.Gap_between_images_in_bytes;

		long iImageTotalSize = iFrameSize*evi.Nr_of_images; 
		if ( iImageTotalSize <= 0 ) return NULL;


		fopen_s(&fin, filename, "rb");
		if ( fin == NULL ) 
		{
			printf("Problem reading file %s", filename);
			return NULL;
		}



		pData = new unsigned char[iFrameByteSize*evi.Nr_of_images];
	

		fseek(fin, evi.Offset_To_First_Image-evi.Gap_between_images_in_bytes , SEEK_SET);

		if ( bIncludeFrameHeader )
		{
			for(int iZ=0; iZ < evi.Nr_of_images; iZ++)
			{
				if ( fread(&pData[iZ*iFrameByteSize], 1, iFrameByteSize, fin) != iFrameByteSize ) 
				{
					delete [] pData; pData = NULL;
					return NULL;
				}						
			}
		}
		else
		{
			for(int iZ=0; iZ < evi.Nr_of_images; iZ++)
			{
				if ( evi.Gap_between_images_in_bytes )
					fseek(fin, evi.Gap_between_images_in_bytes, SEEK_CUR);
				if ( fread(&pData[iZ*iFrameByteSize], 1, iFrameByteSize, fin) != iFrameByteSize ) 
				{
					delete [] pData; pData = NULL;
					return NULL;
				}						
			}
		}

		fclose(fin);
	}
	catch(std::exception ex)
	{
		throw ex;
	}

	return pData;
}

//--------------------------------------------------------------
unsigned char* ReadSequenceHeader_EVI(const char *filename)
{
	FILE *fin = NULL;	
	unsigned char *pSequenceHeaderData = NULL;

	try
	{
		EVI_Header evi;
		if ( ReadImageHeader_EVI(filename, &evi) != 1 ) return NULL;

		if ( evi.Sequence_Header_Bytes <= 0 ) return NULL;

		fopen_s(&fin, filename, "rb");
		if ( fin == NULL ) 
		{
			printf("Problem reading file %s", filename);
			return NULL;
		}



		pSequenceHeaderData = new unsigned char[evi.Sequence_Header_Bytes];
	

		fseek(fin, evi.PlainText_Header_Bytes , SEEK_SET);

		if ( fread(pSequenceHeaderData, 1, evi.Sequence_Header_Bytes, fin) != evi.Sequence_Header_Bytes ) 
		{
			delete[] pSequenceHeaderData; pSequenceHeaderData=NULL;
		}



		fclose(fin);
	}
	catch(std::exception ex)
	{
		throw ex;
	}

	return pSequenceHeaderData;
}


//-----------------------------------------------------------------------------------------------------
int WriteImage_EVI(const char *filename, unsigned char *pSequenceHeader, unsigned char *pData, const EVI_Header *pHeader)
{
	int iRes = 1;
	FILE *fout = NULL;
	const int iMaxTextHeader = 8192;
	try
	{
		if ( pHeader->Width<=0 || pHeader->Height<=0 || pHeader->Nr_of_images<=0 )
		{
			return EVI_ERC_WRONG_DIMENSION;
		}
		int iSequenceHeader = pHeader->Sequence_Header_Bytes;
		if ( pSequenceHeader == NULL && iSequenceHeader > 0 ) 
			return EVI_ERC_BAD_SEQUENCE_HEADER;

		if ( pHeader->PlainText_Header_Bytes <=0 )
			return EVI_ERC_WRONG_HEADER_SIZE;

		if ( pHeader->Bits_per_Pixel <=0 )
			return EVI_ERC_WRONG_PIXEL_TYPE;

		fopen_s(&fout, filename, "wb");
		if ( fout == NULL )
		{
			return EVI_ERC_BAD_FILENAME;
		}

		int iFrameSizeBytes = pHeader->Width*pHeader->Height*pHeader->Bits_per_Pixel/8+pHeader->Gap_between_images_in_bytes;
		//calculate the offset
		int iOffset = EVI_TEXT_HEADER_BYTES + pHeader->Sequence_Header_Bytes + pHeader->Gap_between_images_in_bytes;


		std::ostringstream  strT;
		strT << "PlainText_Header_Bytes " << EVI_TEXT_HEADER_BYTES  << "\n";
		strT << "Image_Type " << pHeader->Image_Type << "\n";
		strT << "Width " << pHeader->Width << "\n";
		strT << "Height " << pHeader->Height << "\n";
		strT << "Offset_To_First_Image " << iOffset  << "\n";
		strT << "Nr_of_images " << pHeader->Nr_of_images  << "\n";
		strT << "Scan_Frame_Count " << pHeader->Nr_of_images  << "\n";
		strT << "Bits_per_Pixel " << pHeader->Bits_per_Pixel  << "\n";
		strT << "Number_of_boards " << pHeader->Number_of_boards  << "\n";
		strT << "Guid " << pHeader->Guid  << "\n";
		strT << "Frame_Bytes " << iFrameSizeBytes   << "\n";
		strT << "Sequence_Header_Bytes " << pHeader->Sequence_Header_Bytes  << "\n";
		strT << "Frame_Rate_HZ " << pHeader->Frame_Rate_HZ   << "\n";
		strT << "Acquisition_Time_us " << pHeader->Acquisition_Time_us   << "\n";
		strT << "Binning " << pHeader->Binning   << "\n";
		strT << "CCR_Type " << pHeader->CCR_Type   << "\n";
		strT << "Device_Serial_Number " << pHeader->Device_Serial_Number   << "\n";
		strT << "Device_Type " << pHeader->Device_Type   << "\n";
		strT << "Energy_Mode " << pHeader->Energy_Mode   << "\n";
		strT << "Energy_type " << pHeader->Energy_type   << "\n";
		strT << "Gap_between_iamges_in_bytes " << pHeader->Gap_between_images_in_bytes   << "\n"; //keep for legacy
		strT << "Gap_between_images_in_bytes " << pHeader->Gap_between_images_in_bytes   << "\n"; 
		strT << "Frame_Header_Bytes " << pHeader->Frame_Header_Bytes   << "\n";
		strT << "HV_T1 " << pHeader->HV_T1   << "\n";
		strT << "HV_T2 " << pHeader->HV_T2   << "\n";
		strT << "HV_TC " << pHeader->HV_TC   << "\n";
		strT << "Number_of_boards " << pHeader->Number_of_boards   << "\n";  
		strT << "Tds " << pHeader->Tds   << "\n";  
		strT << "Tds_Step " << pHeader->Tds_Step   << "\n";  
		strT << "TUBE_CURRENT_MA " << pHeader->TUBE_CURRENT_MA   << "\n";  
		strT << "TUBE_VOLTAGE_KV " << pHeader->TUBE_VOLTAGE_KV   << "\n";  
		strT << "FILTER_THICKNESS_MM " << pHeader->FILTER_THICKNESS_MM   << "\n";
		strT << "FILTER_MATERIAL " << pHeader->FILTER_MATERIAL  << "\n";
		strT << "CCR_Type " << pHeader->CCR_Type   << "\n";  
		strT << "Device_Serial_Number " << pHeader->Device_Serial_Number   << "\n";  
		strT << "Start_Time_Stamp " << pHeader->Start_Time_Stamp   << "\n";  
		strT << "Stop_Time_Stamp " << pHeader->Stop_Time_Stamp   << "\n";  
		strT << "Endianness Little-endian byte order\n";
		strT << "Low_Thresholds [ 1.2 1.2 1.2 1.2 1.2 1.2 1.2 1.2 1.2 1.2 1.2 1.2 ]\n";
		strT << "High_Thresholds [ 3.2 3.2 3.2 3.2 1.2 1.2 1.2 1.2 1.2 1.2 1.2 1.2 ]\n";
		strT << "Low_Thresholds_KV " <<pHeader->Low_Thresholds_KV << "\n";
		strT << "High_Thresholds_KV " <<pHeader->High_Thresholds_KV << "\n";
		strT << "COMMENT\n";




		if ( fwrite(strT.str().c_str(), 1, pHeader->PlainText_Header_Bytes, fout) != pHeader->PlainText_Header_Bytes )
		{iRes=EVI_ERC_WRONG_HEADER_SIZE; throw std::exception("Wrong header size");}
		//std::string str = strT.str();
		//if ( str.size() == 0)
		//{
		//}
		if ( pSequenceHeader != NULL )
		{
			if ( fwrite(pSequenceHeader, 1, pHeader->Sequence_Header_Bytes, fout) != pHeader->Sequence_Header_Bytes )
				{iRes=EVI_ERC_PROBLEM_WRITING_TO_FILE; throw std::exception("Problem writing sequence header to filde");}
		}

		for(int iZ=0; iZ < pHeader->Nr_of_images; iZ++)
		{
			if ( fwrite(&pData[iZ*iFrameSizeBytes], 1, iFrameSizeBytes, fout) != iFrameSizeBytes )
				{iRes=EVI_ERC_PROBLEM_WRITING_TO_FILE; throw std::exception("Problem writing data to filde");}
		}

	}
	catch(std::exception ex)
	{
		//throw ex;
		printf("Exception occured: %s\n", ex.what());
	}

	fclose(fout);
	return iRes;
}

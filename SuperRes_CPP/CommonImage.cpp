//#include "stdafx.h"
#include <string>
#include <locale>
#include "CommonImage.h"
#include "Tiff_Image.h"

//------------------------------------------------------
std::string GetFileExtension(const char* filename, bool toUpper)
{
	std::string fn = filename;
	fn = fn.substr(fn.find_last_of(".")+1);
	std::locale loc;
	if ( toUpper && !fn.empty() )
	{
		for(int i=0; i < fn.size(); i++)
			fn.at(i) = std::toupper(fn.at(i), loc);
	}
	return fn;
}

//-------------------------------------
//-------------------------------------------------------
CIMG_RC GetHeaderFromTiffFile(const char *filename, CIMG_Header &h)
{
	Tiff_header th;
	if ( GetTiffHeader(filename, th) )
	{
		h.width = th.width;
		h.height = th.highet;
		h.num_frames = th.num_frames;
		h.tc = 0; //not known
		h.pixType = th.pixType;
		h.acq_time = 0; //not knowm
		return CIMG_RC_OK;
	}
	else
		CIMG_RC_ERR_INVALID_TIFF;
}

//-------------------------------------------------------
CIMG_RC GetHeaderFromImageFile_func(const char *filename, CIMG_Header &h)
{
	std::string extension = GetFileExtension(filename, true);
	if ( extension.empty() ) return CIMG_RC_ERR_INVALID_FILE_EXTENSION;
	if ( strcmp(extension.c_str(),"TIFF")==0 || strcmp(extension.c_str(),"TIF")==0 )
	{
		return GetHeaderFromTiffFile(filename, h);
	}
	else
		return CIMG_RC_ERR_UNKNOWN_IMAGE_TYPE;
	return CIMG_RC_OK;
}

//------------------------------------------------------
CIMG_RC CIMG::Create(int iW, int iH, int iD, int pixType)
{
	CIMG_RC rc = CIMG_RC_UNKNOWN;

	long iSize = iW*iH*iD;
	switch(pixType)
	{
	case CIMG_PIXEL_TYPE_USG_16: pData = new unsigned char[iSize*2]; break;
	case CIMG_PIXEL_TYPE_USG_32: pData = new unsigned char[iSize*4]; break;
	case CIMG_PIXEL_TYPE_FLOAT: pData = new unsigned char[iSize*4]; break;
	default: rc = CIMG_RC::CIMG_RC_ERR_UNKNOWN_IMAGE_TYPE;
	}
	header.width = iW;
	header.height = iH;
	header.num_frames = iD;
	header.pixType = pixType;	
	return CIMG_RC_OK;
}

//-----------------------------------
CIMG_RC CIMG::ReadDataFromTiffFile(const char *filename )
{

	CIMG_RC rc = ReadHeaderFromImageFile(filename);
	if ( rc != CIMG_RC_OK ) 
		return rc;
		
	pData = ReadTiffImage(filename);
	if ( pData == NULL )
		return CIMG_RC_ERR_INVALID_TIFF;
	return CIMG_RC_OK;
}

//---------------------
CIMG::CIMG()
{
	pData = NULL;
	memset(&header, 0, sizeof(CIMG_Header));
}

//-----------------------
CIMG::~CIMG()
{
	delete[] pData;
}

//--------------------------
CIMG_RC CIMG::ReadHeaderFromImageFile(const char *filename)
{
	return GetHeaderFromImageFile_func(filename, header);
}

//--------------------------
CIMG_RC CIMG::ReadDataFromImageFile(const char *filename)
{
	std::string extension = GetFileExtension(filename, true);
	if ( extension.empty() ) return CIMG_RC_ERR_INVALID_FILE_EXTENSION;
	if ( strcmp(extension.c_str(),"TIFF")==0 || strcmp(extension.c_str(),"TIF")==0 )
	{
		return ReadDataFromTiffFile(filename);

	}
	else
		return CIMG_RC_ERR_UNKNOWN_IMAGE_TYPE;	 
	if ( pData == NULL )
		return CIMG_RC_ERR_INVALID_TIFF;

	return CIMG_RC_OK;
}

//----------------------------
CIMG_RC CIMG::WriteDataToTifFile(const char *filename)
{
	TIFF *pTiffImage = TIFFOpen(filename, "w");
	if ( !pTiffImage )
		return CIMG_RC_ERR_CANNOT_WRITE_TO_FILE;

	int iWidth = header.width;
	int iHeight = header.height;
	int iDepth = header.num_frames;
	uint32 w=iWidth, h=iHeight;

	long iFrameSize = iHeight * iWidth;

	int bits = 16;
	int iRowByteSize = 0;
	switch (header.pixType )
	{
	case CIMG_PIXEL_TYPE_FLOAT : bits = 32; iRowByteSize=iWidth*4; break;
	case CIMG_PIXEL_TYPE_USG_32 : bits = 32; iRowByteSize=iWidth*4; break;
	case CIMG_PIXEL_TYPE_USG_16 : bits = 16; iRowByteSize=iWidth*2; break;
	}

	if ( iWidth > 0 && iHeight > 0 && iDepth > 0 )
	{
		//uint16 *raster16 = 0;
		//float *raster32 = 0;
		unsigned char *raster = 0;

		for(int iZ=0; iZ < iDepth; iZ++)
		{
			unsigned char *pFrameData = (unsigned char*)pData+(iZ*iFrameSize);
			TIFFSetField(pTiffImage,TIFFTAG_IMAGEWIDTH, w); 
			TIFFSetField(pTiffImage,TIFFTAG_IMAGELENGTH, h);

			int samples=1;
			//int bits = 32; //16
			TIFFSetField(pTiffImage, TIFFTAG_BITSPERSAMPLE, bits);
			TIFFSetField(pTiffImage, TIFFTAG_SAMPLESPERPIXEL, samples); 
			TIFFSetField(pTiffImage, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
			TIFFSetField(pTiffImage, TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
			TIFFSetField(pTiffImage, TIFFTAG_ROWSPERSTRIP, w);
			TIFFSetField(pTiffImage, TIFFTAG_PAGENUMBER, iZ, iDepth);
			if ( header.pixType == CIMG_PIXEL_TYPE_FLOAT ) //when writing data as float
			{
				TIFFSetField(pTiffImage, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
			}

			raster = (unsigned char*)_TIFFmalloc(iRowByteSize);
			//T* pInData = InImage.m_ppData[iZ];
			//if ( bits == 16 )
			//	raster16 = (uint16*) _TIFFmalloc(w * sizeof (uint16));
			//else if ( bits == 32 )
			//	raster32 = (float*) _TIFFmalloc(w * sizeof (float));
			
			for (int iY=0; iY < h; iY++)
			{
				memcpy(raster, &pFrameData[iY*iRowByteSize], iRowByteSize);
				int iResult = TIFFWriteScanline(pTiffImage, raster, iY, 0);
			}
			_TIFFfree(raster);


			TIFFWriteDirectory(pTiffImage);

		}
	}
	TIFFFlushData(pTiffImage);
	 TIFFClose(pTiffImage);
	return CIMG_RC_OK;
}

//---------------------
void *CIMG::GetDataPointer()
{
	return pData;
}
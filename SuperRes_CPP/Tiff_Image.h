#pragma once
//FILE _iob[] = { *stdin, *stdout, *stderr };
//
//extern "C" FILE * __cdecl __iob_func(void)
//{
//	return _iob;
//}



#define TIFF_PIXEL_TYPE_UNKNOWN 0
#define TIFF_PIXEL_TYPE_USG_8		1
#define TIFF_PIXEL_TYPE_USG_16	2
#define TIFF_PIXEL_TYPE_SGN_16	3
#define TIFF_PIXEL_TYPE_USG_32	4
#define TIFF_PIXEL_TYPE_SGN_32	5
#define TIFF_PIXEL_TYPE_FLOAT	6
#define TIFF_PIXEL_TYPE_DOUBLE	7

#include "tiff.h"
#include "tiffio.h"

struct Tiff_header
{
	int width;
	int highet;
	int num_frames;
	int pixType; //
};

//--------------------------------------------------------
bool GetTiffHeader(const char *filename, Tiff_header &header)
{
	TIFF *pTiffImage = TIFFOpen(filename, "rh");
	if ( !pTiffImage )
		return false;
	TIFFClose(pTiffImage);

	int iWidth = 0;
	int iHeight = 0;
	int iBits = 0;;
	int IsFloat = 0;
	int iNumFrames=0;


	pTiffImage = TIFFOpen(filename, "r");

	TIFFGetField(pTiffImage,TIFFTAG_IMAGEWIDTH, &iWidth);
	TIFFGetField(pTiffImage,TIFFTAG_IMAGELENGTH, &iHeight);

	TIFFGetField(pTiffImage, TIFFTAG_BITSPERSAMPLE, &iBits);
	TIFFGetField(pTiffImage, TIFFTAG_SAMPLEFORMAT, &IsFloat);


	do
	{
		iNumFrames++;
	} while (TIFFReadDirectory(pTiffImage));
	TIFFClose(pTiffImage);

	header.width = iWidth;
	header.highet = iHeight;
	header.num_frames = iNumFrames;
	switch(iBits)
	{
	case 8: header.pixType = TIFF_PIXEL_TYPE_USG_8; break;
	case 16: header.pixType = TIFF_PIXEL_TYPE_USG_16; break;
	case 32:
		if ( IsFloat == 0 )
			header.pixType = TIFF_PIXEL_TYPE_USG_32;
		else
			header.pixType = TIFF_PIXEL_TYPE_FLOAT;
		break;
	}
	

	return true;
}

//--------------------------------------------------------------
unsigned char *ReadTiffImage(const char *filename)
{
	unsigned char *pImage = 0;
 

	//try
	//{
	TIFF *pTiffImageHeader = TIFFOpen(filename, "r"); //reading the header to see if it is a valid tif file
	if ( !pTiffImageHeader )
		return 0;

		int dircount = 0;
		do {
			dircount++;
		} while (TIFFReadDirectory(pTiffImageHeader));


	TIFFClose(pTiffImageHeader);
	TIFF *pTiffImage = TIFFOpen(filename, "r");
	if ( !pTiffImage )
		return 0;


	//http://blog.bee-eee.com/2008/03/20/c-reading-tiff-using-libtiff/

	int iWidth = 0;
	int iHeight = 0;
	uint32 w=0, h=0; 
	int IsFloat = 0;

	TIFFGetField(pTiffImage, TIFFTAG_SAMPLEFORMAT, &IsFloat);

	TIFFGetField(pTiffImage,TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(pTiffImage,TIFFTAG_IMAGELENGTH, &h);
 
	int samples=1;
	int bits = 32; //16
	uint16 bitspersample;
	TIFFGetField(pTiffImage, TIFFTAG_BITSPERSAMPLE, &bits);
	TIFFGetField(pTiffImage, TIFFTAG_SAMPLESPERPIXEL, &samples); 
	TIFFGetField(pTiffImage, TIFFTAG_BITSPERSAMPLE, &bitspersample);

	//temp reading custom field
	unsigned short *iOp;
	float *fOp;
	unsigned short iCount = 0;
	//try
	//{
	/*int iRes = TIFFGetField(pTiffImage, TIFFTAG_XC_CORRECTION_FLAGS,&iCount, &fOp); */

	iWidth = (int)w;
	iHeight = (int)h;

	long iFrameSize = iHeight * iWidth;
 
	int iZ = 0;
	if ( iWidth > 0 && iHeight > 0 && dircount > 0 )
	{
		int iSize = iWidth*iHeight*dircount*(bits/8);
		pImage = new unsigned char [iSize];


		///////
		if ( bits == 16 )
		{
			uint16 *raster16 = 0;
			do
			{

				raster16 = (uint16*) _TIFFmalloc(w * sizeof (uint16));

				if ( raster16 )
				{
					for (int iY=0; iY < h; iY++)
					{
						int iResult = TIFFReadScanline(pTiffImage, raster16, iY, 0);
						if (iResult)
						{
							uint16* pOutData =  &((uint16*)pImage)[iZ*iFrameSize + iY*iWidth];

							for (long i=0; i < iWidth; i++)
							{
								uint16 v1 = raster16[i];
								uint16 v2 = pOutData[i];
								pOutData[i] = raster16[i];
							}						
						}
					} 
					if ( raster16 ) _TIFFfree(raster16);
				}			
			iZ++;
			} while (TIFFReadDirectory(pTiffImage));
		}
		else if ( bits == 32 && IsFloat==3 )
		{
			float *rasterFloat = 0;
			do
			{
				rasterFloat = (float*) _TIFFmalloc(w * sizeof (float));

				if ( rasterFloat )
				{
					for (int iY=0; iY < h; iY++)
					{
						int iResult = TIFFReadScanline(pTiffImage, rasterFloat, iY, 0);
						if (iResult)
						{
							float* pOutDataF =  &((float*)pImage)[iZ*iFrameSize + iY*iWidth];

							for (long i=0; i < iWidth; i++)
							{
								pOutDataF[i] = rasterFloat[i];
							}						
						}
					} 
					if ( rasterFloat ) _TIFFfree(rasterFloat);
				}			
			iZ++;
			} while (TIFFReadDirectory(pTiffImage));
		}
		else
		{
			uint32 *raster32 = 0;
			do
			{
				raster32 = (uint32*) _TIFFmalloc(w * sizeof (uint32));

				if ( raster32 )
				{
					for (int iY=0; iY < h; iY++)
					{
						int iResult = TIFFReadScanline(pTiffImage, raster32, iY, 0);
						if (iResult)
						{
							uint32* pOutData32 = &((uint32*)pImage)[iZ*iFrameSize + iY*iWidth];

							for (long i=0; i < iWidth; i++)
							{
								pOutData32[i] = raster32[i];
							}						
						}
					} 
					if ( raster32 ) _TIFFfree(raster32);
				}			
			iZ++;
			} while (TIFFReadDirectory(pTiffImage));
		}
		///////


	}
	
	


	

	 TIFFClose(pTiffImage);
	//}
	//catch(std::exception ex)
	//{
	//	
	//}
	return pImage;
}
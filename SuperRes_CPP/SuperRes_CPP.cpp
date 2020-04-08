// SuperRes_CPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <math.h>
#include <time.h>
#include <exception>
#include "CommonImage.h"

using namespace std;

//---------------------------------------------------------------------
void PrintHelp()
{
	std::cout << "Hint\n";
	std::cout << "exe <tif_input_filename> <tif_output_filename> options\n";
	std::cout << "-a\ttilt angle\n";
	std::cout << "-s\tstep\n";
	std::cout << "-f or -r\tforward or backward regarding the input image acquition\n";
	std::cout << "-t <algo type>\tcpu or cpu_lut using look up table\n";
}

//----------------------------------------------------------------------------
void FindDimensionAfterRotation(int iInWidth, int iInHeight, double theta, double fMag, int &iOutWidth, int &iOutHeight)
{
	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	double rads = theta * 3.1415926 / 180.0; // fixed constant PI
	double cs = cos(-rads); // precalculate these values
	double ss = sin(-rads);
	int x1 = (int)(iInHeight * ss);
	int y1 = (int)(iInHeight * cs);
	int x2 = (int)(iInWidth * cs + iInHeight * ss);
	int y2 = (int)(iInHeight * cs - iInWidth * ss);
	int x3 = (int)(iInWidth * cs);
	int y3 = (int)(-iInWidth * ss);

	int minx = min(0, min(x1, min(x2, x3)));
	int miny = min(0, min(y1, min(y2, y3)));
	int maxx = max(0, max(x1, max(x2, x3)));
	int maxy = max(0, max(y1, max(y2, y3)));

	iOutWidth = maxx - minx;
	iOutHeight = maxy - miny;

	iOutWidth *= fMag;
	iOutHeight *= fMag;

}

//------------------------------------------------------------------
template<typename T>
double getInterpolatedPixel_TF(double x, double y, int iWidth, int iHeight, T* pixels)
{
	int xbase = (int)x;
	int ybase = (int)y;
	xbase = min(xbase, iWidth - 1);
	ybase = min(ybase, iHeight - 1);
	//if (xbase  >= iWidth  ybase >= iHeight )
	//	return 1;
	double xFraction = x - xbase;
	double yFraction = y - ybase;
	int offset = ybase * iWidth + xbase;
	double lowerLeft = pixels[offset];
	double lowerRight = xbase == iWidth - 1 ? pixels[offset] : pixels[offset + 1];
	double upperRight = (xbase == iWidth - 1 || ybase == iHeight - 1) ? pixels[offset] : pixels[offset + iWidth + 1];
	double upperLeft = ybase == iHeight - 1 ? pixels[offset] : pixels[offset + iWidth];
	double upperAverage = upperLeft;
	if (xFraction != 0.0)
		upperAverage += xFraction * (upperRight - upperLeft);
	double lowerAverage = lowerLeft;
	if (xFraction != 0.0)
		lowerAverage += xFraction * (lowerRight - lowerLeft);
	if (yFraction == 0.0)
		return lowerAverage;
	else
		return lowerAverage + yFraction * (upperAverage - lowerAverage);
}

//--------------------------------------------------------------------
template <typename T>
void RotateImage_cpu(const T *pData, int iW, int iH, T *pOutData, int iOutWidth, int iOutHeight, double theta, double fMagnification, int interpolation = 0)
{
	memset(pOutData, 0, sizeof(T) * iOutWidth*iOutHeight);
	double fMag = 1.0 / fMagnification;
	float rads = (theta) * 3.1415926 / 180.0;
	float cs = cos(rads); // precalculate these values
	float ss = sin(rads);
	float xcenterOut = (float)(iOutWidth) / 2.0;   // use float here!
	float ycenterOut = (float)(iOutHeight) / 2.0;
	float xcenterIn = (float)iW / 2.0f;
	float ycenterIn = (float)iH / 2.0f;
	for (int row = 0; row < iOutHeight; row++)
	{
		for (int col = 0; col < iOutWidth; col++)
		{
			float u = (float)col - xcenterOut;
			float v = (float)row - ycenterOut;
			float tu = u * cs - v * ss;
			float tv = v * cs + u * ss;

			tu *= fMag;
			tv *= fMag;

			tu += xcenterIn;
			tv += ycenterIn;
			//tu += (iOutWidth - iW) / 2;
			//tu += (iOutHeight - iH) / 2;

			if (tu >= 0 && tu < iW && tv >= 0 && tv < iH)
			{
				if (interpolation == 0)
					pOutData[row*iOutWidth + col] = pData[(int)tv*iW + (int)tu];
				else
					pOutData[row*iOutWidth + col] = getInterpolatedPixel_TF(tu, tv, iW, iH, pData);
			}
		}
	}


}

//--------------------------------------------------------------------
void RotateImage_GetLUT_cpu(int iW, int iH,  int *pLut, int iOutWidth, int iOutHeight, double theta, double fMagnification)
{
	for(long i=0; i < iOutWidth * iOutHeight; i++) pLut[i] = -1;
	double fMag = 1.0 / fMagnification;
	long iInFrameSize = iW*iH;
	float rads = (theta) * 3.1415926 / 180.0;
	float cs = cos(rads); // precalculate these values
	float ss = sin(rads);
	float xcenterOut = (float)(iOutWidth) / 2.0;   // use float here!
	float ycenterOut = (float)(iOutHeight) / 2.0;
	float xcenterIn = (float)iW / 2.0f;
	float ycenterIn = (float)iH / 2.0f;
	for (int row = 0; row < iOutHeight; row++)
	{
		for (int col = 0; col < iOutWidth; col++)
		{
			float u = (float)col - xcenterOut;
			float v = (float)row - ycenterOut;
			float tu = u * cs - v * ss;
			float tv = v * cs + u * ss;

			tu *= fMag;
			tv *= fMag;
			tu += xcenterIn;
			tv += ycenterIn;
			//tu += (iOutWidth - iW) / 2;
			//tu += (iOutHeight - iH) / 2;

			if (tu >= 0 && tu < iW && tv >= 0 && tv < iH)
			{
					//pOutData[row*iOutWidth + col] = getInterpolatedPixel_TF(tu, tv, iW, iH,  pData);
				long offset = (int)tv*iW + (int)tu;
			    // pLut[offset] = row*iOutWidth + col;
			     pLut[row*iOutWidth + col] = offset;
			}
		}
	}
}

//--------------------------------------------------------------------
template <typename T>
void RotateImage_UsingLUT_cpu(const T *pData, int iW, int iH, T *pOutData, int iOutWidth, int iOutHeight, double theta,  int *pLut)
{
	memset(pOutData, 0, sizeof(T) * iOutWidth*iOutHeight);
	for(long i=0; i < iOutWidth*iOutHeight; i++)
	{
		if ( pLut[i] >= 0 )
			pOutData[i] = pData[pLut[i]];
	}
}

//------------------------------------------------------------------------
template <typename T> 
T* RotaeAdd_cpu_lut(const T*pInData, int iW, int iH, int iNumFrames,  int &iOutWidth, int &iOutHeight, double theta, double fStep, double fMag, bool bReversed)
{
	T *pOutData = NULL;
	T * pTempData = NULL;
	int iRotWidth = iW;
	int iRotHeight = iH;
	double iShiftRow = 0;
	double step = fStep * fMag;

	FindDimensionAfterRotation(iW, iH, theta, fMag, iRotWidth, iRotHeight);
	iOutWidth = iRotWidth;
	iOutHeight = iRotHeight + iNumFrames* step + 2;	
	long iFrameSize = iW * iH;
	long iRotFrameSize = iRotWidth * iRotHeight;
	long iOutFrameSize = iOutWidth * iOutHeight;

	pOutData = new T[iOutFrameSize];
	pTempData = new T[iRotFrameSize];

	 int *pLut = new  int[iRotFrameSize];
	RotateImage_GetLUT_cpu(iW, iH, pLut, iRotWidth, iRotHeight, theta, fMag);

	int iCurIndex = 0;
	int iPrevIndex = -1;

	for (int iZ = 0; iZ < iNumFrames; iZ++)
	{
		iCurIndex = (int)iShiftRow*iRotWidth;
		if (iPrevIndex != iCurIndex)
		{
			memset(pTempData, 0, iRotFrameSize * sizeof(T));

			int iZIndex = bReversed ? iZ : iNumFrames - 1 - iZ;

			RotateImage_UsingLUT_cpu<T>((const T*)&pInData[iZIndex*iFrameSize], iW, iH, pTempData, iRotWidth, iRotHeight, theta, pLut);

			T *pOutDataRef = &pOutData[(int)iCurIndex];
			for (int p = 0; p < iRotFrameSize; p++)
			{
				pOutDataRef[p] += pTempData[p];
			}
		}
		iShiftRow += step;
		if (iShiftRow >= iOutHeight) break;
		iPrevIndex = iCurIndex;
	}

	delete[] pLut;
	delete[] pTempData;
	return pOutData;
}

//------------------------------------------------------------------------
template <typename T> 
T* RotaeAdd_cpu(const T*pInData, int iW, int iH, int iNumFrames,  int &iOutWidth, int &iOutHeight, double theta, double fStep, double fMag, bool bReversed)
{
	T *pOutData = NULL;
	T * pTempData = NULL;
	int iRotWidth = iW;
	int iRotHeight = iH;
	double iShiftRow = 0;
	double step = fStep * fMag;

	FindDimensionAfterRotation(iW, iH, theta, fMag, iRotWidth, iRotHeight);
	iOutWidth = iRotWidth;
	iOutHeight = iRotHeight + iNumFrames* step + 2;	
	long iFrameSize = iW * iH;
	long iRotFrameSize = iRotWidth * iRotHeight;
	long iOutFrameSize = iOutWidth * iOutHeight;

	pOutData = new T[iOutFrameSize];
	pTempData = new T[iRotFrameSize];

	int iCurIndex = 0;
	int iPrevIndex = -1;

	for (int iZ = 0; iZ < iNumFrames; iZ++)
	{
		iCurIndex = (int)iShiftRow*iRotWidth;
		if (iPrevIndex != iCurIndex)
		{
			memset(pTempData, 0, iRotFrameSize * sizeof(T));

			int iZIndex = bReversed ? iZ : iNumFrames - 1 - iZ;

			RotateImage_cpu<T>((const T*)&pInData[iZIndex*iFrameSize], iW, iH, pTempData, iRotWidth, iRotHeight, theta, fMag, 0);

			T *pOutDataRef = &pOutData[(int)iCurIndex];
			for (int p = 0; p < iRotFrameSize; p++)
			{
				pOutDataRef[p] += pTempData[p];
			}
		}
		iShiftRow += step;
		if (iShiftRow >= iOutHeight) break;
		iPrevIndex = iCurIndex;
	}
	delete [] pTempData;
	return pOutData;
}


//----------------------------------------------------------------------
CIMG *RunRotateAdd_in_cpu(CIMG *cimg, double theta, double step, double fMag, bool bReversed)
{
	CIMG *outCIMG = NULL;
	CIMG_Header header = cimg->header;
	
	int iOutWidth = 0, iOutHeight;

	switch (header.pixType)
	{
	case CIMG_PIXEL_TYPE_USG_16:
	{
		unsigned short *pOutData = RotaeAdd_cpu<unsigned short>((const unsigned short*)cimg->GetDataPointer(), header.width, header.height, header.num_frames, iOutWidth, iOutHeight, theta, step, fMag, bReversed);		
		outCIMG = new CIMG();
		outCIMG->Create(iOutWidth, iOutHeight, 1, CIMG_PIXEL_TYPE_USG_16);
		memcpy(outCIMG->GetDataPointer(), pOutData, 2 * iOutWidth * iOutHeight);
		break;
	}
	case CIMG_PIXEL_TYPE_USG_32:
	{
		unsigned int *pOutData = RotaeAdd_cpu<unsigned int>((const unsigned int*)cimg->GetDataPointer(), header.width, header.height, header.num_frames, iOutWidth, iOutHeight, theta, step, fMag, bReversed);
		outCIMG = new CIMG();
		outCIMG->Create(iOutWidth, iOutHeight, 1, CIMG_PIXEL_TYPE_USG_32);
		memcpy(outCIMG->GetDataPointer(), pOutData, 4 * iOutWidth * iOutHeight);
		break;
	}
	case CIMG_PIXEL_TYPE_FLOAT:
	{
		float *pOutData = RotaeAdd_cpu<float>((const float*)cimg->GetDataPointer(), header.width, header.height, header.num_frames, iOutWidth, iOutHeight, theta, step, fMag, bReversed);
				outCIMG = new CIMG();
		outCIMG->Create(iOutWidth, iOutHeight, 1, CIMG_PIXEL_TYPE_FLOAT);
		memcpy(outCIMG->GetDataPointer(), pOutData, 4 * iOutWidth * iOutHeight);
		break;
	}
	}
	return outCIMG;
}

//----------------------------------------------------------------------
CIMG *RunRotateAdd_in_cpu_lut(CIMG *cimg, double theta, double step, double fMag, bool bReversed)
{
	CIMG *outCIMG = NULL;
	CIMG_Header header = cimg->header;
	
	int iOutWidth = 0, iOutHeight;

	switch (header.pixType)
	{
	case CIMG_PIXEL_TYPE_USG_16:
	{
		unsigned short *pOutData = RotaeAdd_cpu_lut<unsigned short>((const unsigned short*)cimg->GetDataPointer(), header.width, header.height, header.num_frames, iOutWidth, iOutHeight, theta, step, fMag, bReversed);		
		outCIMG = new CIMG();
		outCIMG->Create(iOutWidth, iOutHeight, 1, CIMG_PIXEL_TYPE_USG_16);
		memcpy(outCIMG->GetDataPointer(), pOutData, 2 * iOutWidth * iOutHeight);
		break;
	}
	case CIMG_PIXEL_TYPE_USG_32:
	{
		unsigned int *pOutData = RotaeAdd_cpu_lut<unsigned int>((const unsigned int*)cimg->GetDataPointer(), header.width, header.height, header.num_frames, iOutWidth, iOutHeight, theta, step, fMag, bReversed);
		outCIMG = new CIMG();
		outCIMG->Create(iOutWidth, iOutHeight, 1, CIMG_PIXEL_TYPE_USG_32);
		memcpy(outCIMG->GetDataPointer(), pOutData, 4 * iOutWidth * iOutHeight);
		break;
	}
	case CIMG_PIXEL_TYPE_FLOAT:
	{
		float *pOutData = RotaeAdd_cpu_lut<float>((const float*)cimg->GetDataPointer(), header.width, header.height, header.num_frames, iOutWidth, iOutHeight, theta, step, fMag, bReversed);
				outCIMG = new CIMG();
		outCIMG->Create(iOutWidth, iOutHeight, 1, CIMG_PIXEL_TYPE_FLOAT);
		memcpy(outCIMG->GetDataPointer(), pOutData, 4 * iOutWidth * iOutHeight);
		break;
	}
	}
	return outCIMG;

}
//---------------------------------------------------------------------
int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		PrintHelp();
		exit(0);
	}
	std::string strFilename(argv[1]);
	std::string strFilenameOut(argv[2]);
	std::string strAlgoType = "undefined";
	double fTheta = 0.0;
	double fStep = 1.0;
	double fMagnification = 1.0;
	bool bReversed = false;
	for (int i = 2; i < argc; i++)
	{
		if (strcmp(argv[i], "-a") == 0)
		{
			fTheta = atof(argv[i + 1]);
			i++;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			fStep = atof(argv[i + 1]);
			i++;
		}
		else if (strcmp(argv[i], "-m") == 0)
		{
			fMagnification = atof(argv[i + 1]);
			i++;
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			bReversed = false;
		}
		else if (strcmp(argv[i], "-r") == 0)
		{
			bReversed = true;
		}
		else if (strcmp(argv[i], "-t") == 0)
		{
			strAlgoType = std::string(argv[i+1]);
			i++;
		}
	}

	std::cout << "To do!\n";
	std::cout << "Input file = " << strFilename << "\n";
	std::cout << "Theta = " << fTheta << "\n";
	std::cout << "Steps = " << fStep << "\n";
	std::cout << "Magnification = " << fMagnification << "\n";
	bReversed == true ? std::cout << "revered scanning\n" : std::cout << "forward scanning\n";
	std::cout << "Type = " << strAlgoType << "\n";

	CIMG *pOutImage = NULL;

	try

	{
		//GetTiffHeader(const char *filename, Tiff_header &header)
		CIMG_RC cm_rc;
		CIMG cimg;
		if ((cm_rc = cimg.ReadDataFromImageFile(strFilename.c_str())) != CIMG_RC_OK)
		{
			printf("Problem read readinf tiff file %s with error %d\n", strFilename.c_str(), cm_rc);
			exit(0);
		}
		CIMG_Header header = cimg.header;

		

		clock_t start = clock();

		if (strAlgoType.compare("cpu") == 0)
			pOutImage = RunRotateAdd_in_cpu(&cimg, fTheta, fStep, fMagnification, bReversed);
		else if (strAlgoType.compare("cpu_lut") == 0)
			pOutImage = RunRotateAdd_in_cpu_lut(&cimg, fTheta, fStep, fMagnification, bReversed);
		else
		{
			cout << "Algo type " << strAlgoType << " was not recognized\n";
			throw new std::exception("Algo type was not recognized");
			//goto cleanup;
		}

		float fElapsed = (float)(clock() - start) / (float)CLOCKS_PER_SEC;

		cout << strAlgoType << " took " << fElapsed << " seconds\n";


		pOutImage->WriteDataToTifFile(strFilenameOut.c_str());
	}
	catch (std::exception ex)
	{
		cout << ex.what();
	}
	//FILE *out; fopen_s(&out, "c:\\temp\\temp.raw", "wb"); fwrite(pOutImage->GetDataPointer(),2, pOutImage->header.width * pOutImage->header.height, out); fclose(out);
	//FILE *out; fopen_s(&out, "c:\\temp\\temp.raw", "wb"); fwrite(cimg.GetDataPointer(),2, cimg.header.width * cimg.header.height, out); fclose(out);
//cleanup:	
	if ( pOutImage != NULL ) delete pOutImage;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

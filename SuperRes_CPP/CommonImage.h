#pragma once

//pixel type
#define CIMG_PIXEL_TYPE_UNKNOWN 0
#define CIMG_PIXEL_TYPE_USG_8		1
#define CIMG_PIXEL_TYPE_USG_16	2
#define CIMG_PIXEL_TYPE_SGN_16	3
#define CIMG_PIXEL_TYPE_USG_32	4
#define CIMG_PIXEL_TYPE_SGN_32	5
#define CIMG_PIXEL_TYPE_FLOAT	6
#define CIMG_PIXEL_TYPE_DOUBLE	7

//error code
enum CIMG_RC
{
	CIMG_RC_UNKNOWN = 0,
	CIMG_RC_OK = 1,
	CIMG_RC_ERR_INVALID_FILE_EXTENSION = 2,
	CIMG_RC_ERR_UNKNOWN_IMAGE_TYPE = 3,
	CIMG_RC_ERR_INVALID_TIFF = 4,
	CIMG_RC_ERR_CANNOT_WRITE_TO_FILE = 5
};

struct CIMG_Header
{
	int width;
	int height;
	int num_frames;
	int pixType; //
	int tc;
	int acq_time;
};

//--------------------------------------------------------------
//CIMG_RC GetHeaderFromImageFile(const char *filename, CIMG_Header &h);

class CIMG
{
public: 
	CIMG();
	~CIMG();

public:
	CIMG_RC Create(int iW, int iH, int iD, int pixType);
	CIMG_Header header;
	CIMG_RC ReadHeaderFromImageFile(const char *filename);
	CIMG_RC ReadDataFromImageFile(const char *filename);
	CIMG_RC ReadDataFromTiffFile(const char *filename);
	CIMG_RC WriteDataToTifFile(const char *filename);
	void* GetDataPointer();

private:
	void *pData;
};
#pragma once

#define FrameHeaderSize 64


typedef enum : unsigned short
{
	XC_PIXTYPE_UNKNOWN = 0,
    /// <summary>8 bit unsigned</summary>
    XC_USG_8BIT = 1,
    /// <summary>16 bit unsigned</summary>
    XC_USG_16BIT,
    /// <summary>16 bit signed</summary>
    XC_SGN_16BIT,
    /// <summary>32 bit unsigned</summary>
    XC_USG_32BIT,
    /// <summary>32 bit signed</summary>
    XC_SGN_32BIT,
    /// <summary>float data type</summary>
    XC_FLOAT,
    /// <summary>double data type</summary>
    XC_DOUBLE
}XcDataType;

typedef enum
{
    /// <summary>No device type</summary>
    XC_DEVICE_UNKNOWN = 0,  
    XC_DEVICE_FLITE = 1, //tilewidth=128
	XC_DEVICE_HYDRA, //tilewidth=256
	XC_DEVICE_ACTAEN, //tilewidth=128
	XC_DEVICE_XMOUSE, //tilewidth=128
	XC_DEVICE_THOR, //tilewidth=128
    XC_DEVICE_TOMOWELD //tilewidth=128

}XcDeviceType;






/// <summary>Enumration for error codes returned from the XcCalibration functions</summary>
typedef enum //Derr
{
    XCC_ER_UNDEFINED				= 0,	//Unrecognized error code
    XCC_SUCCESS						= 1,	//The operation was successful
    XCC_ER_BADCONFIG				= 2,	//Config file does not exist or is not valid
    XCC_ER_NOCALIBFOLDER			= 3,	//Calibration folder does not exist
    XCC_ER_ERRORWRITINGCALIBCOEFF	= 4,	//Calibration coefficients cannot be written
    XCC_ER_NOCALIBFILES				= 5,	//Calibration files dot exist in the calibration folder
    XCC_ER_CALIBDATAINCOMPLETE		= 6,	//Calibration data is not complete or it dose not contain certain tag
    XCC_ER_NOCALIBRATIONIMAGES		= 7,	//Images do not exist to calculate calibration coefficients
    XCC_ER_INVALIDCALIBRATIONIMAGES	= 8,	//Some or all calibration images were invalid
    XCC_ER_NOSEQHEADER				= 9,	//The sequence header is not given or invalid
    XCC_ER_DIMENSIONMISMATCH		= 10,	//Dimension mismatch when doing image operations
    XCC_ER_NODEFECTMAP				= 11,	//No defect map or problem reading the defectmap file
    XCC_ER_PROCESSSTOPPED			= 12,	//The running process was stopped by an external app
    XCC_ER_INVALIDINPUT				= 13,	//Invalid input value is given
    XCC_ER_INVALID_HANDLE			= 14,	//The calibration handle is not valid
    XCC_ER_UNKNOWNEXCEPTION			= 15,	//Exception
    XCC_ER_ABORTED					= 16,	//The process was stopepped prematurely
    XCC_ER_IPP_FAILED				= 17,	//An IPP function failed
    XCC_ER_INVALID_TILEWIDTH		= 18,	//Wrong tile width is given
    XCC_ER_INVALID_TILEHEIGHT		= 19,	//Wrong tile height is given
    XCC_ER_INVALID_CALIBRATION_PARAM= 20,	//Wrong given option for post processing
    XCC_ER_INVALID_POSTPROC_PARAM	= 21,	//Wrong given option for post processing
	XCC_ER_INVALID_ROI				= 22,	//Given ROI parameters are wrong
	XCC_ER_INVALID_FRAMERATE		= 23,	//Wrong frame rate is provided
	XCC_ER_INVALID_NUM_FRAMES		= 24,	//Wrong number of frames is provided
	XCC_ER_UNSUPPORTED_PIXELTYPE	= 25,	//Wrong given pixel type
	XCC_ER_NOENERGYTYPE				= 26,	//No energy type is given
	XCC_ER_INVALID_CORRECTIONTYPE	= 27,	//Wrong correction type
	XCC_ER_PARAM_NOTFOUND			= 28,	//At least one parameter was not found in the parameter file
	XCC_ER_INVALID_SWITCHING		= 29,	//Wrong switching parameters
	XCC_ER_INVALID_XRAY_PARAM		= 30,	//Wrong switching parameters

}XCC_ReturnCode;


//--------------------------------
typedef enum
{
	XC_CM_UNDEFINED = 0,
    /// <summary>Calibration type for varying temperature</summary>
    XC_CM_VTEMP_FRAME = 1,
    /// <summary>Calibration for constant temperature</summary>
    XC_CM_CTEMP_FRAME,
	/// <summary>Calibration for constant temperature for TDS</summary>
	XC_CM_CTEMP_TDS,
	/// <summary>Calibration for constant temperature for where each node represent a thickness containing a number of images taken at different kv or mA</summary>
	XC_CM_CTEMP_FRAME_ADJUSTABLENODES,
	/// <summary>Calibration for constant temperature for where each node represent a filter/thickness containing a number of images taken at different kv or mA</summary>
	XC_CM_CTEMP_TDS_ADJUSTABLENODES

}XcCalibrationType;



typedef enum 
{
	XcCal_ENERGY_UNDEFINED = 0,
	XcCal_ENERGY_LOW = 2,
	XcCal_ENERGY_HIGH = 3,
	XcCal_ENERGY_TOTAL = 5
}XcCal_EnergyType;

/// <summary>
/// using this enum, the user can retrive information from a connected detector 
/// </summary>
typedef enum 
{
 XCC_CALIB_PARAM_UNDEFINED = 0,
 XCC_CALIB_PARAM_NORMALIZATION_FACTOR_HIGH_ENERGY = 1,  //float: the total width of the detector
 XCC_CALIB_PARAM_NORMALIZATION_FACTOR_TOTAL_ENERGY,     //float: the height of the detector
}XcCalib_Parameters;


/// <summary>
/// Parameters used for saving to evi image format. 
/// </summary>
struct EVISettings {
	float fkVp;				//The tube voltage used for the acquisition
	float fmA;				//The tube current used for the acquisition
	float iFrameRate_HZ;      //the frame rate used
	float fAcquisitionTime;	//The acq time 	
	XcCal_EnergyType eType;  //is it Total or High energy type
	int iNumBoards;         //always set to 1 for Hydra, for Flite the actual number of boards

	//void* sequence_header;	//If this is not nullptr then some parameters are extracted from it such as the acquisition time
};


////
struct XcPostProcessingParam
{
	XcDeviceType deviceType;		//Not used. The correction type will be used instead which is acquired internally when calibration parameter is loaded	
	bool bApplyGeometricCorrection;	//Apply/don't apply the geometric correction	
	bool bApplyTileEdgeCorrection;	//Apply/don't apply edge enhancement
	XcCal_EnergyType eType;			//is it Total or High energy type
	unsigned int iTC;				//TC value of the current image
};

/// <summary>
/// Parameters used for image acquisition. 
/// </summary>
struct AcquisitionSettings {
	float fkVp;						//The tube voltage used for the acquisition
	float fmA;						//The tube current used for the acquisition
	float iFrameRate_HZ;			//the frame rate used
	unsigned int iAcquisitionTime;	//The acq time; if the sequence_header is provided (not nullptr) then this value will be used 	
	unsigned int iTC;				//TC value of the current image
	XcCal_EnergyType eType;			//energy type that should be provided by the user during the gain correction
};


#pragma once
//#include <stdio.h>
//#include <string>
//#include <vector>

#include "XcCalibrationDefs.h"



class XcCalibrationBase;

#ifdef XCALIB_EXPORTS
#define XCALIB_API __declspec(dllexport)
#else
#define XCALIB_API __declspec(dllimport)
#endif

//xCounter calibration calss library
/** 
   @brief A class XcCalibration
*/
class XCALIB_API XcCalibration
{		
public:



	/// <summary>
	/// Constructor. A config file must be provided which would contain information that might be needed for calibration calculation
	/// </summary>
	/// <param name="strDetectorConfig">The path to the config file</param>
	XcCalibration(const char *strCalibrationConfig);
	~XcCalibration();

	/// <summary>
	/// This method allows loading an updated/new config file after the class is already created; Note the config file is loaded duering the class construction
	/// </summary>
	/// <param name="strDetectorConfig">The path to the config file</param>
	XCC_ReturnCode ReloadConfigFile(const char *strCalibrationConfig);


	/// <summary>
	/// Specifies which calibration method to be used. There are several types of calibration methods that can be found in the enum
	/// Note: The calibration methods must be specified to ensure that approperiate correction are taken into account
	/// </summary>
	/// <param name="calibType">The type calibration method to be used for calibration calculation and correction</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode SetCalibrationType(XcCalibrationType calibType);



	/// <summary>
	/// sets up the folder where the data calibration exists. The calibration images must be EVI format with correct information in the header
	/// Depending on a type of calibration, the content of the EVI file will be examined to perform a valididty test and re-arrange the images 
	/// in a specific order that is required by the calibration algorithm
	/// </summary>
	/// <param name="strCalibFolderPath">The path to the folder where the calibaration files are stored</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode InitCalibration(const char *strCalibFolderPath);

 


	/// <summary>
	/// the calibration coefficient file must be loaded for correction
	/// </summary>
	/// <param name="strCalinCoeffFilename">The calibration coefficient file to loaded from</param>
	/// <returns>A error code with success = 0x1</returns>		
	XCC_ReturnCode LoadCalibrationCoeffs(const char *strCalibCoeffFilename);

	/// <summary>
	/// The update calibration coefficient file must be loaded for correction and when calculating for update
	/// </summary>
	/// <param name="strCalibCoeffFilename">The update calibration coefficient file to loaded from</param>
	/// <returns>A error code with success = 0x1</returns>		
	/// <comment>The base calibration must first exists and loaded then the update calibration can be loaded to adjust the correction</comment>		
	XCC_ReturnCode LoadCalibrationCoeffs_update(const char *strUpdateCalibCoeffFilename);


	/// <summary>
	/// After the calibration is loaded, the calibration type for the loaded parameter can be enquired 
	/// </summary>
	/// <returns>Returns the enum, indicating which calibration parameter exists in the loaded parameter </returns>	
	XcCalibrationType GetCalibrationType();

	/// <summary>
	/// It carries out the calculation of calibration coeefeicients following a successfull calibration initialization. The calculation method will be 
	/// determined using the calculation type specified in the config file.
	/// </summary>
	/// <param name="bUseROI_ForBuildingCalibrationParameters">an indication of using/not_using ROI region. When false, the whole image dimension will be used. When true a valid ROI must be provided using SetROI
	///                                                        This option is allied when buiding the calibration coefficients</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode CalculateCalibCoeffs(bool bUseROI_ForBuildingCalibrationParameters);

	/// <summary>
	/// When calibration is done, update of calibration can be done with a single image that must be taken with parameters (kv, ma and thickness, frame rate) as one of the images
	/// used for the base calibration. It is important to keep the original calibration coefficient file and save the updated calibration coeffcient in a separate file. When calling
	/// this fucntion the original calibration coefficient must be loaded
	/// </summary>
	/// <param name="strUpdateFolder">Update folder that should contain the image to be used for calibration update</param>
	/// <comment>Currently the update folder should have one EVI image</comment>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode CalculateCalibCoeffs_update(const char *strUpdateFolder); 

	/// <summary>
	/// It specifies an ROI region within which the calibration coefficients will be calculated. This is only used when the calibration coefficients are being calculated.
	/// The correction, the correction will need the complete frames but the correction will only be applied to this ROI. The corrected frame will have blank outside the ROI
	/// </summary>
	/// <param name="iROI_x">Starting column of ROI</param>
	/// <param name="iROI_y">Starting row of ROI</param>
	/// <param name="iROI_width">Width of ROI</param>
	/// <param name="iROI_height">Height column of ROI</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode SetROI(int iROI_x, int iROI_y, int iROI_width, int iROI_Height); 

	/// <summary>
	/// saves the calibration coeffcients to the given filename following a successful calculation of the coeffcients
	/// </summary>
	/// <param name="strCalinCoeffFilename">The calibration coefficient file to be written to</param>
	/// <returns>A error code with success = 0x1</returns>		
	XCC_ReturnCode SaveCalibrationCoeffs(const char *strCalibCoeffFilename);

	/// <summary>
	/// saves the update calibration coeffcients to the given filename following a successful calculation of the update coeffcients
	/// </summary>
	/// <param name="strCalinCoeffFilename">The calibration coefficient file to be written to</param>
	/// <returns>A error code with success = 0x1</returns>		
	XCC_ReturnCode SaveCalibrationCoeffs_update(const char *strCalinCoeffFilename);

	/// <summary>
	/// It performs gain correction on a sequence given the correct acquision parameters.The input and output are pointers to the individual frames with each frame containing the frame header.
	/// The input and output data are assumed to be unsigned 16 bit data type
	/// </summary>
	/// <param name="ppInData">Point to input data</param>
	/// <param name="ppCorrectedImage">Pointer to output data</param>
	/// <param name="iFrameHeaderByteSize">Input image frame header size in bytes, 0 if input data has no frame header</param>
	/// <param name="iImageDataWidth">Width of image region</param>
	/// <param name="iImageDataHeight">Height of image region</param>
	/// <param name="iNumFrames">Number of frames to be corrected</param>
	/// <param name="AcqParam">A set of acquisition parameters</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode ComplexGainCorrectionForSequence(void **ppInData, void **ppCorrectedImage, int iFrameHeaderByteSize, int iImageDataWidth, int iImageDataHeight, int  iNumFrames, const AcquisitionSettings &AcqParam);

	/// <summary>
	/// It performs gain correction on a sequence given the correct acquision parameters.The input and output are pointers to the individual frames with each fram containing the frame header.
	/// The input and output data datatype must be defined.
	/// For TDS, we normally the iNumFrames=1 where the data is constrcuted from a number frames when loaded from the EVI file format 
	/// </summary>
	/// <param name="ppInData">Point to input data</param>
	/// <param name="pixTypeInputData">Pixel type of the input data. Generally this is unsigned short (XC_USG_16BIT)</param>
	/// <param name="ppCorrectedImage">Pointer to output data</param>
	/// <param name="pixTypeInputData">Pixel type of the output data</param>
	/// <param name="iFrameHeaderByteSize">Input image frame header size in bytes, 0 if input data has no frame header</param>
	/// <param name="iImageDataWidth">Width of image region</param>
	/// <param name="iImageDataHeight">Height of image region</param>
	/// <param name="iNumFrames">Number of frames to be corrected</param>
	/// <param name="AcqParam">A set of acquisition parameters</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode ComplexGainCorrectionForSequence(void **ppInData, XcDataType pixTypeInputData, void **ppCorrectedImage, XcDataType pixTypeOutputData, int iFrameHeaderByteSize, int iImageDataWidth, int iImageDataHeight, int  iNumFrames, const AcquisitionSettings &AcqParam);

	/// <summary>
	/// It performs gain correction on a sequence given the correct acquision parameters.The input and output are pointers to the individual frames with each fram containing the frame header.
	/// The input and output are pointers to the first address of frames that are assumed to be allocated in contiguous memory locations.
	/// The input and output data are assumed to be unsigned 16 bit data type
	/// For TDS, we normally the iNumFrames=1 where the data is constrcuted from a number frames when loaded from the EVI file format 
	/// </summary>
	/// <param name="pInData">Point to input data</param>
	/// <param name="pCorrectedImage">Pointer to output data</param>
	/// <param name="iFrameHeaderByteSize">Input image frame header size in bytes, 0 if input data has no frame header</param>
	/// <param name="iImageDataWidth">Width of image region</param>
	/// <param name="iImageDataHeight">Height of image region</param>
	/// <param name="iNumFrames">Number of frames to be corrected</param>
	/// <param name="AcqParam">A set of acquisition parameters</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode ComplexGainCorrectionForSequence_Contiguous(void *pInData, void *pCorrectedImage, int iFrameHeaderByteSize, int iImageDataWidth, int iImageDataHeight, int  iNumFrames, const AcquisitionSettings &AcqParam);


	/// <summary>
	/// It performs gain correction on a sequence given the correct acquision parameters.The input and output are pointers to the individual frames with each fram containing the frame header.
	/// The input and output are pointers to the first address of frames that are assumed to be allocated in contiguous memory locations.
	/// The input and output pixels types must be provided
	/// For TDS, we normally the iNumFrames=1 where the data is constrcuted from a number frames when loaded from the EVI file format 
	/// </summary>
	/// <param name="pInData">Point to input data</param>
	/// <param name="pixTypeInput">Pixel type of the input data</param>
	/// <param name="pCorrectedImage">Pointer to output data</param>
	/// <param name="pixTypeOutput">Pixel type of the output data</param>
	/// <param name="iFrameHeaderByteSize">Input image frame header size in bytes, 0 if input data has no frame header</param>
	/// <param name="iImageDataWidth">Width of image region</param>
	/// <param name="iImageDataHeight">Height of image region</param>
	/// <param name="iNumFrames">Number of frames to be corrected</param>
	/// <param name="AcqParam">A set of acquisition parameters</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode ComplexGainCorrectionForSequence_Contiguous(void *pInData, XcDataType pixTypeInput, void *pCorrectedImage, XcDataType pixTypeOutput, int iFrameHeaderByteSize, int iImageDataWidth, int iImageDataHeight, int  iNumFrames, const AcquisitionSettings &AcqParam);



	/// <summary>
	/// After loading the calibration parameters, the gain image must be created for some type of correction using the current acquisition parameters.
	/// This function must be called if ROI correction is to be used. For the one-off corection the ComplexCorrection can be used and there is no need for this function call
	/// </summary>
	/// <param name="iGainImageWidth">The width of the gain image which should be the same as the width of calibration images</param>
	/// <param name="iGainImageHeight">The height of the gain image which should the same as the width of calibration images</param>
	/// <param name="AcqParam">A set of acquisition parameters</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode PrepareForROICorrection(int iGainImageWidth, int iGainImageHeight, const AcquisitionSettings &AcqParam);

	 /// <summary>
	 /// Returns parameter given the parameter tag 
	 /// </summary>
	 /// <param name="errorMessage">The error message</param>
	 /// <param name="iMessageSize">The number characters returned is equal to min(actual message length, iMessageSize)</param>
	 /// <returns>Nothing is returned</returns> 
	 XCC_ReturnCode GetParameter(XcCalib_Parameters param, void *value, int iSize);



	/// <summary>
	/// It performs gain correction on a given section of image. Before using this function, the CreateGainImage must be called to prepare the gain images internally
	/// Deprecated: use ComplexGainCorrection_ROI or ComplexGainCorrection_ROI_Contiguous
	/// </summary>
	/// <comment> Deprecated. The ROI post-processing is now separated from the ROU gain correction </comment>
	XCC_ReturnCode ComplexGainCorrectionROI(void *pInData, XcDataType pixTypeInput, void *pCorrectedData, XcDataType pixTypeOutput, int iImageDataWidth, int iImageDataHeight, int  iNumFrameIndex, int iROI_X, int iROI_Y, int iROI_Width, int iROI_Height, const AcquisitionSettings &AcqParam, const XcPostProcessingParam &postProcessingParams);
	

	/// <summary>
	/// It performs gain correction on a given section of image. Before using this function, the CreateGainImage must be called to prepare the gain images internally
	/// </summary>
	/// <param name="pInData">Pointer to a sum-image taken from the input that has the dimension of the given ROI; NOTE: it is not a pointer to the ROI section of the original image</param>
	/// <param name="pixTypeInput">Pixel type of the input data</param>
	/// <param name="pCorrectedImage">Pointer to a sum-image taken from the output that has the dimension of the given ROI</param>
	/// <param name="pixTypeOutput">Pixel type of the output data</param>
	/// <param name="iImageDataWidth">Width of the original image</param>
	/// <param name="iImageDataHeight">Height of the originalimage</param>
	/// <param name="iNumFrameIndex">The current frame number (for TDS it is generally 0)</param>
	/// <param name="iEnergyType">The energy type Total or High energy</param>
	/// <param name="iROI_X">Start column of the ROI</param>
	/// <param name="iROI_Y">Start row of the ROI</param>
	/// <param name="iROI_Width">the width of ROI</param>
	/// <param name="iROI_Height">the height of ROI</param>
	/// <param name="iStartFrame">Apply correction starting from this frame number; in TDS this is 0</param>
	/// <param name="iNumFrames">Number of frames to be corrected; in TDS this is 1</param>
	/// <param name="AcqParam">A set of acquisition parameters</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode ComplexGainCorrection_ROI(void **ppInData, XcDataType pixTypeInput, void **ppCorrectedData, XcDataType pixTypeOutput, int iImageWidth, int iImageHeight, int iROI_X, int iROI_Y, int iROI_Width, int iROI_Height, int iStartFrame, int iNumFrames, const AcquisitionSettings &AcqParam);
	XCC_ReturnCode ComplexGainCorrection_ROI_Contiguous(void *pInData, XcDataType pixTypeInput, void *pCorrectedData, XcDataType pixTypeOutput, int iImageWidth, int iImageHeight, int iROI_X, int iROI_Y, int iROI_Width, int iROI_Height, int iStartFrame, int iNumFrames, const AcquisitionSettings &AcqParam);


	/// <summary>
	/// Perfoms the edge correction on a single frame, each frame might have the frame header. 
	/// This function should be called after the gain correction is applied. The deviceType must be provided in order to apply correction at the correct edges
	/// The dimensions of source and destination are assumed to be the same
	/// </summary>
	/// <param name="pInData">Pointer to a single frame input data containing frame header</param>
	/// <param name="pOutData">Pointer to a single frame output data containing frame header</param>
	/// <param name="iWidth">Width of the teh image data</param>
	/// <param name="iHeight">Height of the teh image data</param>
	/// <param name="pixelType">The pixel type of input and output data</param>
	/// <param name="iFrameHeaderByteSize">The size of frame header in bytes. If there is no frame header then this value is set to 0</param>
	/// <param name="params">A valid device type must be provided and the other parameters are optional</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode PostProcessingCorrection(const void *pInData, void *pOutData, int iWidth, int iHeight, XcDataType iPixelType, int iFrameHeaderByteSize, const XcPostProcessingParam &params);



	/// <summary>
	/// Returns the dimensions of the destination image given the current pot-processing parameters. 
	/// This should be called prior PostProcessingCorrection_VarSrcDestDimensions to know the exact dimension of the destination image
	/// </summary>
	/// <param name="iSrcWidth">Width of the source image data</param>
	/// <param name="iSrcHeight">Height of the source image data</param>
	/// <param name="iDestWidth">Width of the destination image data</param>
	/// <param name="iDestHeight">Height of the destination image data</param>
	/// <param name="pixelType">The pixel type of source and destination data</param>
	/// <param name="iFrameHeaderByteSize">The size of frame header in bytes. If there is no frame header then this value is set to 0</param>
	XCC_ReturnCode GetImageDimensionsForPostProcessing(int iSrcWidth, int iSrcHeight, int &iDestWidth, int &iDestHeight, const XcPostProcessingParam &params);

	/// <summary>
	/// Perfoms the edge correction on a single frame, each frame will have the frame header. 
	/// This function should be called after the gain correction is applied. The deviceType must be provided in order to apply correction at the correct edges
	/// The dimesnion of the destination can be different depending on whether the geometric correction is applied or not.
	/// Prior to this call GetImageDimensionsForPostProcessing should be called to get the dimension of the destination given the pot processing parameters
	/// </summary>
	/// <param name="pSrcData">Pointer to a single frame input data containing frame header</param>
	/// <param name="pDestData">Pointer to a single frame output data containing frame header</param>
	/// <param name="iSrcWidth">Width of the source image data</param>
	/// <param name="iSrcHeight">Height of the source image data</param>
	/// <param name="iDestWidth">Width of the destination image data</param>
	/// <param name="iDestHeight">Height of the destination image data</param>
	/// <param name="pixelType">The pixel type of source and destination data</param>
	/// <param name="iFrameHeaderByteSize">The size of frame header in bytes. If there is no frame header then this value is set to 0</param>
	/// <param name="params">A valid device type must be provided and the other parameters are optional</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode PostProcessingCorrection_VarSrcDestDimensions(const void *pSrcData, void *pDestData, int iSrcWidth, int iSrcHeight, int iDestWidth, int iDestHeight, XcDataType iPixelType, int iFrameHeaderByteSize, const XcPostProcessingParam &params);

	/// <summary>
	/// Perfoms the edge correction on a single frame, each frame will have the frame header. 
	/// This function should be called after the gain correction is applied. The deviceType must be provided in order to apply correction at the correct edges
	/// The dimesnion of the destination can be different depending on whether the geometric correction is applied or not.
	/// Prior to this call GetImageDimensionsForPostProcessing should be called to get the dimension of the destination given the pot processing parameters
	/// </summary>
	/// <param name="pSrcData">Pointer to a single frame input data containing frame header</param>
	/// <param name="pDestData">Pointer to a single frame output data containing frame header</param>
	/// <param name="iSrcWidth">Width of the source image data</param>
	/// <param name="iSrcHeight">Height of the source image data</param>
	/// <param name="iDestWidth">Width of the destination image data</param>
	/// <param name="iDestHeight">Height of the destination image data</param>
	/// <param name="pixelType">The pixel type of source and destination data</param>
	/// <param name="iFrameHeaderByteSize">The size of frame header in bytes. If there is no frame header then this value is set to 0</param>
	/// <param name="params">A valid device type must be provided and the other parameters are optional</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode PostProcessingCorrectionROI_VarSrcDestDimensions(const void *pSrcData, void *pDestData, int iSrcWidth, int iSrcHeight, int iDestWidth, int iDestHeight, XcDataType iPixelType, int iROI_X, int iROI_Y, int iROI_Width, int iROI_Height, const XcPostProcessingParam &params);

	/// <summary>
	/// Perfoms the edge correction on multipl frames, each frame might have the frame header. 
	/// This function should be called after the gain correction is applied. The deviceType must be provided in order to apply correction at the correct edges
	/// The dimensions of source and destination can be the same otherwise the function GetImageDimensionsForPostProcessing should be called
	/// This function is generally used for doing post-processing for frame based correction where we may have multiple frames. PostProcessing performs enhancement on individual frames and the result 
	/// will be different compared to PostProcessingCorrection_MultiFrames as in the latter the full volume is used to do the correction
	/// </summary>
	/// <param name="ppInData">Double pointer to frames input data  which may/may not contain frame header</param>
	/// <param name="ppOutData">Double pointer to frames output data  which may/may not contain frame header</param>
	/// <param name="iSrcWidth">Width of the source image data</param>
	/// <param name="iSrcHeight">Height of the source image data</param>
	/// <param name="iDestWidth">Width of the destination image data. Can be the same as iSrcWidth unless the shifted columns are to be kept after geometric correction</param>
	/// <param name="iDestHeight">Height of the destination image data. Can be the same as iSrcHeight unless the shifted columns are to be kept after geometric correction</param>
	/// <param name="iNumFrames">Number of frames that </param>
	/// <param name="pixelType">The pixel type of input and output data</param>
	/// <param name="iFrameHeaderByteSize">The size of frame header in bytes. If there is no frame header then this value is set to 0</param>
	/// <param name="params">A valid device type must be provided and the other parameters are optional</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode PostProcessingCorrection_MultipleFrames(const void **ppInData, void **ppOutData, XcDataType iPixelType, int iSrcWidth, int iSrcHeight, int iDestWidth, int iDestHeight, int iNumFrame, int iFrameHeaderByteSize, const XcPostProcessingParam &params);


	/// <summary>
	/// Perfoms the edge correction on multipl frames, each frame might have the frame header. 
	/// This function should be called after the gain correction is applied. The deviceType must be provided in order to apply correction at the correct edges
	/// The dimensions of source and destination can be the same otherwise the function GetImageDimensionsForPostProcessing should be called
	/// This function is generally used for doing post-processing for frame based correction where we may have multiple frames. PostProcessing performs enhancement on individual frames and the result 
	/// will be different compared to PostProcessingCorrection_MultiFrames as in the latter the full volume is used to do the correction
	/// </summary>
	/// <param name="pInData">Pointer to frames input data  which may/may not contain frame header</param>
	/// <param name="pOutData">Pointer to frames output data  which may/may not contain frame header</param>
	/// <param name="iSrcWidth">Width of the source image data</param>
	/// <param name="iSrcHeight">Height of the source image data</param>
	/// <param name="iDestWidth">Width of the destination image data. Can be the same as iSrcWidth unless the shifted columns are to be kept after geometric correction</param>
	/// <param name="iDestHeight">Height of the destination image data. Can be the same as iSrcHeight unless the shifted columns are to be kept after geometric correction</param>
	/// <param name="iNumFrames">Number of frames that </param>
	/// <param name="pixelType">The pixel type of input and output data</param>
	/// <param name="iFrameHeaderByteSize">The size of frame header in bytes. If there is no frame header then this value is set to 0</param>
	/// <param name="params">A valid device type must be provided and the other parameters are optional</param>
	/// <returns>A error code with success = 0x1</returns>
	XCC_ReturnCode PostProcessingCorrection_MultipleFrames_Contiguous(const void *pInData, void *pOutData, XcDataType iPixelType, int iSrcWidth, int iSrcHeight, int iDestWidth, int iDestHeight, int iNumFrame, int iFrameHeaderByteSize, const XcPostProcessingParam &params);

	/// <summary>
	/// Perfoms defect correction given a valid defect map. 
	/// The defect correction is done as part of gain correction so in most cases this function will not be needed  
	/// </summary>
	/// <param name="pInData">Pointer to a single frame input data which may/may not contain frame header</param>
	/// <param name="pOutData">Pointer to a single frame output data which may/may not contain frame header</param>
	/// <param name="pOutData">Pointer to a defect map with no freame header. The defectmap should have the same dimension as the input/oputput images with defective pixels set to the value 1 </param>
	/// <param name="iWidth">Width of the image data</param>
	/// <param name="iHeight">Height of the image data</param>
	/// <param name="iNumFrames">Number of frames to be corrected </param>
	/// <param name="pixelType">The pixel type of input and output data</param>
	/// <param name="iFrameHeaderByteSize">The size of frame header in bytes. If there is no frame header then this value is set to 0</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode DefectCorrection(const void *pInData, void *pOutData, XcDataType iPixelType, unsigned char *pDefectMap, int iWidth, int iHeight, int iNumFrame, int iFrameHeaderByteSize);


	/// <summary>
	/// Returns the last error message that might have occured 
	/// </summary>
	/// <param name="errorMessage">The error message</param>
	/// <param name="iMessageSize">The number characters returned is equal to min(actual message length, iMessageSize)</param>
	/// <returns>Nothing is returned</returns>	
	void GetLastErrorMessage(char *errorMessage, int iMessageSize);



	/// <summary>
	/// stops a running process. It can be used by an external caller to terminate the running process 
	/// </summary>
	/// <returns>Nothing is returned</returns>	
	void StopProcess();

	
	/// <summary>
	/// indicates if currently any process running or not 
	/// </summary>
	/// <returns>Returns a boolean indicating if a process is running</returns>
	bool IsProcessingRunning();

	/// <summary>
	/// To see which stage the running process is at. It starts from 1 and ends at 100 when complete 
	/// </summary>
	/// <returns>Returns a boolean indicating if a process is running</returns>
	int GetProgressStage();

	/// <summary>
	/// Returns the version number of the dll 
	/// </summary>
	/// <param name="version">The caharcter array containing the version number</param>
	/// <param name="strLen">The size of the the character array to be returned</param>
	/// <returns>Returns a boolean indicating if operation was successful or not</returns>
	bool GetVersion(char *version, int iLen);

	//internal use
	//int GetTemperatureSensorNum();

	/// <summary>
	/// Writes content of the loaded parameter file to a folder for debugging purposes. The content will be saved as separate files  
	/// </summary>
	/// <param name="OutputFolder">The folder where the content will be saved to</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode WriteParameterFileContent(const char *OutputFolder);

	/// <summary>
	/// Writes content of a single parameter with the loaded parameter file  
	/// </summary>
	/// <param name="OutFilename">Raw images will be written to this file</param>
	/// <param name="tag">The name of the parameter which can be observed using WriteParameterSummary</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode WriteSingleParameterFileContent(const char *OutFilename, const char *tag);

	/// <summary>
	/// Writes content of the loaded parameter file to a text file containing description of the parameter file  
	/// </summary>
	/// <param name="SummaryFilename">The text file that will be filled with the parameter description</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode WriteParameterSummary(const char *SummaryFilename);

	/// <summary>
	/// Write log info to a file. The log information is collected during the operation of the calibration   
	/// </summary>
	/// <param name="LogFilename">The text file that will contain log information</param>
	/// <returns>A error code with success = 0x1</returns>	
	XCC_ReturnCode PrintLog(const char *LogFilename);

private:

	XcCalibrationBase *pCalib;
};



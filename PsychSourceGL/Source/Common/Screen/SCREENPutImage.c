/*
	PsychToolbox2/Source/Common/Screen/SCREENPutImage.cpp		
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
	
		Only OS X for now.
    
	HISTORY:

		02/06/03  	awi		Created.
		10/12/04	awi		In useString: moved commas to inside [].  Filled in synopsis
		1/14/05		awi		Commented out calls to enable alpha blending.
		2/25/05		awi		Added call to PsychUpdateAlphaBlendingFactorLazily().  Drawing now obeys settings by Screen('BlendFunction').
		2/25/07		cgb		Now lives in the GLfloat world.
 
  
	TO DO:
    
*/



#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] =  "Screen('PutImage', windowPtr, imageArray [,rect])";
//                                             1          2            3
static char synopsisString[] =
	"Copy the matrix \"imageArray\" to a window, slowly. "
	"\"rect\" is in window coordinates. The whole image is copied to \"rect\", scaling if "
	"necessary. The rect default is the imageArray's rect, centered in the window. "
	"The orientation of the array in the window is identical to that of Matlab's "
	"numerical array displays in the Command Window. The first pixel is in the upper "
	"left, and the rows are horizontal.";
static char seeAlsoString[] = "GetImage";
	
// Macro version of a function found in MiniBox.c.  Eliminates the unneeded overhead required by a function call.
// This improves speed by several milliseconds for medium to large images.
#define PSYCHINDEXELEMENTFROM3DARRAY(mDim, nDim, pDim, m, n, p) (p*mDim*nDim + n*mDim + m)

PsychError SCREENPutImage(void) 
{
	PsychRectType 		windowRect, positionRect;
	int 			ix, iy, numPlanes, matrixRedIndex, matrixGreenIndex, matrixBlueIndex, matrixAlphaIndex, matrixGrayIndex;
	int 			inputM, inputN, inputP, positionRectWidth, positionRectHeight, pixelIndex = 0;
	PsychWindowRecordType	*windowRecord;
	unsigned char		*inputMatrixByte;
	double			*inputMatrixDouble;
	GLfloat			*pixelData, matrixGrayValue, matrixRedValue, matrixGreenValue,
					matrixBlueValue, matrixAlphaValue, compactPixelValue;
	PsychArgFormatType	inputMatrixType;
	GLfloat			xZoom = 1, yZoom = -1;
	
	int di;
	mxArray *dpixdata;
	double *dp;
        
	// All sub functions should have these two lines.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if (PsychIsGiveHelp()) {
		PsychGiveHelp();
		return PsychError_none;
	};

	// Cap the number of inputs.
	PsychErrorExit(PsychCapNumInputArgs(4));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
        
	// Get the image matrix.
	inputMatrixType = PsychGetArgType(2);
	switch (inputMatrixType) {
		case PsychArgType_none:
		case PsychArgType_default:
			PsychErrorExitMsg(PsychError_user, "imageArray argument required");
			break;
		case PsychArgType_uint8:
			PsychAllocInUnsignedByteMatArg(2, TRUE, &inputM, &inputN, &inputP, &inputMatrixByte);
			break;
		case PsychArgType_double:
			PsychAllocInDoubleMatArg(2, TRUE, &inputM, &inputN, &inputP, &inputMatrixDouble);
			break;
		default:
			PsychErrorExitMsg(PsychError_user, "imageArray must be uint8 or double type");
			break;
	}
        
	// Get the window and get the rect and stuff.
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
	numPlanes = PsychGetNumPlanesFromWindowRecord(windowRecord);
	PsychGetRectFromWindowRecord(windowRect, windowRecord);
	if (PsychCopyInRectArg(3, FALSE, positionRect)) {
		if (IsPsychRectEmpty(positionRect)) {
			return PsychError_none;
		}
		positionRectWidth = (int)PsychGetWidthFromRect(positionRect);
		positionRectHeight = (int)PsychGetHeightFromRect(positionRect);
		if (inputP != 1 && inputP != 3 && inputP != 4) {
			PsychErrorExitMsg(PsychError_user, "Third dimension of image matrix must be 1, 3, or 4");
		}
		if (positionRectWidth != inputN  || positionRectHeight != inputM) {
			// Calculate the zoom factor.
			xZoom = (GLfloat)positionRectWidth/(GLfloat)inputN;
			yZoom = -((GLfloat)positionRectHeight/(GLfloat)inputM);
		}
	}
	else {
	   positionRect[kPsychLeft] = 0;
	   positionRect[kPsychTop] = 0;
	   positionRect[kPsychRight] = inputN;
	   positionRect[kPsychBottom] = inputM;
	   PsychCenterRect(positionRect, windowRect, positionRect);
	}
        
	// Put up the image.
	if (numPlanes == 1) {  //screen planes, not image matrix planes.  
		PsychErrorExitMsg(PsychError_unimplemented, "Put Image does not yet support indexed mode");
		//remember to test here for inputP==3 because that would be wrong. 
	}
	else if (numPlanes == 4) {
		// Allocate memory to hold the pixel data that we'll later pass to OpenGL.
		pixelData = (GLfloat*)PsychMallocTemp(sizeof(GLfloat) * inputN * inputM * 4);
		
		// Loop through all rows and columns of the pixel data passed from Matlab, extract it,
		// and stick it into 'pixelData'.
		for (iy = 0; iy < inputM; iy++) {
			for (ix = 0; ix < inputN; ix++) {
				if (inputP == 1) { // Grayscale
					// Extract the grayscale value.
					matrixGrayIndex = PSYCHINDEXELEMENTFROM3DARRAY(inputM, inputN, 1, iy, ix, 0);
					if (inputMatrixType == PsychArgType_uint8) {
						// If the color range is > 255, then force it to 255 for 8-bit values.
						matrixGrayValue = (GLfloat)inputMatrixByte[matrixGrayIndex];
						if (windowRecord->colorRange > 255) {
							matrixGrayValue /= (GLfloat)255;
						}
						else {
							matrixGrayValue /= (GLfloat)windowRecord->colorRange;
						}
					}
					else {
						matrixGrayValue = (GLfloat)(inputMatrixDouble[matrixGrayIndex] / windowRecord->colorRange);
					}
			
					// RGB will all be the same for grayscale.  We'll go ahead and fix alpha to the max value.
					pixelData[pixelIndex++] = matrixGrayValue; // R
					pixelData[pixelIndex++] = matrixGrayValue; // G
					pixelData[pixelIndex++] = matrixGrayValue; // B
					pixelData[pixelIndex++] = (GLfloat)1.0;	   // A
				}
				else if (inputP == 3) { // RGB
					matrixRedIndex = PSYCHINDEXELEMENTFROM3DARRAY(inputM, inputN, 3, iy, ix, 0);
					matrixGreenIndex = PSYCHINDEXELEMENTFROM3DARRAY(inputM, inputN, 3, iy, ix, 1);
					matrixBlueIndex = PSYCHINDEXELEMENTFROM3DARRAY(inputM, inputN, 3, iy, ix, 2);
					
					if (inputMatrixType == PsychArgType_uint8) {
						// If the color range is > 255, then force it to 255 for 8-bit values.
						matrixRedValue = (GLfloat)inputMatrixByte[matrixRedIndex];
						matrixGreenValue = (GLfloat)inputMatrixByte[matrixGreenIndex];
						matrixBlueValue = (GLfloat)inputMatrixByte[matrixBlueIndex];
						if (windowRecord->colorRange > 255) {
							matrixRedValue /= (GLfloat)255;
							matrixGreenValue /= (GLfloat)255;
							matrixBlueValue /= (GLfloat)255;
						}
						else {
							matrixRedValue /= (GLfloat)windowRecord->colorRange;
							matrixGreenValue /= (GLfloat)windowRecord->colorRange;
							matrixBlueValue /= (GLfloat)windowRecord->colorRange;
						}
					}
					else {
						matrixRedValue = (GLfloat)(inputMatrixDouble[matrixRedIndex] / windowRecord->colorRange);
						matrixGreenValue = (GLfloat)(inputMatrixDouble[matrixGreenIndex] / windowRecord->colorRange);
						matrixBlueValue = (GLfloat)(inputMatrixDouble[matrixBlueIndex] / windowRecord->colorRange);
					}
					
					pixelData[pixelIndex++] = matrixRedValue;
					pixelData[pixelIndex++] = matrixGreenValue;
					pixelData[pixelIndex++] = matrixBlueValue;
					pixelData[pixelIndex++] = (GLfloat)1.0;
				}
				else if (inputP == 4) { // RGBA
					matrixRedIndex = PSYCHINDEXELEMENTFROM3DARRAY(inputM, inputN, 4, iy, ix, 0);
					matrixGreenIndex = PSYCHINDEXELEMENTFROM3DARRAY(inputM, inputN, 4, iy, ix, 1);
					matrixBlueIndex = PSYCHINDEXELEMENTFROM3DARRAY(inputM, inputN, 4, iy, ix, 2);
					matrixAlphaIndex = PSYCHINDEXELEMENTFROM3DARRAY(inputM, inputN, 4, iy, ix, 3);
					
					if (inputMatrixType == PsychArgType_uint8) {
						// If the color range is > 255, then force it to 255 for 8-bit values.
						matrixRedValue = (GLfloat)inputMatrixByte[matrixRedIndex];
						matrixGreenValue = (GLfloat)inputMatrixByte[matrixGreenIndex];
						matrixBlueValue = (GLfloat)inputMatrixByte[matrixBlueIndex];
						matrixAlphaValue = (GLfloat)inputMatrixByte[matrixAlphaIndex];
						if (windowRecord->colorRange > 255) {
							matrixRedValue /= (GLfloat)255;
							matrixGreenValue /= (GLfloat)255;
							matrixBlueValue /= (GLfloat)255;
							matrixAlphaValue /= (GLfloat)255;
						}
						else {
							matrixRedValue /= (GLfloat)windowRecord->colorRange;
							matrixGreenValue /= (GLfloat)windowRecord->colorRange;
							matrixBlueValue /= (GLfloat)windowRecord->colorRange;
							matrixAlphaValue /= (GLfloat)windowRecord->colorRange;
						}
					}
					else {
						matrixRedValue = (GLfloat)(inputMatrixDouble[matrixRedIndex] / windowRecord->colorRange);
						matrixGreenValue = (GLfloat)(inputMatrixDouble[matrixGreenIndex] / (GLfloat)windowRecord->colorRange);
						matrixBlueValue = (GLfloat)(inputMatrixDouble[matrixBlueIndex] / (GLfloat)windowRecord->colorRange);
						matrixAlphaValue = (GLfloat)(inputMatrixDouble[matrixAlphaIndex] / (GLfloat)windowRecord->colorRange);
					}
					
					pixelData[pixelIndex++] = matrixRedValue;
					pixelData[pixelIndex++] = matrixGreenValue;
					pixelData[pixelIndex++] = matrixBlueValue;
					pixelData[pixelIndex++] = matrixAlphaValue;
				}
			} // for (iy = 0; iy < inputM; iy++)
		} // for (ix = 0; ix < inputN; ix++)

		// Enable GL-Context of current onscreen window:
		PsychSetGLContext(windowRecord);
		// Enable this windowRecords framebuffer as current drawingtarget:
		PsychSetDrawingTarget(windowRecord);
		
		PsychUpdateAlphaBlendingFactorLazily(windowRecord);

		// Set the raster position so that we can draw starting at this location.
		glRasterPos2f((GLfloat)(positionRect[kPsychLeft]), (GLfloat)(positionRect[kPsychTop]));
		PsychTestForGLErrors();
		
		// Tell glDrawPixels to unpack the pixel array along GLfloat boundaries.
		glPixelStorei(GL_UNPACK_ALIGNMENT, (GLint)sizeof(GLfloat));
		PsychTestForGLErrors();
		
		glPixelZoom(xZoom, yZoom);
		PsychTestForGLErrors();
		      
		// Dump the pixels onto the screen.
		glDrawPixels(inputN, inputM, GL_RGBA, GL_FLOAT, pixelData);
		PsychTestForGLErrors();
		
		PsychFlushGL(windowRecord);  // OS X: This does nothing if we are multi buffered, otherwise it glFlushes
		PsychTestForGLErrors();
	}
	else if(numPlanes == 3) {
		PsychErrorExitMsg(PsychError_unimplemented, "PutImage found hardware without an alpha channel.");
	}

	return PsychError_none;
}


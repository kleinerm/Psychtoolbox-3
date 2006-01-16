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
	

PsychError SCREENPutImage(void) 
{
	PsychRectType 		windowRect,positionRect;
	int 			ix, iy, numPlanes, bitsPerColor, matrixRedIndex, matrixGreenIndex, matrixBlueIndex, matrixAlphaIndex, matrixGrayIndex;
	int 			inputM, inputN, inputP, positionRectWidth, positionRectHeight; 
	PsychWindowRecordType	*windowRecord;
	unsigned char		*inputMatrixByte;
	double			*inputMatrixDouble;
	GLuint			*compactMat, matrixGrayValue, matrixRedValue, matrixGreenValue, matrixBlueValue, matrixAlphaValue, compactPixelValue;
	PsychArgFormatType	inputMatrixType;
	GLfloat			xZoom=1, yZoom=-1;
        
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	//cap the number of inputs
	PsychErrorExit(PsychCapNumInputArgs(4));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
        
        //get the image matrix
        inputMatrixType=PsychGetArgType(2);
        switch(inputMatrixType){
            case PsychArgType_none : case PsychArgType_default:
                PsychErrorExitMsg(PsychError_user, "imageArray argument required");
                break;
            case PsychArgType_uint8 :
                PsychAllocInUnsignedByteMatArg(2, TRUE, &inputM, &inputN, &inputP, &inputMatrixByte);
                break;
            case PsychArgType_double :
                PsychAllocInDoubleMatArg(2, TRUE, &inputM, &inputN, &inputP, &inputMatrixDouble);
                break;
            default :
                PsychErrorExitMsg(PsychError_user, "imageArray must be uint8 or double type");
                break;
        }
        
        //get the window and get the rect and stuff
        PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
        numPlanes=PsychGetNumPlanesFromWindowRecord(windowRecord);
        bitsPerColor=PsychGetColorSizeFromWindowRecord(windowRecord);
        PsychGetRectFromWindowRecord(windowRect, windowRecord);
	if(PsychCopyInRectArg(3, FALSE, positionRect)){
            positionRectWidth=(int)PsychGetWidthFromRect(positionRect);
            positionRectHeight=(int)PsychGetHeightFromRect(positionRect);
            if(inputP != 1 && inputP != 3 && inputP != 4)
                PsychErrorExitMsg(PsychError_user, "Third dimension of image matrix must be 1, 3, or 4"); 
            if( positionRectWidth != inputN  || positionRectHeight != inputM){
                //calculate the zoom factor
                xZoom=(GLfloat)positionRectWidth/(GLfloat)inputN;
                yZoom=-((GLfloat)positionRectHeight/(GLfloat)inputM);
            }
        }else{
           positionRect[kPsychLeft]=0;
           positionRect[kPsychTop]=0;
           positionRect[kPsychRight]=inputN;
           positionRect[kPsychBottom]=inputM;
           PsychCenterRect(positionRect, windowRect, positionRect);
           //This should be centered  
        }
        
        
        //put up the image
        if(numPlanes==1){  //screen planes, not image matrix planes.  
            PsychErrorExitMsg(PsychError_unimplemented, "Put Image does not yet support indexed mode");
            //remember to test here for inputP==3 because that would be wrong. 
        }else if(numPlanes==4){
            compactMat=(GLuint *)mxMalloc(sizeof(GLuint) * inputN * inputM);
            for(ix=0;ix<inputN;ix++){
                for(iy=0;iy<inputM;iy++){
                    if(inputP==1){
                        matrixGrayIndex=PsychIndexElementFrom3DArray(inputM, inputN, 1, iy, ix, 0);
                        if(inputMatrixType==PsychArgType_uint8)
                            matrixGrayValue=(GLuint)inputMatrixByte[matrixGrayIndex];
                        else //inputMatrixType==PsychArgType_double
                            matrixGrayValue=(GLuint)inputMatrixDouble[matrixGrayIndex];
                
                        compactPixelValue=((matrixGrayValue<<8 | matrixGrayValue)<<8 | matrixGrayValue)<<8 | 255; 
                        compactMat[iy*inputN+ix]=compactPixelValue; 
                    }else if(inputP==3){
                        matrixRedIndex=PsychIndexElementFrom3DArray(inputM, inputN, 3, iy, ix, 0);
                        matrixGreenIndex=PsychIndexElementFrom3DArray(inputM, inputN, 3, iy, ix, 1);
                        matrixBlueIndex=PsychIndexElementFrom3DArray(inputM, inputN, 3, iy, ix, 2);
                        if(inputMatrixType==PsychArgType_uint8){
                            matrixRedValue=(GLuint)inputMatrixByte[matrixRedIndex];
                            matrixGreenValue=(GLuint)inputMatrixByte[matrixGreenIndex];
                            matrixBlueValue=(GLuint)inputMatrixByte[matrixBlueIndex];
                            matrixAlphaValue=(GLuint)255;
                        }else{
                            matrixRedValue=(GLuint)inputMatrixDouble[matrixRedIndex];
                            matrixGreenValue=(GLuint)inputMatrixDouble[matrixGreenIndex];
                            matrixBlueValue=(GLuint)inputMatrixDouble[matrixBlueIndex];
                            matrixAlphaValue=(GLuint)255;
                        }
                        compactPixelValue= ((matrixRedValue<<8 | matrixGreenValue )<<8 | matrixBlueValue)<<8 | matrixAlphaValue; 
                        compactMat[iy*inputN+ix]=compactPixelValue; 
                    }else if(inputP==4){
                        matrixRedIndex=PsychIndexElementFrom3DArray(inputM, inputN, 3, iy, ix, 0);
                        matrixGreenIndex=PsychIndexElementFrom3DArray(inputM, inputN, 3, iy, ix, 1);
                        matrixBlueIndex=PsychIndexElementFrom3DArray(inputM, inputN, 3, iy, ix, 2);
                        matrixAlphaIndex=PsychIndexElementFrom3DArray(inputM, inputN, 3, iy, ix, 3);
                        if(inputMatrixType==PsychArgType_uint8){  
                            matrixRedValue=(GLuint)inputMatrixByte[matrixRedIndex];
                            matrixGreenValue=(GLuint)inputMatrixByte[matrixGreenIndex];
                            matrixBlueValue=(GLuint)inputMatrixByte[matrixBlueIndex];
                            matrixAlphaValue=(GLuint)inputMatrixByte[matrixAlphaIndex];
                        }else{
                            matrixRedValue=(GLuint)inputMatrixDouble[matrixRedIndex];
                            matrixGreenValue=(GLuint)inputMatrixDouble[matrixGreenIndex];
                            matrixBlueValue=(GLuint)inputMatrixDouble[matrixBlueIndex];
                            matrixAlphaValue=(GLuint)inputMatrixDouble[matrixAlphaIndex];
                        }
                        compactPixelValue= ((matrixRedValue<<8 | matrixGreenValue )<<8 | matrixBlueValue)<<8 | matrixAlphaValue; 
                        compactMat[iy*inputN+ix]=compactPixelValue; 

                    }

                 }
            }

            // Enable GL-Context of current onscreen window:
            PsychSetGLContext(windowRecord);
            // Enable this windowRecords framebuffer as current drawingtarget:
            PsychSetDrawingTarget(windowRecord);
            
            PsychUpdateAlphaBlendingFactorLazily(windowRecord);

            glRasterPos2i((GLint)(positionRect[kPsychLeft]), (GLint)(positionRect[kPsychTop]));
            PsychTestForGLErrors();
            glPixelStorei(GL_UNPACK_ALIGNMENT, (GLint)(sizeof(GLuint)));  //4  
            PsychTestForGLErrors();
            glPixelZoom(xZoom,yZoom);
            PsychTestForGLErrors();            
            glDrawPixels(inputN, inputM, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, compactMat);
            free((void *)compactMat);
            PsychTestForGLErrors();
            PsychFlushGL(windowRecord);  //OS X: This does nothing if we are multi buffered, otherwise it glFlushes
            PsychTestForGLErrors();
        }else if(numPlanes==3)
            PsychErrorExitMsg(PsychError_unimplemented, "PutImage found hardware without an alpha channel.");	

	return(PsychError_none);
}

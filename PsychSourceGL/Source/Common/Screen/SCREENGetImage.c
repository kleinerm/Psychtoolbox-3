/*
	SCREENGetImage.c		
  
	AUTHORS:
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
	
		Only OS X for now.
    
	HISTORY:
	
		01/08/03  	awi		Created.
		10/12/04	awi		In useString: moved commas to inside [].

 
	TO DO:
    
		We could probably speed this up by using a single call to glReadPixels instead of three.  It doesn't matter what we pack the result into,
		since we have to iterate over and copy out from the memory which glReadPixels fills so to rearrange correclty into the return matrix. None
		of the glReadPixels modes will fill the return matrix in the same format that MATLAB wants.  
*/



#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] =  "imageArray=Screen('GetImage', windowPtr [,rect] [,bufferName])";
//                                                        1           2       3
static char synopsisString[] =
	"Slowly copy an image from a window, returning a Matlab uint8 array. If the "
	"window's pixelSize>8 then the returned imageArray has three layers, i.e. it's an "
	"RGB image. \"rect\" is in window coordinates, and its default is the whole window. "
	"[CHANGE: as of April, 1998, imageArray is now uint8 instead of double to save "
	"space (a factor of 8).] Matlab will complain if you try to do math on a uint8 "
	"array, so you may need to use DOUBLE to convert it, e.g. imageArray/255 will "
	"produce an error, but double(imageArray)/255 is ok. Also see Screen 'PutImage' "
	"and 'CopyWindow', and the clipboard functions PasteImage.mex and CopyImage.mex. "
	"bufferName is a string specifying theh buffer name, either \"frontBuffer\" or "
	"\backBuffer\".";
static char seeAlsoString[] = "PutImage";
	

PsychError SCREENGetImage(void) 
{
	PsychRectType 		windowRect,sampleRect;
        int 			ix, iy, numPlanes, bitsPerColor, sampleRectWidth, sampleRectHeight, invertedY, redReturnIndex, greenReturnIndex, blueReturnIndex, planeSize;
        ubyte 			*returnArrayBase, *redPlane, *greenPlane, *bluePlane;
        PsychWindowRecordType	*windowRecord;
        //char 			*bufferFlagNames[] = {"frontBuffer", "backBuffer", NULL};
        GLenum			whichBuffer; 
        //GLenum		glBuffers[] = {GL_FRONT, GL_BACK, NULL};
        GLboolean		isDoubleBuffer;
        
        
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	//cap the numbers of inputs and outputs
	PsychErrorExit(PsychCapNumInputArgs(2));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(1));  //The maximum number of outputs
        
        glGetBooleanv(GL_DOUBLEBUFFER, &isDoubleBuffer);
	
        PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);

        // Enable GL-Context of current onscreen window:
        PsychSetGLContext(windowRecord);
        // Enable this windowRecords framebuffer as current drawingtarget:
        PsychSetDrawingTarget(windowRecord);
        
        numPlanes=PsychGetNumPlanesFromWindowRecord(windowRecord);
        bitsPerColor=PsychGetColorSizeFromWindowRecord(windowRecord);
        PsychGetRectFromWindowRecord(windowRect, windowRecord);
	if(!PsychCopyInRectArg(2, FALSE, sampleRect))
            memcpy(sampleRect, windowRect, sizeof(PsychRectType));
        whichBuffer=GL_FRONT; 
        //glReadBuffer(whichBuffer);
        
        //PsychCopyInNamedFlagIndexArg(3, FALSE, bufferFlagNames, bufferFlagIndices, &whichBuffer);  //not yet implemented.  
        sampleRectWidth=PsychGetWidthFromRect(sampleRect);
        sampleRectHeight=PsychGetHeightFromRect(sampleRect);
        
        if(numPlanes==1)
            PsychErrorExitMsg(PsychError_unimplemented, "GetImage does not yet support indexed mode");
        else if(numPlanes==3 || numPlanes==4){
            //Note this will only work correctly for 8-bit pixels.  
            if(PsychAllocOutUnsignedByteMatArg(1, TRUE, sampleRectHeight, sampleRectWidth, 3, &returnArrayBase)){
                /*
                redPlaneReturn=returnArrayBase + PsychIndexPlaneFrom3DArray(sampleRectHeight, sampleRectWidth, 3, 0);
                greenPlaneReturn=returnArrayBase + PsychIndexPlaneFrom3DArray(sampleRectHeight, sampleRectWidth, 3, 1);
                bluePlaneReturn=returnArrayBase + PsychIndexPlaneFrom3DArray(sampleRectHeight, sampleRectWidth, 3, 2);
                */
                redPlane= mxMalloc(3 * sizeof(GL_UNSIGNED_BYTE) * sampleRectWidth * sampleRectHeight);
                planeSize=sampleRectWidth * sampleRectHeight;
                greenPlane= redPlane + planeSize;
                bluePlane= redPlane + 2 * planeSize;
                glPixelStorei(GL_PACK_ALIGNMENT,1);
                invertedY=windowRect[kPsychBottom]-sampleRect[kPsychBottom];
                glReadPixels(sampleRect[kPsychLeft],invertedY, 	sampleRectWidth, sampleRectHeight, GL_RED, GL_UNSIGNED_BYTE, redPlane); 
                glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_GREEN, GL_UNSIGNED_BYTE, greenPlane);
                glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_BLUE, GL_UNSIGNED_BYTE, bluePlane);
                //in one pass transpose and flip what we read with glReadPixels before returning.  
                //-glReadPixels insists on filling up memory in sequence by reading the screen row-wise whearas Matlab reads up memory into columns.
                //-the Psychtoolbox screen as setup by gluOrtho puts 0,0 at the top left of the window but glReadPixels always believes that it's at the bottom left.     
                for(ix=0;ix<sampleRectWidth;ix++){
                    for(iy=0;iy<sampleRectHeight;iy++){
                        redReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth, 3, iy, ix, 0);
                        greenReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth, 3, iy, ix, 1);
                        blueReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth, 3, iy, ix, 2);
                        returnArrayBase[redReturnIndex]=redPlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];  
                        returnArrayBase[greenReturnIndex]=greenPlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];
                        returnArrayBase[blueReturnIndex]=bluePlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];
                    }
                }
            }
        }
        else 
            PsychErrorExitMsg(PsychError_internal, "Unexpected number of color planes");	

	return(PsychError_none);
}

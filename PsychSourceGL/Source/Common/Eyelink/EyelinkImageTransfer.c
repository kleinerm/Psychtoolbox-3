/*
 EyelinkImageTransfer.c		
 
 AUTHORS:
 
 nuha@sr-research.com nj
 
 PLATFORMS:
 
 All.
 
 
 HISTORY:
 
 mm/dd/yy
 
 10/10/10	nj		Created. 
 
 */

#include "PsychEyelink.h"
#include "bitmap.h"
//#include "core_expt.h"
#include <stdlib.h>
#include <stdio.h>

# ifdef WIN32
	#include <GLUT/glut.h>
#else
	#include <GLUT/glut.h>
#endif

// Help strings
static char useString[] =  "[status =] Eyelink('ImageTransfer', 'imagePath', 'X Position', 'Y Position','Width','Height', tracker X Position', 'tracker Y Position', 'xferoptions')";

static char synopsisString[] =
" This function transfers a 24-bit or 32-bit bitmap to the tracker PC as backdrop for gaze cursors"
" width or height set to zero will use bitmap height and width\n"
" xferoptions Transfer options set with bitwise OR of the following constants, determines how bitmap is processed:"
" 0 Averaging combined pixels"
" 1 Choosing darkest and keep thin dark lines."
" 2 Choosing darkest and keep thin white lines and control how bitmap size is"
" reduced to fit tracker display."
" 4 Maximizes contrast for clearest image."
" 8 Disables the dithering of the image."
" 16 Converts the image to grayscale (grayscale works best for EyeLinkI,text,etc.).";

static char seeAlsoString[] = "";

PsychError EyelinkImageTransfer(void)
{
	int				iStatus = -1;
	int				xs;
	int				ys;
	int				width;
	int				height;
	int				xd;
	int				yd;
	int				i;
	int				xferoptions;
	char			*filename;		
	int				pitch,j,size;
	int				pitch24, pitch32;
	EYEBITMAP		bmp;		
	BITMAPINFO		*BitmapInfo; /* Bitmap information */
	GLubyte			*BitmapBits; /* Bitmap data */
	EYEPIXELFORMAT  *pfp;
	
#if PSYCH_SYSTEM == PSYCH_WINDOWS

	BYTE* pPixels;	
	BYTE *dest;
	BYTE *src;
	BYTE* pDest;
	BYTE* pReturn;

#else
	byte* pPixels;
	byte* dest;
	byte* src;
	byte* pDest;
	byte* pReturn;
#endif

	PsychPushHelp(useString, synopsisString, seeAlsoString);

	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}
	
	
	// assign arguments
	PsychAllocInCharArg(1, TRUE, &filename);
	PsychCopyInIntegerArg(2, TRUE, &xs);
	PsychCopyInIntegerArg(3, TRUE, &ys);
	PsychCopyInIntegerArg(4, TRUE, &width);
	PsychCopyInIntegerArg(5, TRUE, &height);
	PsychCopyInIntegerArg(6, TRUE, &xd);
	PsychCopyInIntegerArg(7, TRUE, &yd);
	PsychCopyInIntegerArg(8, TRUE, &xferoptions);
		
	BitmapBits = LoadDIBitmap(filename, &BitmapInfo);
	
	if (BitmapBits == NULL){		
		PsychErrorExitMsg(PsychError_user, "Loading Bitmap failed\n");
		fflush(stdout);
	}

	bmp.w = (int) BitmapInfo->bmiHeader.biWidth;
	bmp.h = (int) BitmapInfo->bmiHeader.biHeight;
	bmp.depth = (int) 32;
	bmp.pitch = (int) bmp.w * 4;

	pPixels = BitmapBits;
	
	size = (((int) BitmapInfo->bmiHeader.biBitCount*(int) BitmapInfo->bmiHeader.biWidth+31)/32)*4*bmp.h;  

	//invert	
	pitch24   = ((abs(bmp.w)*24+31)/32)*4; 
	pitch32   = abs(bmp.w) * 4;
	pDest    = (byte*) malloc(sizeof(byte)*pitch32*abs(bmp.h));
	pReturn = (byte*) malloc(sizeof(byte)*pitch32*abs(bmp.h));
	
	if (!pDest || !pReturn){
		
		if (pDest) free(pDest);
		if (pReturn) free(pReturn);
		PsychErrorExitMsg(PsychError_user, "Loading Bitmap failed\n");
		fflush(stdout);
	}
	
	memset(pDest,   0, pitch32*abs(bmp.h));
	memset(pReturn, 0, pitch32*abs(bmp.h));
		
		
	if ((int) BitmapInfo->bmiHeader.biBitCount == 24){			
		
		for( i=0; i<abs(bmp.h); ++i)
			for( j=0; j<abs(bmp.w); ++j)
			{
				pDest[i*pitch32+j*4+0] = BitmapBits[i*pitch24+j*3+0]; 
				pDest[i*pitch32+j*4+1] = BitmapBits[i*pitch24+j*3+1];
				pDest[i*pitch32+j*4+2] = BitmapBits[i*pitch24+j*3+2];
				pDest[i*pitch32+j*4+3] = 0;
			}
		
		if(bmp.w>0 && bmp.h>0)
		{
			pitch = bmp.w*4; // for 32bits img
			
			for(i=0; i<bmp.h; ++i)
			{
				byte *dest = pReturn +((bmp.h-1-i)*(pitch));
				byte *src = pDest +(i*pitch);
				for(j=0; j<pitch; ++j)
					dest[j]= src[j];
			}		
		}
		else
		{
			memcpy(pReturn, pDest, abs(bmp.w)*4*abs(bmp.h));
		}
		bmp.pixels = pReturn;
		
	}else {		
		if(bmp.w>0 && bmp.h>0)
		{
			pitch = bmp.w*4;		
			for(i=0; i<bmp.h; ++i)
			{
				byte *dest = pReturn +((bmp.h-1-i)*(pitch));
				byte *src = pPixels +(i*pitch);
				for(j=0; j<pitch; ++j)
					dest[j]= src[j];
			}
		
		}else {
			memcpy(pReturn, pPixels, abs(bmp.w)*4*abs(bmp.h));
		}
			bmp.pixels = pReturn; 
		}
		
	// Check arguments
	PsychErrorExit(PsychCapNumInputArgs(8));
	PsychErrorExit(PsychRequireNumInputArgs(8));
	PsychErrorExit(PsychCapNumOutputArgs(1));
		
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
		
    iStatus = el_bitmap_to_backdrop(&bmp, xs, ys, width, height, xd, yd, xferoptions);

	if (BitmapInfo)
	{
		free(BitmapInfo);
		if (BitmapBits) free(BitmapBits);
	}

	PsychCopyOutDoubleArg(1, FALSE, iStatus);

	if (pDest) free(pDest);
	if (pReturn) free(pReturn);
	
	
	return(PsychError_none);
	
	
}
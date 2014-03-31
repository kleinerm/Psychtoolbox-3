/*

EyelinkImageTransfer.c		

AUTHORS:

nuha@sr-research.com nj
li@sr-research.com   lj

PLATFORMS:

All.

HISTORY:

10/10/10	nj		Created.
05/24/11    mk      Cleanup.
12/20/13    lj      fixed EyelinkImageTransfer for crash due to unallocated memory in EYEBITMAP.
 

*/

#include "PsychEyelink.h"
#include "bitmap.h"

// Help strings
static char useString[] = "[status] = Eyelink('ImageTransfer', imagePath [, xPosition=0][, yPosition=0][, width=0][, height=0][, trackerXPosition=0][, trackerYPosition=0][, xferoptions=0]);";

static char synopsisString[] =
"This function transfers a 24-bit or 32-bit bitmap to the tracker PC as backdrop for gaze cursors.\n"
"'width' and 'height' define image size. If set to zero will use bitmap height and width.\n"
"'xferoptions' Transfer options set with bitwise OR of the following constants, determines how bitmap is processed:\n"
" 0 Averaging combined pixels\n"
" 1 Choosing darkest and keep thin dark lines\n"
" 2 Choosing darkest and keep thin white lines and control how bitmap size is reduced to fit tracker display\n"
" 4 Maximizes contrast for clearest image\n"
" 8 Disables the dithering of the image\n"
" 16 Converts the image to grayscale (grayscale works best for EyeLinkI,text,etc.)\n";

static char seeAlsoString[] = "";

PsychError EyelinkImageTransfer(void)
{
	int				iStatus = -1;
	int				xs = 0;
	int				ys = 0;
	int				width = 0;
	int				height = 0;
	int				xd = 0;
	int				yd = 0;
	int				i;
	int				xferoptions = 0;
	char			*filename;		
	int				pitch,j,size;
	int				pitch24, pitch32;
	EYEBITMAP		bmp;		
	BITMAPINFO		*BitmapInfo; /* Bitmap information */
	psych_uint8		*BitmapBits; /* Bitmap data */
	
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
    
	// Check arguments
	PsychErrorExit(PsychCapNumInputArgs(8));
	PsychErrorExit(PsychRequireNumInputArgs(1));
	PsychErrorExit(PsychCapNumOutputArgs(1));
    
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
    
	// assign arguments
	PsychAllocInCharArg(1, TRUE, &filename);
	PsychCopyInIntegerArg(2, FALSE, &xs);
	PsychCopyInIntegerArg(3, FALSE, &ys);
	PsychCopyInIntegerArg(4, FALSE, &width);
	PsychCopyInIntegerArg(5, FALSE, &height);
	PsychCopyInIntegerArg(6, FALSE, &xd);
	PsychCopyInIntegerArg(7, FALSE, &yd);
	PsychCopyInIntegerArg(8, FALSE, &xferoptions);
    
	BitmapBits = LoadDIBitmap(filename, &BitmapInfo);
	
	if (BitmapBits == NULL){		
		PsychErrorExitMsg(PsychError_user, "Loading Bitmap failed\n");
	}
    
	bmp.w = (int) BitmapInfo->bmiHeader.biWidth;
	bmp.h = (int) BitmapInfo->bmiHeader.biHeight;
	bmp.depth = (int) 32;
	bmp.pitch = (int) bmp.w * 4;
    
	pPixels = BitmapBits;
	
	size = (((int) BitmapInfo->bmiHeader.biBitCount * (int) BitmapInfo->bmiHeader.biWidth + 31) / 32) * 4 * bmp.h;  
    
	// invert
	pitch24 = ((abs(bmp.w) * 24 + 31) / 32) * 4; 
	pitch32 = abs(bmp.w) * 4;
	pDest   = (byte*) PsychMallocTemp(sizeof(byte) * pitch32 * abs(bmp.h));
	pReturn = (byte*) PsychMallocTemp(sizeof(byte) * pitch32 * abs(bmp.h));
	if (!pDest || !pReturn) PsychErrorExitMsg(PsychError_outofMemory, "Out of memory while trying to load bitmap!");
    
	memset(pDest,   0, pitch32 * abs(bmp.h));
	memset(pReturn, 0, pitch32 * abs(bmp.h));
    
	if ((int) BitmapInfo->bmiHeader.biBitCount == 24) {
		for( i=0; i<abs(bmp.h); ++i)
			for( j=0; j<abs(bmp.w); ++j) {
				pDest[i*pitch32+j*4+0] = BitmapBits[i*pitch24+j*3+0]; 
				pDest[i*pitch32+j*4+1] = BitmapBits[i*pitch24+j*3+1];
				pDest[i*pitch32+j*4+2] = BitmapBits[i*pitch24+j*3+2];
				pDest[i*pitch32+j*4+3] = 0;
			}
                
                if(bmp.w>0 && bmp.h>0) {
                    pitch = bmp.w*4; // for 32bits img
                    
                    for(i=0; i<bmp.h; ++i) {
                        dest = pReturn +((bmp.h-1-i)*(pitch));
                        src = pDest +(i*pitch);
                        for(j=0; j<pitch; ++j)
                            dest[j]= src[j];
                    }
                }
                else {
                    memcpy(pReturn, pDest, abs(bmp.w)*4*abs(bmp.h));
                }
                bmp.pixels = pReturn;
	} else {
		if(bmp.w>0 && bmp.h>0) {
			pitch = bmp.w*4;	
			for(i=0; i<bmp.h; ++i)
			{
				dest = pReturn +((bmp.h-1-i)*(pitch));
				src = pPixels +(i*pitch);
				for(j=0; j<pitch; ++j)
					dest[j]= src[j];
			}
		} else {
			memcpy(pReturn, pPixels, abs(bmp.w)*4*abs(bmp.h));
		}
        
        bmp.pixels = pReturn; 
    }
    bmp.format = (EYEPIXELFORMAT* ) PsychMallocTemp(sizeof(EYEPIXELFORMAT));
    memset(bmp.format, 0, sizeof(EYEPIXELFORMAT));
	
	
	bmp.format->Rmask = 0x000000ff;
	bmp.format->Gmask = 0x0000ff00;
	bmp.format->Bmask = 0x00ff0000;
	bmp.format->Amask = 0xff000000;
   
    iStatus = el_bitmap_to_backdrop(&bmp, xs, ys, width, height, xd, yd, xferoptions);
    
	if (BitmapInfo) {
		free(BitmapInfo);
		if (BitmapBits) free(BitmapBits);
	}
    if(bmp.format)
	{		
		if(bmp.format) PsychFreeTemp(bmp.format);
	}

    
	PsychCopyOutDoubleArg(1, FALSE, iStatus);
    
	return(PsychError_none);
}

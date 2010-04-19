/*
	Common/Screen/PsychMovieWritingSupportQuickTime.c
	
	PLATFORMS:	
	
		MacOS/X and MS-Windows
		
		This is the movie editing and writing/creation engine based on the
		Apple QuickTime API. It works on Apple MacOS/X and MS-Windows.

	AUTHORS:
	
		Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de

	HISTORY:
	
	04/18/10		mk		Wrote it. 
	
	DESCRIPTION:
	
		Psychtoolbox functions for dealing with Quicktime movie editing. This implementation uses
		Quicktimes API's, therefore it works on Operating systems with Quicktime support,
		currently OS/X and Windows.

	NOTES:

*/

#include "Screen.h"

// No Quicktime support for GNU/Linux:
#if PSYCH_SYSTEM != PSYCH_LINUX

#if PSYCH_SYSTEM == PSYCH_OSX
#include <Quicktime/QuickTimeComponents.h>
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
#include <QTML.h>
#include <QuickTimeComponents.h>
#endif

// Forward declaration of internal helper function:


// Record which defines all state for a capture device:
typedef struct {
	Movie						Movie;
	Track						Track;
	Media						Media;
	FSSpec						File;
	short						ResRefNum;
	short						ResID;
	GWorldPtr					GWorld;
	PixMapHandle				PixMap;
	CodecType					CodecType;
	CodecQ						CodecQuality;
	long						MaxComprSize;
	Handle						ComprDataHdl;
	Ptr							ComprDataPtr;
	ImageDescriptionHandle		ImageDesc;
	CGrafPtr 					SavedPort;
	GDHandle					SavedDevice;
	Rect						Rect;
	int							height;
	int							width;
	int							padding;
} PsychMovieWriterRecordType;

static PsychMovieWriterRecordType moviewriterRecordBANK[PSYCH_MAX_MOVIEWRITERDEVICES];
static int moviewritercount = 0;
static psych_bool firsttime = TRUE;

void PsychMovieWritingInit(void)
{
	int i;
	
	for (i = 0; i < PSYCH_MAX_MOVIEWRITERDEVICES; i++) {
		memset(&(moviewriterRecordBANK[i]), 0, sizeof(PsychMovieWriterRecordType));
	}
	
	moviewritercount = 0;
	
	return;
}

void PsychDeleteAllMovieWriters(void)
{
	int i;
		
	for (i = 0; i < PSYCH_MAX_MOVIEWRITERDEVICES; i++) {
		if (moviewriterRecordBANK[i].Movie) PsychFinalizeNewMovieFile(i);
	}
}

void PsychExitMovieWriting(void)
{
	PsychDeleteAllMovieWriters();
    firsttime = TRUE;
	return;
}

PsychMovieWriterRecordType* PsychGetMovieWriter(int moviehandle, psych_bool unsafe)
{
	if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIEWRITERDEVICES) PsychErrorExitMsg(PsychError_user, "Invalid handle for moviewriter provided!");
	if (!unsafe && (NULL == moviewriterRecordBANK[moviehandle].Movie)) PsychErrorExitMsg(PsychError_user, "Invalid handle for moviewriter provided! No such writer open.");
	return(&(moviewriterRecordBANK[moviehandle]));
}

unsigned char*	PsychGetVideoFrameForMoviePtr(int moviehandle, unsigned int* twidth, unsigned int* theight)
{
	PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(moviehandle, FALSE);
	if (NULL == pwriterRec->PixMap) return(NULL);

	*twidth = pwriterRec->width;
	*theight = pwriterRec->height - pwriterRec->padding; // Hack hack pwriterRec->padding !
	return((unsigned char*) GetPixBaseAddr(pwriterRec->PixMap));
}

int PsychAddVideoFrameToMovie(int moviehandle, int frameDurationUnits, psych_bool isUpsideDown)
{
	PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(moviehandle, FALSE);

	OSErr				myErr = noErr;
	int					x, y, w, h;
	unsigned char*		pixptr   = (unsigned char*) GetPixBaseAddr(pwriterRec->PixMap);
	unsigned int*		wordptr  = (unsigned int*) GetPixBaseAddr(pwriterRec->PixMap);
	unsigned int		*wordptr2, *wordptr1;
	unsigned int		dummy;

	if (NULL == pwriterRec->Media) return(0);

	// Draw testpattern: Disabled at compile-time by default:
	if (FALSE) {
		for (y = 0; y < pwriterRec->height; y++) {
			for (x = 0; x < pwriterRec->width; x++) {
				*(pixptr++) = (unsigned char) 255; // alpha
				*(pixptr++) = (unsigned char) y; // Red
				*(pixptr++) = (unsigned char) x; // Green
				*(pixptr++) = (unsigned char) 0; // Blue
			}
		}
	}
	
	// Imagebuffer is upside-down: Need to flip it vertically:
	if (isUpsideDown) {
		h = pwriterRec->height - pwriterRec->padding; // Hack pwriterRec->padding !
		w = pwriterRec->width;
		wordptr1 = wordptr;
		for (y = 0; y < h/2; y++) {
			wordptr2 = wordptr;
			wordptr2 += ((h - 1 - y) * w);
			for (x = 0; x < w; x++) {
				dummy = *wordptr1;
				*(wordptr1++) = *wordptr2;
				*(wordptr2++) = dummy;				 
			}
		}
	}
	
	// Apply codec to compress image:
	myErr = CompressImage(	pwriterRec->PixMap, 
							&pwriterRec->Rect, 
							pwriterRec->CodecQuality,
							pwriterRec->CodecType,
							pwriterRec->ImageDesc, 
							pwriterRec->ComprDataPtr);
	if (myErr != noErr)
		goto bail;
	
	// Add encoded buffer to movie:
	myErr = AddMediaSample(	pwriterRec->Media, 
							pwriterRec->ComprDataHdl,
							0,								// no offset in data
							(**(pwriterRec->ImageDesc)).dataSize, 
							frameDurationUnits,			// frame duration
							(SampleDescriptionHandle) pwriterRec->ImageDesc, 
							1,								// one sample
							0,								// self-contained samples
							NULL);
	if (myErr != noErr)
		goto bail;

bail:
		//	SetGWorld(pwriterRec->SavedPort, pwriterRec->SavedDevice);
		return(myErr);
}

	
int PsychCreateNewMovieFile(char* moviefile, int width, int height, double framerate, char* movieoptions)
{
	PsychMovieWriterRecordType* pwriterRec = NULL;
	int						moviehandle = 0;
	long					myFlags = createMovieFileDeleteCurFile | createMovieFileDontCreateResFile;
	OSErr					myErr = noErr;
	char*					poption;
	int						dummyInt;
	float					dummyFloat;
	
	// Still capacity left?
	if (moviewritercount >= PSYCH_MAX_MOVIEWRITERDEVICES) PsychErrorExitMsg(PsychError_user, "Maximum number of movie writers exceeded. Please close some first!");

	// Find first free (i.e., NULL) slot and assign moviehandle:
	while ((pwriterRec = PsychGetMovieWriter(moviehandle, TRUE)) && pwriterRec->Movie) moviehandle++;

	if (firsttime) {
#if PSYCH_SYSTEM == PSYCH_WINDOWS
        // Initialize Quicktime for Windows compatibility layer: This will fail if
        // QT isn't installed on the Windows machine...
        myErr = InitializeQTML(0);
        if (myErr!=noErr) {
            PsychErrorExitMsg(PsychError_internal, "Quicktime Media Layer initialization failed: Quicktime not properly installed?!?");
        }
#endif

        // Initialize Quicktime-Subsystem:
        myErr = EnterMovies();
        if (myErr!=noErr) {
            PsychErrorExitMsg(PsychError_internal, "Quicktime EnterMovies() failed!!!");
        }
		firsttime = FALSE;
	}

	// Translate char string with movie name to FSSpec:
	NativePathNameToFSSpec(moviefile, &pwriterRec->File, 0);

	pwriterRec->CodecType = kH264CodecType;
	pwriterRec->CodecQuality = codecNormalQuality; 
	pwriterRec->ResID = movieInDataForkResID;	
	pwriterRec->height = height;
	pwriterRec->width = width;
	pwriterRec->padding = (PSYCH_SYSTEM == PSYCH_OSX) ? 1 : 0;
	
	// Assign numeric 32-bit FOURCC equivalent code to select codec:
	// This is optional. We default to kH264CodecType:
	if ((poption = strstr(movieoptions, "CodecFOURCCId="))) {
		if (sscanf(poption, "CodecFOURCCId=%i", &dummyInt) == 1) {
			pwriterRec->CodecType = (CodecType) dummyInt;
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Codec with FOURCC numeric id %i selected for encoding of movie %i [%s].\n", dummyInt, moviehandle, moviefile);
		}
		else PsychErrorExitMsg(PsychError_user, "Invalid CodecFOURCCId= parameter provided in movieoptions parameter. Parse error!");
	}

	// Assign numeric encoding quality level:
	// This is optional. We default to "normal quality":
	if ((poption = strstr(movieoptions, "EncodingQuality="))) {
		if (sscanf(poption, "EncodingQuality=%f", &dummyFloat) == 1) {
			// Map floating point quality level between 0.0 and 1.0 to 10 discrete levels:
			dummyInt = (int)(10.0 * dummyFloat + 0.5);
			dummyInt = (dummyInt < 0)  ?  0 : dummyInt;
			dummyInt = (dummyInt > 10) ? 10 : dummyInt;
			
			// Assign one of Quicktime's 6 code quality setting to the 10 levels:
			switch(dummyInt) {
				case 0:
					pwriterRec->CodecQuality = codecMinQuality;
				break;

				case 1:
					pwriterRec->CodecQuality = codecMinQuality;
				break;

				case 2:
					pwriterRec->CodecQuality = codecLowQuality;
				break;

				case 3:
					pwriterRec->CodecQuality = codecLowQuality;
				break;

				case 4:
					pwriterRec->CodecQuality = codecNormalQuality;
				break;

				case 5:
					pwriterRec->CodecQuality = codecNormalQuality;
				break;

				case 6:
					pwriterRec->CodecQuality = codecNormalQuality;
				break;

				case 7:
					pwriterRec->CodecQuality = codecHighQuality;
				break;

				case 8:
					pwriterRec->CodecQuality = codecHighQuality;
				break;

				case 9:
					pwriterRec->CodecQuality = codecMaxQuality;
				break;

				case 10:
					pwriterRec->CodecQuality = codecLosslessQuality;
				break;
				
			}
			
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Encoding quality level %i selected for encoding of movie %i [%s].\n", dummyInt, moviehandle, moviefile);
		}
		else PsychErrorExitMsg(PsychError_user, "Invalid EncodingQuality= parameter provided in movieoptions parameter. Parse error!");
	}
	

	// Create a movie file for the destination movie:
	myErr = CreateMovieFile(&pwriterRec->File, FOUR_CHAR_CODE('TVOD'), smCurrentScript, myFlags, &pwriterRec->ResRefNum, &pwriterRec->Movie);
	if (myErr != noErr)
		goto bail;

	// Create the movie track and media:
	pwriterRec->Track = NewMovieTrack(pwriterRec->Movie, FixRatio(width, 1), FixRatio(height, 1), kNoVolume);
	myErr = GetMoviesError();
	if (myErr != noErr)
		goto bail;
		
	pwriterRec->Media = NewTrackMedia(pwriterRec->Track, VideoMediaType, (int) framerate, NULL, 0);
	myErr = GetMoviesError();
	if (myErr != noErr)
		goto bail;

	// Create the media samples:
	myErr = BeginMediaEdits(pwriterRec->Media);
	if (myErr != noErr)
		goto bail;

	// Create a GWorld as target with a pixedepth of 32 bpp:
	// Hack hack pwriterRec->padding!!
	MacSetRect(&pwriterRec->Rect, 0, 0, width - pwriterRec->padding, height - pwriterRec->padding);
	myErr = NewGWorld(&pwriterRec->GWorld, 32, &pwriterRec->Rect, NULL, NULL, (GWorldFlags)0);
	if (myErr != noErr)
		goto bail;

	pwriterRec->PixMap = GetGWorldPixMap(pwriterRec->GWorld);
	if (pwriterRec->PixMap == NULL)
		goto bail;

	LockPixels(pwriterRec->PixMap);
	myErr = GetMaxCompressionSize(	pwriterRec->PixMap,
									&pwriterRec->Rect, 
									0,							// let ICM choose depth
									pwriterRec->CodecQuality, 
									pwriterRec->CodecType, 
									(CompressorComponent) anyCodec,
									&pwriterRec->MaxComprSize);
	if (myErr != noErr)
		goto bail;

	pwriterRec->ComprDataHdl = NewHandle(pwriterRec->MaxComprSize);
	if (pwriterRec->ComprDataHdl == NULL)
		goto bail;

	HLockHi(pwriterRec->ComprDataHdl);
	pwriterRec->ComprDataPtr = *(pwriterRec->ComprDataHdl);

	pwriterRec->ImageDesc = (ImageDescriptionHandle)NewHandle(4);
	if (pwriterRec->ImageDesc == NULL)
		goto bail;

//	GetGWorld(&pwriterRec->SavedPort, &pwriterRec->SavedDevice);
	SetGWorld(pwriterRec->GWorld, NULL);
	
	// Increment count of open movie writers:
	moviewritercount++;
	
	// Return new handle:
	return(moviehandle);

bail:

	if (pwriterRec->ComprDataHdl != NULL)
		DisposeHandle(pwriterRec->ComprDataHdl);

	if (pwriterRec->GWorld != NULL)
		DisposeGWorld(pwriterRec->GWorld);
		
	if (pwriterRec->ResRefNum != 0)
		CloseMovieFile(pwriterRec->ResRefNum);

	if (pwriterRec->Movie != NULL)
		DisposeMovie(pwriterRec->Movie);

	pwriterRec->Movie = NULL;
	pwriterRec->Track = NULL;
	pwriterRec->Media = NULL;
	pwriterRec->ResRefNum = 0;
	pwriterRec->GWorld = NULL;
	pwriterRec->PixMap = NULL;
	pwriterRec->MaxComprSize = 0L;
	pwriterRec->ComprDataHdl = NULL;
	pwriterRec->ComprDataPtr = NULL;
	pwriterRec->ImageDesc = NULL;
	pwriterRec->SavedPort = NULL;
	pwriterRec->SavedDevice = NULL;

	// Return failure:
	return(-1);
}

int PsychFinalizeNewMovieFile(int movieHandle)
{
	OSErr					myErr = noErr;

	PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(movieHandle, FALSE);

	if (NULL == pwriterRec->Media) return(0);

	myErr = EndMediaEdits(pwriterRec->Media);
	if (myErr != noErr)
		goto bail;
	
	// add the media to the track
	myErr = InsertMediaIntoTrack(pwriterRec->Track, 0, 0, GetMediaDuration(pwriterRec->Media), fixed1);
	if (myErr != noErr)
		goto bail;
	
	// add the movie atom to the movie file
	myErr = AddMovieResource(pwriterRec->Movie, pwriterRec->ResRefNum, &pwriterRec->ResID, NULL);

bail:
	if (pwriterRec->ImageDesc != NULL)
		DisposeHandle((Handle)pwriterRec->ImageDesc);

	if (pwriterRec->ComprDataHdl != NULL)
		DisposeHandle(pwriterRec->ComprDataHdl);

	if (pwriterRec->GWorld != NULL)
		DisposeGWorld(pwriterRec->GWorld);

	if (pwriterRec->ResRefNum != 0)
		CloseMovieFile(pwriterRec->ResRefNum);

	if (pwriterRec->Movie != NULL)
		DisposeMovie(pwriterRec->Movie);

	pwriterRec->Movie = NULL;
	pwriterRec->Track = NULL;
	pwriterRec->Media = NULL;
	pwriterRec->ResRefNum = 0;
	pwriterRec->GWorld = NULL;
	pwriterRec->PixMap = NULL;
	pwriterRec->MaxComprSize = 0L;
	pwriterRec->ComprDataHdl = NULL;
	pwriterRec->ComprDataPtr = NULL;
	pwriterRec->ImageDesc = NULL;
	pwriterRec->SavedPort = NULL;
	pwriterRec->SavedDevice = NULL;
	
	// Decrement count of active writers:
	moviewritercount--;

	return(myErr == 0);
}

// End of routines.
	
#else

// This is Linux: Implement dummy functions:
void PsychMovieWritingInit(void) { return; }
void PsychExitMovieWriting(void) { return; }
void PsychDeleteAllMovieWriters(void) { return; }
unsigned char*	PsychGetVideoFrameForMoviePtr(int moviehandle, unsigned int* twidth, unsigned int* theight) { return(NULL); }
int PsychAddVideoFrameToMovie(int moviehandle, int frameDurationUnits, psych_bool isUpsideDown) { return(0); }
int PsychCreateNewMovieFile(char* moviefile, int width, int height, double framerate, char* movieoptions)
{
	PsychErrorExitMsg(PsychError_unimplemented, "Sorry, movie writing and editing support not yet implemented on this operating system.");
}

int PsychFinalizeNewMovieFile(int movieHandle)
{
	PsychErrorExitMsg(PsychError_unimplemented, "Sorry, movie writing and editing support not yet implemented on this operating system.");
}

#endif

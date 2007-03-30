/*
PsychToolbox2/Source/Common/WindowBank.h		

AUTHORS:

	Allen.Ingling@nyu.edu               awi
        mario.kleiner at tuebingen.mpg.de   mk

PLATFORMS: 

	OS X and Microsoft Windows. Only the PsychTargetSpecificWindowRecordType struct
	is different for the different operating systems.

HISTORY:
	
	07/18/02  awi		Wrote it.
        04/22/05  mk            Added new flags to PsychWindowRecordType for stereo mode, skipped frames, IFI estimate and timestamping
                                - These functions are mostly used by Screen('Flip') and Screen('DrawingFinished').
        07/22/05  mk            Removed constants for max number of windows. We resize dynamically now (see WindowBank.c)
        10/11/05  mk            Support for special Quicktime movie textures in .targetSpecific part of PsychWindowRecord added.
	12/27/05  mk            Added new targetSpecific - struct for the Win32 port of PTB.

DESCRIPTION:

	WindowBank contains functions for storing records of open  onscreen and offscreen windows.

	We don't use real window pointers when we return a window pointer to the scripting 
	environment.  Instead, we return an index which the PsychToolbox maps to a window
	pointer. Since the mapping is always from the index passed from the scripting environment,
	we can implement this with array lookup. 

	The window pointer or screen number is frequently passed as a single number, and by using
	our own indices we make things easier for ourselves when disambiguating the meaning of a 
	windowPointerOrScreenNumber value, because the value itself is a direct and unambiguouis 
	reference.  


T0 DO: 
	
	Change abbreviaioions to "windowIndex" from "windex" and to "screenNumber" from "scrumber"
	
	The WindowRecord will store either a texture (and OpenGL texture) or an offscreen window (GL context in system memory).
	Separate WindowRecord fields according to whether they retain info for i. offscree windows ii. textures iii. both and 
	make the mutually exclusive fields variant.  This is more for clarity than to save memory.    
  

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_WindowBank
#define PSYCH_IS_INCLUDED_WindowBank


/*
	includes
*/

#include "Screen.h"


//constants
#define PSYCH_MAX_SCREENS				10		//the total possible number of screens
#define PSYCH_LAST_SCREEN				9		//the highest possible screen index
#define PSYCH_FIRST_SCREEN				0		//the lowest possible screen index
#define PSYCH_ALLOC_SCREEN_RECORDS		        10		//length of static array allocated to hold screen pointers.
#define PSYCH_FIRST_WINDOW				10		//the lowest possible windox index
					

//constants
#define PSYCH_INVALID_WINDEX 				-1
#define PSYCH_INVALID_SCUMBER				-1

// Maximum number of different hook chains:
#define MAX_SCREEN_HOOKS 13

// Maximum number of slots in windowRecords fboTable:
#define MAX_FBOTABLE_SLOTS 2+3+3+2

// Type of hook function attached to a specific hook chain slot:
#define kPsychShaderFunc	0
#define kPsychCFunc			1
#define kPsychMFunc			2
#define kPsychBuiltinFunc	3

// Definition of flags for imagingMode of Image processing pipeline.
// These are used internally, but need to be exposed to Matlab as well.
#define kPsychNeedFastBackingStore 1		// Any FBO's needed at all?
#define kPsychNeedImageProcessing  2		// Any image processing needed at all?
#define kPsychNeedOutputConversion 4		// Output conversion blit needed?
#define kPsychNeedSeparateStreams  8		// Quad-buffered stereo needed?
#define kPsychNeedStereoMergeOp	   16		// Merging stereo -> single fb needed?
#define kPsychNeed32BPCFloat	   32		// float32 FBO's needed?
#define kPsychNeed16BPCFloat	   64		// float16 FBO's needed?
#define kPsychNeed16BPCFixed	   128		// fixed point 16 bpc FBO's needed?
#define kPsychNeedDualPass         256      // At least support for dual-pass processing needed?
#define kPsychNeedMultiPass        512      // Support for multi-pass processing needed?

// Definition of a single hook function spec:
typedef struct PsychHookFunction*	PtrPsychHookFunction;
typedef struct PsychHookFunction {
	PtrPsychHookFunction	next;
	char*					idString;
	int						hookfunctype;
	char*					pString1;
	void*					cprocfunc;
	unsigned int			shaderid;
	unsigned int			luttexid1;
} PsychHookFunction;

// Definition of an OpenGL Framebuffer object (FBO) for internal use.
typedef struct PsychFBO {
	GLuint					fboid;		// Handle to FBO.
	GLuint					coltexid;	// Texture handle for color buffer texture (color attachment zero).
	GLuint					ztexid;		// Texture handle for z-Buffer texture, if any. Zero otherwise.
	GLuint					stexid;		// Texture handle for stencil-Buffer texture, if any. Zero otherwise.
	int						width;		// Width of FBO.
	int						height;		// Height of FBO.
} PsychFBO;

// Typedefs for WindowRecord in WindowBank.h


#if PSYCH_SYSTEM == PSYCH_OSX
// Definition of OS-X core graphics and Core OpenGL handles:
typedef struct{
        CGLContextObj		contextObject;
        CGLPixelFormatObj	pixelFormatObject;
		CGLContextObj		glusercontextObject;   // OpenGL context for userspace rendering code, e.g., moglcore...
        CVOpenGLTextureRef QuickTimeGLTexture;     // Used for textures returned by movie routines in PsychMovieSupport.c
        void*              deviceContext;          // Dummy pointer, just here for compatibility to Windows-Port (simplifies code)
} PsychTargetSpecificWindowRecordType;
#endif 

#if PSYCH_SYSTEM == PSYCH_WINDOWS
// Definition of Win32 Window handles, device handles and OpenGL contexts
typedef struct{
  HGLRC		              contextObject;      // OpenGL rendering context.
  HDC                     deviceContext;      // Device context of the window.
  HWND                    windowHandle;       // The window handle.
  PIXELFORMATDESCRIPTOR   pixelFormatObject;  // The context's pixel format object.
  HGLRC					  glusercontextObject;	   // OpenGL context for userspace rendering code, e.g., moglcore...
  CVOpenGLTextureRef      QuickTimeGLTexture; // Used for textures returned by movie routines in PsychMovieSupport.c
  // CVOpenGLTextureRef is not ready yet. Its typedefd to a void* to make the compiler happy.
} PsychTargetSpecificWindowRecordType;
#endif 

#if PSYCH_SYSTEM == PSYCH_LINUX
// Definition of Linux/X11 specific information:
typedef struct{
  GLXContext		contextObject;       // GLX OpenGL rendering context.
  int             	pixelFormatObject;   // Just here for compatibility. Its a dummy entry without meaning.
  Display*              deviceContext;       // Pointer to the X11 display connection.
  Window                windowHandle;        // Handle to the onscreen window.
  GLXContext		glusercontextObject; // OpenGL context for userspace rendering code, e.g., moglcore...
  CVOpenGLTextureRef QuickTimeGLTexture;     // Used for textures returned by movie routines in PsychMovieSupport.c
  // CVOpenGLTextureRef is not ready yet. Its typedefd to a void* to make the compiler happy.
} PsychTargetSpecificWindowRecordType;
#endif 

#define kPsychUnaffiliatedWindow	-1		// valid value for screenNumber field of a window record meaning that that pixel format
											// and alignment of the window are not set to match those of any display surface.

typedef struct _PsychWindowRecordType_ *PsychWindowRecordPntrType;												

//typedefs for the window bank.  We use the same structure for both windows and textures.   
typedef struct _PsychWindowRecordType_{

	//need to be divided up according to use for textures, windows, or both.
	PsychWindowType                         windowType;
	int					screenNumber;   // kPsychUnaffiliated is -1 and means the offscreen window is unaffiliated.
	PsychWindowIndexType                    windowIndex;
	void					*surface; 
	int					surfaceSizeBytes;	//what is this ?  the size in bytes ? 
	PsychRectType                           rect;
	boolean					isValid;		//between when we allocate the record and when we fill in values.
	int					depth;
	int					nrchannels;
	int					redSize;		
	int					greenSize;
	int					blueSize;
	int					alphaSize;
        int                                     multiSample;            // Number of samples to use for anti-aliasing of each drawn pixel:
                                                                        // 0=Standard GL, n>0 : Use Multisampling, try to get at least n samples/pixel.
	//used only when this structure holds a window
	Boolean					actualEnableBlending;
	GLenum					actualSourceAlphaBlendingFactor;
	GLenum					actualDestinationAlphaBlendingFactor;
	GLenum					nextSourceAlphaBlendingFactor;
	GLenum					nextDestinationAlphaBlendingFactor;
	
	
	//Used only when this structure holds a texture:    
	GLuint					*textureMemory;
	size_t					textureMemorySizeBytes;
	GLuint					textureNumber;
        int                                     textureOrientation;     // Orientation of texture data in internal storage. Defines texcoord assingment.  
        int					texturecache_slot;      // Reference of cache structure for this texture, if any...
        GLenum                                  texturetarget;          // Explicit target type of texture (GL_TEXTURE_2D, ...)
        // The following three are only used for injecting special textures into PTB, e.g., High Dynamic range textures in floating point format.
        // They default to zero, which means: Derive texture representation from depth.
        GLint                                   textureinternalformat;  // Explicit definition of glinternalformat for texture creation.
        GLenum                                  textureexternalformat;  // Explicit definitin of external format for texture creation.
        GLenum                                  textureexternaltype;    // Explicit definition of data type for texture creation.
		GLuint				textureFilterShader;	// Optional GLSL program handle for a shader to apply during PsychBlitTextureToDisplay().

	//Used only when this structure holds a window:
	PsychTextAttributes                     textAttributes;
	
	//line stipple attributes, for windows not textures.
	GLushort				stipplePattern;
	GLint					stippleFactor;
	boolean					stippleEnabled;
        
        int                                     stereomode;             // MK: Is this a stereo window? 0=non-stereo, >0 == specific type of stero.
        int                                     stereodrawbuffer;       // MK: Which drawbuffer is active in stereo? 0=left, 1=right, 2=none
        bool                                    auxbuffer_dirty[2];     // MK: State of auxbuffers 0 and 1: Dirty or not? (For stereo algs.)
        int                                     nrIFISamples;           // MK: nrIFISamples and IFIRunningSum are used to calculate an
        double                                  IFIRunningSum;          // MK: accurate estimate of the real interframe interval (IFI) in Flip.
		double                                  time_at_last_vbl;       // MK: Timestamp (system-time) at last VBL detected by Flip.
        double                                  VideoRefreshInterval;   // MK: Estimated video refresh interval of display. Can be different to IFI.
        int                                     VBL_Endline;            // MK: Estimated scanline which marks end of VBL area.
        bool                                    PipelineFlushDone;      // MK: Will be set by SCREENDrawingFinished to signal pipeline flush.
        bool                                    backBufferBackupDone;   // MK: Will be set by SCREENDrawingFinished to signal backbuffer backup.
        int                                     nr_missed_deadlines;    // MK: Counter, incremented by Flip if it detects a missed/skipped frame.

	// Pointers to temporary arrays with gamma tables to upload to the gfx-card at next Screen('Flip'):
	// They default to NULL and get possibly set in Screen('LoadNormalizedGammaTable'):
	float* inRedTable;
	float* inGreenTable;
	float* inBlueTable;
	int    inTableSize;					// Number of slots in the LUT tables.
	int    loadGammaTableOnNextFlip;	// Type of upload operation: 0 = None, 1 = Load on next Flip via OS gamma table routines, then reset flag.
	
	// Settings for the image processing and hook callback pipeline: See PsychImagingPipelineSupport.hc for definition and implementation:
	double					colorRange;								// Maximum allowable color component value. See SCREENColorRange.c for explanation.
	int						imagingMode;							// Master mode switch for imaging and callback hook pipeline.
	PtrPsychHookFunction	HookChain[MAX_SCREEN_HOOKS];			// Array of pointers to the hook-chains for different hooks.
	Boolean					HookChainEnabled[MAX_SCREEN_HOOKS];		// Array of Booleans to en-/disable single chains temporarily.

	// Indices into our FBO table: The special value -1 means: Don't use.
	int						drawBufferFBO[2];						// Storage for drawing FBOs: These are the targets of all drawing operations before
																	// Screen('DrawingFinished') or Screen('Flip') is called. They are read-only wrt.
																	// to all following processing stages: 0=Left eye (or mono) channel, 1=Right eye channel.
	int						processedDrawBufferFBO[3];				// These contain the final output of all per-view channel postprocessing operations:
																	// 0=Left eye (or mono) channel, 1=Right eye channel, 2=Temporary bounce buffer for iterative
																	// multi-pass processing. These provide the input for the stereo merger in stereo modes that
																	// require merging of the two views, e.g., anaglyph stereo.
	int						preConversionFBO[3];					// preConversion FBO's: FBO zero/one are the targets for any stereo merge operations. FBO two is
																	// (optionally) a temporary bounce buffer for multipass post processing.
	int						finalizedFBO[2];						// This is the final framebuffer: Usually the system backbuffer, but could be something special.

	PsychFBO*				fboTable[MAX_FBOTABLE_SLOTS];			// This array contains pointers to the FBO structs which are referenced by the indices above.
	int						fboCount;								// This contains the number of FBO's in fboTable.
	
	//Used only when this structure holds a window:
	//platform specific stuff goes within the targetSpecific structure.  Defined in PsychVideoGlue and accessors are in PsychWindowGlue.c
	//Only use abstracted accessors on this structure, otherwise you will break platform portability.
	PsychTargetSpecificWindowRecordType	targetSpecific;  //defined within 
	
} PsychWindowRecordType;



//various window and screen related
void			PsychFindScreenWindowFromScreenNumber(int screenNumber, PsychWindowRecordType **winRec);
PsychError 		InitWindowBank(void);
PsychError 		CloseWindowBank(void);
void 			PsychCreateWindowRecord(PsychWindowRecordType **winRec);
PsychError 		FreeWindowRecordFromIndex(PsychWindowIndexType windowIndex);
PsychError 		FreeWindowRecordFromPntr(PsychWindowRecordType *winRec);
int 			PsychCountOpenWindows(PsychWindowType winType);
boolean 		IsValidScreenNumber(PsychNumdexType numdex);
boolean			IsValidScreenNumberOrUnaffiliated(PsychNumdexType numdex);
boolean 		IsWindowIndex(PsychNumdexType numdex);
boolean			IsScreenNumberUnaffiliated(PsychNumdexType numdex);
PsychError 		FindWindowRecord(PsychWindowIndexType windowIndex, PsychWindowRecordType **windowRecord);
PsychError 		FindScreenRecord(int screenNumber, PsychScreenRecordType **screenRecord);
boolean 		PsychIsLastOnscreenWindow(PsychWindowRecordType *windowRecord);
void			PsychCreateVolatileWindowRecordPointerList(int *numWindows, PsychWindowRecordType ***pointerList);
void 			PsychDestroyVolatileWindowRecordPointerList(PsychWindowRecordType **pointerList);


//end include once
#endif






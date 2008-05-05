/*
	SCREENMakeTexture.c	

	AUTHORS:
		
		Allen Ingling		awi				Allen.Ingling@nyu.edu
		Mario Kleiner		mk              mario.kleiner@tuebingen.mpg.de

	PLATFORMS:
	
		Only OS X for now.

	HISTORY:
		
		6/25/04		awi		Created.
		1/4/05      mk      Performance optimizations, some bug-fixes.
		1/13/05		awi		Merged in Marios's changes from 1/4/05 into the master at Psychtoolbox.org.
		1/19/05		awi		Removed unused variables to eliminate compiler warnings.
		1/26/05		awi		Added StoreNowTime() calls.  

	DESCRIPTION:

		Conversion code for creating OpenGL textures from Matlab/Octave image matrices. Handles uint8 and
		double matrices as input. Converts into 8bpc textures by default, but also supports half_float and
		float 16 bpc, 32 bpc floating point textures on modern hardware.
		
	TO DO:

		set the offscreen window recored valid flag here.  
		(And change the allocator so that it inits the textureMemory flag to null.)
		modify close so that it dealloctes the texture memory
		modify close documentation to mention that it also works for textures


*/


#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "textureIndex=Screen('MakeTexture', WindowIndex, imageMatrix [, optimizeForDrawAngle=0] [, specialFlags=0] [, floatprecision=0] [, textureOrientation=0] [, textureShader=0]);";
//                                                            1            2              3                          4                5                    6						7
static char synopsisString[] = 
	"Convert the 2D or 3D matrix 'imageMatrix' into an OpenGL texture and return an index which may be passed to 'DrawTexture' to specify the texture. "
	"In the the OS X Psychtoolbox textures replace offscreen windows for fast drawing of images during animation."
	"The imageMatrix argument may consist of one monochrome plane (Luminance), LA planes, RGB planes, or RGBA planes where "
	"A is alpha, the transparency of a pixel. Alpha values range between zero (=fully transparent) and 255 (=fully opaque). "
	"You need to enable Alpha-Blending via Screen('BlendFunction',...) for the transparency values to have an effect. "
	"The argument 'optimizeForDrawAngle' if provided, asks Psychtoolbox to optimize the texture for especially fast "
	"drawing at the specified rotation angle. The default is 0 == Optimize for upright drawing. If 'specialFlags' is set "
	"to 1 and the width and height of the imageMatrix are powers of two (e.g., 64 x 64, 256 x 256, 512 x 512, ...), then "
	"the texture is created as an OpenGL power-of-two texture of type GL_TEXTURE_2D. Otherwise Psychtoolbox will try to "
	"pick the most optimal format for fast drawing and low memory consumption. Power-of-two textures are especially useful "
	"for animation of drifting gratings (see the demos) and for simple use with the OpenGL 3D graphics functions. "
	"If 'specialFlags' is set to 2 then PTB will try to use its own high quality texture filtering algorithm for drawing "
	"of bilinearly filtered textures instead of the hardwares built-in method. This only works on modern hardware with "
	"fragment shader support and is slower than using the hardwares built in filtering, but it may provide higher precision "
	"on some hardware. Please note that PTB automatically enables its own filter algorithm when used with floating point "
	"textures or when Screen('ColorRange') is used to enable unclamped color processing: PTB will check if your hardware "
	"is capable of unrestricted high precision color processing in that case. If your hardware can't guarantee high precision, "
	"PTB will enable its own shader-based workarounds to provide higher precision at the cost of lower speed. \n"
	"'floatprecision' defines the precision with which the texture should be stored and processed. Default value is zero, "
	"which asks to store textures with 8 bit per color component precision, a suitable format for standard images read via "
	"imread(). A non-zero value will store the textures color component values as floating point precision numbers, useful "
	"for complex blending operations and calculations on the textures and for processing and display of high dynamic range "
	"image textures, either on a LDR display via tone-mapping, or on a HDR display. If floatprecision is set to 1, the "
	"texture gets stored in half_float format, i.e. 16 bit per color component - Suitable for most display purposes and "
	"fast on recent gfx-hardware. A value of 2 asks for full 32 bit single precision float per color component. Useful for complex "
	"computations and image processing, but can be extremely slow when texture filtering is used on any piece of graphics hardware "
	"manufactured before the year 2007. 'textureOrientation' This optional argument labels textures with a special orientation. "
	"Normally (value 0) a Matlab matrix is passed in standard Matlab column-major dataformat. This is efficient for drawing of "
	"textures but not for processing them via Screen('TransformTexture'). Therefore textures need to be transformed on demand "
	"if used that way. This flag allows to short-cut the process: A setting of 1 will ask for immediate conversion into the "
	"optimized format. A setting of 2 will tell PTB that the Matlab matrix has been already converted into optimal format, "
	"so no further processing is needed. A value of 3 tells PTB that the texture is completely isotropic, with no real orientation,"
	"therefore no conversion is required. This latter setting only makes sense for random noise textures or other textures generated "
	"from a distribution with uncorrelated noise-like pixels, e.g., some power spectrum distribution.\n"
	"'textureShader' - optional: If you provide the handle of an OpenGL GLSL shader program, then this shader program will be "
	"executed (bound) during drawing of this texture via the Screen('DrawTexture',...); command -- The normal texture drawing "
	"operation is replaced by your customized algorithm. This is useful for two purposes: a) Very basic on-the-fly image processing "
	"on the texture. b) Procedural shading: Your texture matrix doesn't encode an image, but only per-pixel parameters is input "
	"for some formula to compute the real image during drawing. E.g., instead of defining a gabor patch as image or other standard "
	"stimulus, one could define it as a mathematical formula to be evaluated at draw-time. The Screen('SetOpenGLTexture') command "
	"allows you to create purely virtual textures, that only consist of such a shader and some virtual size, but don't have any "
	"real data matrix associated with it -- all content is generated on the fly.";

static char seeAlsoString[] = "DrawTexture TransformTexture BlendFunction";
	 
PsychError SCREENMakeTexture(void) 
{
    int					ix;
    PsychWindowRecordType		*textureRecord;
    PsychWindowRecordType		*windowRecord;
    PsychRectType			rect;
    Boolean				isImageMatrixBytes, isImageMatrixDoubles;
    int					numMatrixPlanes, xSize, ySize, iters; 
    unsigned char			*byteMatrix;
    double				*doubleMatrix;
    GLuint                              *texturePointer;
    GLubyte                             *texturePointer_b;
	GLfloat								*texturePointer_f;
    double *rp, *gp, *bp, *ap;    
    GLubyte *rpb, *gpb, *bpb, *apb;    
    int                                 usepoweroftwo, usefloatformat, assume_texorientation, textureShader;
    double                              optimized_orientation;
    Boolean                             bigendian;

    // Detect endianity (byte-order) of machine:
    ix=255;
    rpb=(GLubyte*) &ix;
    bigendian = ( *rpb == 255 ) ? FALSE : TRUE;
    ix = 0; rpb = NULL;

    if(PsychPrefStateGet_DebugMakeTexture())	//MARK #1
        StoreNowTime();
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //Get the window structure for the onscreen window.  It holds the onscreein GL context which we will need in the
    //final step when we copy the texture from system RAM onto the screen.
    PsychErrorExit(PsychCapNumInputArgs(7));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	
    PsychErrorExit(PsychCapNumOutputArgs(1)); 
    
    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    if((windowRecord->windowType!=kPsychDoubleBufferOnscreen) && (windowRecord->windowType!=kPsychSingleBufferOnscreen))
        PsychErrorExitMsg(PsychError_user, "MakeTexture called on something else than a onscreen window");
    
	// Get optional texture orientation flag:
	assume_texorientation = 0;
	PsychCopyInIntegerArg(6, FALSE, &assume_texorientation);
	
	// Get optional texture shader handle:
	textureShader = 0;
	PsychCopyInIntegerArg(7, FALSE, &textureShader);
	
    //get the argument and sanity check it.
    isImageMatrixBytes=PsychAllocInUnsignedByteMatArg(2, kPsychArgAnything, &ySize, &xSize, &numMatrixPlanes, &byteMatrix);
    isImageMatrixDoubles=PsychAllocInDoubleMatArg(2, kPsychArgAnything, &ySize, &xSize, &numMatrixPlanes, &doubleMatrix);
    if(numMatrixPlanes > 4)
        PsychErrorExitMsg(PsychError_inputMatrixIllegalDimensionSize, "Specified image matrix exceeds maximum depth of 4 layers");
    if(ySize<1 || xSize <1)
        PsychErrorExitMsg(PsychError_inputMatrixIllegalDimensionSize, "Specified image matrix must be at least 1 x 1 pixels in size");
    if(! (isImageMatrixBytes || isImageMatrixDoubles))
        PsychErrorExitMsg(PsychError_user, "Illegal argument type");  //not  likely. 

	// Is this a special image matrix which is already pre-transposed to fit our optimal format?
	if (assume_texorientation == 2) {
		// Yes. Swap xSize and ySize to take this into account:
		ix = xSize;
		xSize = ySize;
		ySize = ix;
		ix = 0;
	}

	// Build defining rect for this texture:
    PsychMakeRect(rect, 0, 0, xSize, ySize);

    // Copy in texture preferred draw orientation hint. We default to zero degrees, if
    // not provided.
    // This parameter is not yet used. It is silently ignorerd for now...
    optimized_orientation = 0;
    PsychCopyInDoubleArg(3, FALSE, &optimized_orientation);
    
    // Copy in special creation mode flag: It defaults to zero. If set to 1 then we
    // always create a power-of-two GL_TEXTURE_2D texture. This is useful if one wants
    // to create and use drifting gratings with no effort - texture wrapping is only available
    // for GL_TEXTURE_2D, not for non-pot types. It is also useful if the texture is to be
    // exported to external OpenGL code to simplify tex coords assignments.
    usepoweroftwo=0;
    PsychCopyInIntegerArg(4, FALSE, &usepoweroftwo);

    // Check if size constraints are fullfilled for power-of-two mode:
    if (usepoweroftwo & 1) {
      for(ix = 1; ix < xSize; ix*=2);
      if (ix!=xSize) {
	PsychErrorExitMsg(PsychError_inputMatrixIllegalDimensionSize, "Power-of-two texture requested but width of imageMatrix is not a power of two!");
      }

      for(ix = 1; ix < ySize; ix*=2);
      if (ix!=ySize) {
	PsychErrorExitMsg(PsychError_inputMatrixIllegalDimensionSize, "Power-of-two texture requested but height of imageMatrix is not a power of two!");
      }
    }

	// Check if creation of a floating point texture is requested? We default to non-floating point,
	// standard 8 bpc textures if this parameter is not provided.
	usefloatformat = 0;
    PsychCopyInIntegerArg(5, FALSE, &usefloatformat);
	if (usefloatformat<0 || usefloatformat>2) PsychErrorExitMsg(PsychError_user, "Invalid value for 'floatprecision' parameter provided! Valid values are 0 for 8bpc int, 1 for 16bpc float or 2 for 32bpc float.");
	if (usefloatformat && !isImageMatrixDoubles) {
		// Floating point texture requested. We only support this if our input is a double matrix, not
		// for uint8 matrices - converting them to float precision would be just a waste of ressources
		// without any benefit for precision.
		PsychErrorExitMsg(PsychError_user, "Creation of a floating point precision texture requested, but uint8 matrix provided! Only double matrices are acceptable for this mode.");
	}

    //Create a texture record.  Really just a window record adapted for textures.  
    PsychCreateWindowRecord(&textureRecord);						//this also fills the window index field.
    textureRecord->windowType=kPsychTexture;
    // MK: We need to assign the screen number of the onscreen-window, so PsychCreateTexture()
    // can query the size of the screen/onscreen-window...
    textureRecord->screenNumber=windowRecord->screenNumber;
    textureRecord->depth=32;
    PsychCopyRect(textureRecord->rect, rect);
    
    //Allocate the texture memory and copy the MATLAB matrix into the texture memory.
    // MK: We only allocate the amount really needed for given format, aka numMatrixPlanes - Bytes per pixel.
	if (usefloatformat) {
		// Allocate a double for each color component and pixel:
		textureRecord->textureMemorySizeBytes= sizeof(double) * numMatrixPlanes * xSize * ySize;		
	}
    else {
		// Allocate one byte per color component and pixel:
		textureRecord->textureMemorySizeBytes= numMatrixPlanes * xSize * ySize;
    }
	// MK: Allocate memory page-aligned... -> Helps Apple texture range extensions et al.
    if(PsychPrefStateGet_DebugMakeTexture()) 	//MARK #2
        StoreNowTime();
    textureRecord->textureMemory=malloc(textureRecord->textureMemorySizeBytes);
    if(PsychPrefStateGet_DebugMakeTexture()) 	//MARK #3
        StoreNowTime();	
    texturePointer=textureRecord->textureMemory;
    
    // Does script explicitely request usage of a GL_TEXTURE_2D power-of-two texture?
    if (usepoweroftwo & 1) {
      // Enforce creation as a power-of-two texture:
      textureRecord->texturetarget=GL_TEXTURE_2D;
    }

	// Now the conversion routines that convert Matlab/Octave matrices into memory
	// buffers suitable for OpenGL:
	if (usefloatformat) {
		// Conversion routines for HDR 16 bpc or 32 bpc textures -- Slow path.
		// Our input is always double matrices...
		iters = xSize * ySize;

		// Our input buffer is always of GL_FLOAT precision:
		textureRecord->textureexternaltype = GL_FLOAT;
		texturePointer_f=(GLfloat*) texturePointer;
		
		if(numMatrixPlanes==1) {
			for(ix=0;ix<iters;ix++){
				*(texturePointer_f++)= (GLfloat) *(doubleMatrix++);  
			}
			textureRecord->depth=(usefloatformat==1) ? 16 : 32;

			textureRecord->textureinternalformat = (usefloatformat==1) ? GL_LUMINANCE_FLOAT16_APPLE : GL_LUMINANCE_FLOAT32_APPLE; 
			textureRecord->textureexternalformat = GL_LUMINANCE;
		}

		if(numMatrixPlanes==2) {
			rp=(double*) ((psych_uint64) doubleMatrix);
			ap=(double*) ((psych_uint64) doubleMatrix + (psych_uint64) iters*sizeof(double));

			for(ix=0;ix<iters;ix++){
				*(texturePointer_f++)= (GLfloat) *(rp++);  
				*(texturePointer_f++)= (GLfloat) *(ap++);  
			}
			textureRecord->depth=(usefloatformat==1) ? 32 : 64;
			textureRecord->textureinternalformat = (usefloatformat==1) ? GL_LUMINANCE_ALPHA_FLOAT16_APPLE : GL_LUMINANCE_ALPHA_FLOAT32_APPLE; 
			textureRecord->textureexternalformat = GL_LUMINANCE_ALPHA;
		}
		
		if(numMatrixPlanes==3) {
			rp=(double*) ((psych_uint64) doubleMatrix);
			gp=(double*) ((psych_uint64) doubleMatrix + (psych_uint64) iters*sizeof(double));
			bp=(double*) ((psych_uint64) gp + (psych_uint64) iters*sizeof(double));

			for(ix=0;ix<iters;ix++){
				*(texturePointer_f++)= (GLfloat) *(rp++);  
				*(texturePointer_f++)= (GLfloat) *(gp++);  
				*(texturePointer_f++)= (GLfloat) *(bp++);  
			}
			textureRecord->depth=(usefloatformat==1) ? 48 : 96;
			textureRecord->textureinternalformat = (usefloatformat==1) ? GL_RGB_FLOAT16_APPLE : GL_RGB_FLOAT32_APPLE; 
			textureRecord->textureexternalformat = GL_RGB;
		}
		
		if(numMatrixPlanes==4) {
			rp=(double*) ((psych_uint64) doubleMatrix);
			gp=(double*) ((psych_uint64) doubleMatrix + (psych_uint64) iters*sizeof(double));
			bp=(double*) ((psych_uint64) gp + (psych_uint64) iters*sizeof(double));
			ap=(double*) ((psych_uint64) bp + (psych_uint64) iters*sizeof(double));

			for(ix=0;ix<iters;ix++){
				*(texturePointer_f++)= (GLfloat) *(rp++);  
				*(texturePointer_f++)= (GLfloat) *(gp++);  
				*(texturePointer_f++)= (GLfloat) *(bp++);  
				*(texturePointer_f++)= (GLfloat) *(ap++);  
			}			
			textureRecord->depth=(usefloatformat==1) ? 64 : 128;
			textureRecord->textureinternalformat = (usefloatformat==1) ? GL_RGBA_FLOAT16_APPLE : GL_RGBA_FLOAT32_APPLE; 
			textureRecord->textureexternalformat = GL_RGBA;
		}

		// End of HDR conversion code...
	}
    else {
		// Standard LDR texture 8 bpc conversion routines -- Fast path.
	
		// Improved implementation: Takes 13 ms on a 800x800 texture...
		if(isImageMatrixDoubles && numMatrixPlanes==1){
			texturePointer_b=(GLubyte*) texturePointer;
			iters=xSize*ySize;
			for(ix=0;ix<iters;ix++){
				*(texturePointer_b++)= (GLubyte) *(doubleMatrix++);  
			}
			textureRecord->depth=8;
		}
		
		// Improved version: Takes 3 ms on a 800x800 texture...
		// NB: Implementing memcpy manually by a for-loop takes 10 ms! This is a huge difference.
		// -> That's because memcpy on MacOS-X is implemented with hand-coded, highly tuned Assembler code for PowerPC.
		// -> It's always wise to use system-routines if available, instead of coding it by yourself!
		if(isImageMatrixBytes && numMatrixPlanes==1){
			memcpy((void*) texturePointer, (void*) byteMatrix, xSize*ySize);
			textureRecord->depth=8;
		}
		
		// New version: Takes 33 ms on a 800x800 texture...
		if(isImageMatrixDoubles && numMatrixPlanes==2){
			texturePointer_b=(GLubyte*) texturePointer;
			iters=xSize*ySize;
			rp=(double*) ((psych_uint64) doubleMatrix);
			ap=(double*) ((psych_uint64) doubleMatrix + (psych_uint64) iters*sizeof(double));
			for(ix=0;ix<iters;ix++){
				*(texturePointer_b++)= (GLubyte) *(rp++);  
				*(texturePointer_b++)= (GLubyte) *(ap++);  
			}
			textureRecord->depth=16;
		}
		
		// New version: Takes 20 ms on a 800x800 texture...
		if(isImageMatrixBytes && numMatrixPlanes==2){
			texturePointer_b=(GLubyte*) texturePointer;
			iters=xSize*ySize;
			rpb=(GLubyte*) ((psych_uint64) byteMatrix);
			apb=(GLubyte*) ((psych_uint64) byteMatrix + (psych_uint64) iters);
			for(ix=0;ix<iters;ix++){
				*(texturePointer_b++)= *(rpb++);  
				*(texturePointer_b++)= *(apb++);  
			}
			textureRecord->depth=16;
		}
		
		// Improved version: Takes 43 ms on a 800x800 texture...
		if(isImageMatrixDoubles && numMatrixPlanes==3){
			texturePointer_b=(GLubyte*) texturePointer;
			iters=xSize*ySize;
			rp=(double*) ((psych_uint64) doubleMatrix);
			gp=(double*) ((psych_uint64) doubleMatrix + (psych_uint64) iters*sizeof(double));
			bp=(double*) ((psych_uint64) gp + (psych_uint64) iters*sizeof(double));
			for(ix=0;ix<iters;ix++){
				*(texturePointer_b++)= (GLubyte) *(rp++);  
				*(texturePointer_b++)= (GLubyte) *(gp++);  
				*(texturePointer_b++)= (GLubyte) *(bp++);  
			}
			textureRecord->depth=24;
		}
		
		// Improved version: Takes 25 ms on a 800x800 texture...
		if(isImageMatrixBytes && numMatrixPlanes==3){
			texturePointer_b=(GLubyte*) texturePointer;
			iters=xSize*ySize;
			
			rpb=(GLubyte*) ((psych_uint64) byteMatrix);
			gpb=(GLubyte*) ((psych_uint64) byteMatrix + (psych_uint64) iters);
			bpb=(GLubyte*) ((psych_uint64) gpb + (psych_uint64) iters);
			for(ix=0;ix<iters;ix++){
				*(texturePointer_b++)= *(rpb++);  
				*(texturePointer_b++)= *(gpb++);  
				*(texturePointer_b++)= *(bpb++);  
			}
			textureRecord->depth=24;
		}
		
		// Improved version: Takes 55 ms on a 800x800 texture...
		if(isImageMatrixDoubles && numMatrixPlanes==4){
			texturePointer_b=(GLubyte*) texturePointer;
			iters=xSize*ySize;
			
			rp=(double*) ((psych_uint64) doubleMatrix);
			gp=(double*) ((psych_uint64) doubleMatrix + (psych_uint64) iters*sizeof(double));
			bp=(double*) ((psych_uint64) gp + (psych_uint64) iters*sizeof(double));
			ap=(double*) ((psych_uint64) bp + (psych_uint64) iters*sizeof(double));
			if (bigendian) {
				// Code for big-endian machines like PowerPC:
				for(ix=0;ix<iters;ix++){
					*(texturePointer_b++)= (GLubyte) *(ap++);  
					*(texturePointer_b++)= (GLubyte) *(rp++);  
					*(texturePointer_b++)= (GLubyte) *(gp++);  
					*(texturePointer_b++)= (GLubyte) *(bp++);  
				}
			}
			else {
				// Code for little-endian machines like Intel Pentium:
				for(ix=0;ix<iters;ix++){
					*(texturePointer_b++)= (GLubyte) *(bp++);  
					*(texturePointer_b++)= (GLubyte) *(gp++);  
					*(texturePointer_b++)= (GLubyte) *(rp++);  
					*(texturePointer_b++)= (GLubyte) *(ap++);  
				}
			}
			
			textureRecord->depth=32;
		}
		
		// Improved version: Takes 33 ms on a 800x800 texture...
		if(isImageMatrixBytes && numMatrixPlanes==4){
			texturePointer_b=(GLubyte*) texturePointer;
			iters=xSize*ySize;
			
			rpb=(GLubyte*) ((psych_uint64) byteMatrix);
			gpb=(GLubyte*) ((psych_uint64) byteMatrix + (psych_uint64) iters);
			bpb=(GLubyte*) ((psych_uint64) gpb + (psych_uint64) iters);
			apb=(GLubyte*) ((psych_uint64) bpb + (psych_uint64) iters);
			if (bigendian) {
				// Code for big-endian machines like PowerPC:
				for(ix=0;ix<iters;ix++){
					*(texturePointer_b++)= *(apb++);  
					*(texturePointer_b++)= *(rpb++);  
					*(texturePointer_b++)= *(gpb++);  
					*(texturePointer_b++)= *(bpb++);  
				}
			}
			else {
				// Code for little-endian machines like Intel Pentium:
				for(ix=0;ix<iters;ix++){
					*(texturePointer_b++)= *(bpb++);  
					*(texturePointer_b++)= *(gpb++);  
					*(texturePointer_b++)= *(rpb++);  
					*(texturePointer_b++)= *(apb++);  
				}
			}
			
			textureRecord->depth=32;
		}
	} // End of 8 bpc texture conversion code (fast-path for LDR textures)
    
    // The memory buffer now contains our texture data in a format ready to submit to OpenGL.
    
	// Assign parent window and copy its inheritable properties:
	PsychAssignParentWindow(textureRecord, windowRecord);

    // Texture orientation is zero aka transposed aka non-renderswapped.
    textureRecord->textureOrientation = ((assume_texorientation != 2) && (assume_texorientation != 3)) ? 0 : 2;
    
	// This is our best guess about the number of image channels:
	textureRecord->nrchannels = numMatrixPlanes;

    // Let's create and bind a new texture object and fill it with our new texture data.
    PsychCreateTexture(textureRecord);
    
	// Assign GLSL filter-/lookup-shaders if needed:
	PsychAssignHighPrecisionTextureShaders(textureRecord, windowRecord, usefloatformat, (usepoweroftwo & 2) ? 1 : 0);

	// User specified override shader for this texture provided? This is useful for
	// basic image processing and procedural texture shading:
	if (textureShader!=0) {
		// Assign provided shader as filtershader to this texture: We negate it so
		// that the texture blitter routines know this is a custom shader, not our
		// built in filter shader:
		textureRecord->textureFilterShader = -1 * textureShader;
	}
	
    // Texture ready. Mark it valid and return handle to userspace:
    PsychSetWindowRecordValid(textureRecord);
    PsychCopyOutDoubleArg(1, FALSE, textureRecord->windowIndex);
	
	// Swapping the texture to upright orientation requested?
	if (assume_texorientation == 1) {
		// Transform sourceRecord source texture into a normalized, upright texture if it isn't already in
		// that format. We require this standard orientation for simplified shader design.

		PsychSetShader(windowRecord, 0);
		PsychNormalizeTextureOrientation(textureRecord);
	}
	
	// Shall the texture be finally declared "normally oriented"?
	// This is either due to explicit renderswapping if assume_textureorientation == 1,
	// or because it was already pretransposed in Matlab/Octave if assume_textureorientation == 2,
	// or because user space tells us the texture is isotropic if assume_textureorientation == 3.
	if (assume_texorientation > 0) {
		// Yes. Label it as such:
		textureRecord->textureOrientation = 2;
	}
    
    if(PsychPrefStateGet_DebugMakeTexture()) 	//MARK #4
        StoreNowTime();
    
    return(PsychError_none);
}

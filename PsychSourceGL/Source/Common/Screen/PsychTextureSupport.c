/*
	PsychTextureSupport.c
	
	PLATFORMS:	
	
		This is the OS independent version.  
				
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu
		Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de

	HISTORY:
	
		3/9/04		awi		Wrote it 
		7/27/04		awi		Removed commented-out code.
		1/4/05		mk		Performance optimizations, some bug-fixes.
		1/13/05		awi		Merged in Mario's changes of 1/4/05 into the psychtoolbox.org master.
		1/23/05		awi		Merged mk's update which adds glEnable(GL_TEXTURE_RECTANGLE_EXT) call.  Fixes DrawText and DrawTexture
								interference reported by Frans Cornelissen and others;
		1/30/05     mk			Small bugfix: Now specifying sourceRect in DrawTexture really works as expected.
		5/13/05		mk		Support for rotated drawing of textures. Switched to bilinear texture filtering instead of nearest neighbour.
		6/07/05		mk		Change definition of texture coords from GLint to GLflot -> Allows subpixel accurate texturing.
		7/23/05		mk		New options filterMode and globalAlpha for PsychBlitTextureToDisplay().
		8/11/05		mk		New texture handling: We now use real OpenGL textures instead of our old "pseudo-texture"
								implementation. This is *way* faster, e.g., drawing times decrease from 35 ms to 3 ms for big
								textures. Some experimental optimizations are implemented but not yet enabled...
		10/11/05	mk		Support for special Quicktime movie textures added.
		01/02/05	mk		Moved from OSX folder to Common folder. Contains nearly only shared code.
		3/07/06		awi		Print warnings conditionally according to PsychPrefStateGet_SuppressAllWarnings(). 
	
	DESCRIPTION:
	
		Psychtoolbox functions for dealing with textures.
	
	NOTES:
	

		
*/




#include "Screen.h"

// renderswap trades time in MakeTexture vs. time in DrawTexture. Setting it to true will
// *significantly* increase the duration of each MakeTexture call, but depending on the
// exact type of gfx-card and the orientation in which textures are drawn, it can also
// significantly speed up drawing (2-3 times!). In the future, we'll implement a clever scheme
// so that PTB can decide by itself on a case-by-case bases, if renderswap true or false is
// the better choice. For the 1.0.6 release we'll just keep it fixed to "false"...
static Boolean renderswap = FALSE;

// If set to true, then the apple client storage extensions are used: I doubt that they have any
// advantage for the current way PTB is used, but it can be useful to conserve VRAM on very
// low-mem gfx cards if Screen('Preference', 'ConserveVRAM') is set appropriately.
static Boolean clientstorage = FALSE;

// This stores the texture format/mode to use: We autodetect available types at first
// invocation of PsychCreateTexture()... We try to use GL_EXT_TEXTURE_RECTANGLE_2D textures for
// higher speed/efficiency and lower memory consumption. If that fails, we try
// vendor specifics like GL_NV_TEXTURE_RECTANGLE, if everything fails, we resort to
// GL_TEXTURE_2D... This switch defines the global mode for the texture mapping engine...
static GLenum  texturetarget = 0;

void PsychDetectTextureTarget(PsychWindowRecordType *win)
{
    // First time invocation?
    if (texturetarget==0) {
        // Yes. Need to auto-detect texturetarget to use...
		PsychSetGLContext(win);

        if (strstr(glGetString(GL_EXTENSIONS), "GL_EXT_texture_rectangle") && GL_TEXTURE_RECTANGLE_EXT != GL_TEXTURE_2D) {
	    // Great! GL_TEXTURE_RECTANGLE_EXT is available! Use it.
	    texturetarget = GL_TEXTURE_RECTANGLE_EXT;
	    if(PsychPrefStateGet_Verbosity()>2)
			printf("PTB-INFO: Using OpenGL GL_TEXTURE_RECTANGLE_EXT extension for efficient high-performance texture mapping...\n");
        }
        else if (strstr(glGetString(GL_EXTENSIONS), "GL_NV_texture_rectangle") && GL_TEXTURE_RECTANGLE_NV != GL_TEXTURE_2D){
	    // Try NVidia specific texture rectangle extension:
	    texturetarget = GL_TEXTURE_RECTANGLE_NV;
	    if(PsychPrefStateGet_Verbosity()>2)
			printf("PTB-INFO: Using NVidia's GL_TEXTURE_RECTANGLE_NV extension for efficient high-performance texture mapping...\n");
        }
        else {
	    // No texture rectangle extension available :(
	    // We fall back to standard power of two textures...
		if(PsychPrefStateGet_Verbosity()>1){
			printf("\nPTB-WARNING: Your graphics hardware & driver doesn't support OpenGL rectangle textures.\n");
			printf("PTB-WARNING: This won't affect the correctness or visual accuracy of image drawing, but it can significantly\n");
			printf("PTB-WARNING: degrade performance/speed and increase memory consumption of images by up to a factor of 4!\n");
			printf("PTB-WARNING: If you use a lot of image stimuli (DrawTexture, Offscreen windows, Stereo display, Quicktime movies)\n");
			printf("PTB-WARNING: and you are unhappy with the performance, then please upgrade your graphics driver and possibly\n");
			printf("PTB-WARNING: your gfx hardware if you need higher performance...\n");
		}
	    //printf("%s", glGetString(GL_EXTENSIONS));
	    fflush(NULL);
	    texturetarget = GL_TEXTURE_2D;
	  }
    }
    return;
}

void PsychInitWindowRecordTextureFields(PsychWindowRecordType *win)
{
	win->textureMemory=NULL;
	win->textureNumber=0;
	win->textureMemorySizeBytes=0;
        // NULL-Out special texture handle for Quicktime Movie textures (see PsychMovieSupport.c)
        win->targetSpecific.QuickTimeGLTexture = NULL;
        // Setup initial texture orientation: 0 = Transposed texture == Format of Matlab image matrices.
        // This number defines how the height and width of a texture need to be interpreted and how
        // texture coordinates are assigned in PsychBlitTextureToDisplay().
        win->textureOrientation=0;
	// Set to default minus 1-value (== disabled): Meaning of this field is specific for source of texture. It should
	// somehow identify the cache data structure for textures of specifif origin in a unique way.
	// If this is a cached texture for use by the PsychMovieSupport Quicktime subsystem and we
	// use GWorld rendering, then this points to the movieRecord of the movie which is associated
	// with this texture...
	win->texturecache_slot=-1;
        // Explicit storage of the type of texture target for this texture: Zero means - Autodetect.
        win->texturetarget=0;
	// We do not have a preset for texture representation by default:
        win->textureinternalformat=0;
        win->textureexternalformat=0;
        win->textureexternaltype=0;
		// Optional GLSL filtershader: This defaults to zero i.e. no such thing. SCREENMakeTexture.c will
		// initialize this on demand: If a floating point texture is created on a piece of gfx-hardware that
		// doesn't support float-texture filtering. Then a shader is created which can reimplement that for
		// float-textures and the handle is stored in the onscreen window record and in the corresponding
		// texture. The blitters can then optionally bind that shader if filtering is requested.
		win->textureFilterShader=0;
}


/*
	PsychCreateTextureForWindow()
	
	For each onscreen window we create only one OpenGL texture.  Psychtoolbox textures are not really OpenGL textures but are instead blocks of memory 
	holding images in texture format.  To display a Psychtoolbox texture we blit the image memory into the onscree window's texture then
	draw  the texture to the screen.  This results in two copies, first from the image memory to another block of system memory which is the texture,
	then from the texture across the video bus to the video memory.  
	
	It would be better if Psychtoolbox textures were actual textures and we could avoid the first copy, but the combination of TextureRect+ClientTexture+TextureRange
	extensions seems to place an unacceptable limit of 96 MB on the total texture size. Going without the TextureRange extension seems to cause too much variability in 
	the blit times.  Nonetheless it would be worthwhile to create preferences for selectively enableing these extensions and searching more thourougly for a combination
	which gives us higher blit rates than the current dual-copy method, particularly in light of the fact that different video cards may give different behavior.
	
*/
void PsychCreateTextureForWindow(PsychWindowRecordType *win)
{
	GLenum				textureHint;
	double				frameWidth, frameHeight;

	PsychSetGLContext(win);
        // MK: Width and height swapped due to texture rotation trick in PsychBlitTextureToDisplay:
	frameHeight=PsychGetWidthFromRect(win->rect);
	frameWidth=PsychGetHeightFromRect(win->rect);
	
	//create a texture number for this texture
	glGenTextures(1, &win->textureNumber);

	//choose the texture accleration extension
	// FIXME	textureHint= GL_STORAGE_SHARED_APPLE;  //GL_STORAGE_PRIVATE_APPLE, GL_STORAGE_CACHED_APPLE

	win->textureMemorySizeBytes= (unsigned long)(frameWidth * frameHeight * sizeof(GLuint));
	win->textureMemory=malloc(win->textureMemorySizeBytes);
	
	//setup texturing
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, win->textureNumber);


	// glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_EXT, 0, NULL);
	// MK: Leave this untouched, altough i couldn't find a situation where it helps performance at with my code...
	// FIXME         glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_EXT,win->textureMemorySizeBytes, win->textureMemory);

        // MK: Slows things down in current configuration, therefore disabled: glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_STORAGE_HINT_APPLE , textureHint);
        // Not using GL_STORAGE_SHARED_APPLE provided increased reliability of timing and significantly shorter rendering times
        // when testing with a G5-Mac with 1.6Ghz CPU and 256 MB RAM, MacOS-X 10.3.7, using 12 textures of 800x800 pixels each.
        // Rendering times with this code are around 6 msecs, while original PTB 1.0.40 code took 17 ms on average...
        // Can't test this on other machines like G4 or machines with more RAM...
        // -> Sometimes, GL_STORAGE_SHARED_APPLE is faster, but only if texture width and height are divisable by 16 and
        // all used memory is page-aligned and a couple other conditions are met. So... Sometimes you are 20% faster, but most of
        // the time you are 2 to 3 times slower than without this extensions...
        // The "Principle of least surprise" would suggest to disable the extension, because the end-user doesn't
        // expect sudden and random changes in performance of his PTB scripts.
        // Alternatively one could code up different path's depending on if the preconditions are met or not...
        //
        // We could reenable the extension, if wanted, but then the MakeTexture code needs to be modified in a way
        // that will slow down MakeTexture a bit. It's a tradeoff between speed of DrawTexture and speed of MakeTexture.
        //
        // BTW -> Does disabling the extension solve "severe tearing bug" reported in Forum message 3007?
        // Explanation: GL_STORAGE_SHARED_APPLE enables texture fetches over AGP bus via DMA operations and
        // should increase performance. But DMA only triggers when texture width is divisible by 8, in all other
        // cases it's disabled. Bug in message 3007 only happens when texture width is divisible by 8. Could this
        // be a bug in the G4 Laptops graphics hardware (DMA-Engine) or in its OpenGL driver???
        // Would be interesting to find out... --> Solved! Was a OpenGL driver bug - resolved in MacOS-X 10.3.7
	// FIXME	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	// MK: Setting GL_UNPACK_ALIGNMENT == 1 fixes a bug, where textures are drawn incorrectly, if their
	// width or height is not divisible by 4.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, (GLsizei)frameWidth, (GLsizei)frameHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, win->textureMemory);
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);  //glColor does not work while a texture is bound.  
}



void PsychCreateTexture(PsychWindowRecordType *win)
{
        GLenum                          texturetarget;
	GLenum				textureHint;
	double				sourceWidth, sourceHeight;
        GLint                           glinternalFormat, gl_realinternalformat = 0;
        static GLint                    gl_lastrequestedinternalFormat = 0;
	GLint gl_rbits=0, gl_gbits=0, gl_bbits=0, gl_abits=0, gl_lbits=0;
	long screenWidth, screenHeight;
        int twidth, theight;
	void* texmemptr;
	bool recycle = FALSE;
// TEST:
if (win->textureOrientation==0 && renderswap) win->textureOrientation=1;
        // Enable the proper OpenGL rendering context for the window associated with this
        // texture:
	PsychSetGLContext(win);

        // Setup texture-target if not already done:
        PsychDetectTextureTarget(win);
        
	// Assign proper texturetarget for creation:
	texturetarget = PsychGetTextureTarget(win);

        // Check if user requested explicit use of clientstorage + Use of System RAM for
        // storage of textures instead of VRAM caching in order to conserve VRAM memory on
        // low-mem gfx-cards. Enable clientstorage, if so...
        clientstorage = (PsychPrefStateGet_ConserveVRAM() & kPsychDontCacheTextures) ? TRUE : FALSE;
        
	// Create a unique texture handle for this texture:
	// If the texture already has a handle assigned then this means that we shouldn't
	// create and setup a new OpenGL texture from scratch, but bind and recycle the
	// given texture object. Just bind it and update its content via glTexSubImage()... calls.
	// Updating textures is potentially faster than recreating them -> Quicktime movie playback
	// and the Videocapture code et al. will benefit from this...
	if (win->textureNumber == 0) {
		glGenTextures(1, &win->textureNumber);
		recycle = FALSE;
		//printf("CREATING NEW TEX %i\n", win->textureNumber);
	}
	else {
		recycle = TRUE;
		//printf("RECYCLING TEX %i\n", win->textureNumber);
	}

	// Setup texturing:
	glDisable(GL_TEXTURE_2D);
	glEnable(texturetarget);

	// Create & bind a new OpenGL texture object and attach it to our new texhandle:
        glBindTexture(texturetarget, win->textureNumber);

        // Setup texture parameters like optimization, storage format et al.

	// Choose the texture acceleration extension out of GL_STORAGE_PRIVATE_APPLE, GL_STORAGE_CACHED_APPLE, GL_STORAGE_SHARED_APPLE
        // We normally use CACHED storage for caching textures in gfx-cards VRAM for high-perf drawing,
        // but if user explicitely requests client storage for saving VRAM memory, we do so and
        // use SHARED storage in system RAM --> Slower but saves VRAM memory.
	#if PSYCH_SYSTEM == PSYCH_OSX
	    textureHint= (clientstorage) ? GL_STORAGE_SHARED_APPLE : GL_STORAGE_CACHED_APPLE;  
            glTexParameteri(texturetarget, GL_TEXTURE_STORAGE_HINT_APPLE , textureHint);
	    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, (clientstorage) ? GL_TRUE : GL_FALSE);
        #endif

        // Not using GL_STORAGE_SHARED_APPLE provided increased reliability of timing and significantly shorter rendering times
        // when testing with a G5-Mac with 1.6Ghz CPU and 256 MB RAM, MacOS-X 10.3.7, using 12 textures of 800x800 pixels each.
        // Rendering times with this code are around 6 msecs, while original PTB 1.0.40 code took 17 ms on average...
        // Can't test this on other machines like G4 or machines with more RAM...
        // -> Sometimes, GL_STORAGE_SHARED_APPLE is faster, but only if texture width and height are divisable by 16 and
        // all used memory is page-aligned and a couple other conditions are met. So... Sometimes you are 20% faster, but most of
        // the time you are 2 to 3 times slower than without this extensions...
        // The "Principle of least surprise" would suggest to disable the extension, because the end-user doesn't
        // expect sudden and random changes in performance of his PTB scripts.
        // Alternatively one could code up different path's depending on if the preconditions are met or not...
        //
        // We could reenable the extension, if wanted, but then the MakeTexture code needs to be modified in a way
        // that will slow down MakeTexture a bit. It's a tradeoff between speed of DrawTexture and speed of MakeTexture.
        //
        // BTW -> Does disabling the extension solve "severe tearing bug" reported in Forum message 3007?
        // Explanation: GL_STORAGE_SHARED_APPLE enables texture fetches over AGP bus via DMA operations and
        // should increase performance. But DMA only triggers when texture width is divisible by 8, in all other
        // cases it's disabled. Bug in message 3007 only happens when texture width is divisible by 8. Could this
        // be a bug in the G4 Laptops graphics hardware (DMA-Engine) or in its OpenGL driver???
        // Would be interesting to find out...

        // Setting GL_UNPACK_ALIGNMENT == 1 fixes a bug, where textures are drawn incorrectly, if their
        // width or height is not divisible by 4.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        // Override for 4-Byte aligned Quicktime textures created via GWorlds:
        if (win->textureOrientation==3) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        
        // The texture object is ready for use: Assign it our texture data:
        
        // Definition of width and height is swapped due to texture rotation trick, see comments in PsychBlit.....
        if (win->textureOrientation==0 || win->textureOrientation==1) {
            // Transposed case: Optimized for fast MakeTexture from Matlab image matrix.
            // This is true for all calls from MakeTexure.
            sourceHeight=PsychGetWidthFromRect(win->rect);
            sourceWidth=PsychGetHeightFromRect(win->rect);
        }
        else {
            // Non-transposed upright case: This is used for textures created by 'OpenOffscreenWindow'
            // One can directly draw to these textures as rendertargets aka OpenGL framebuffer objects...
            sourceHeight=PsychGetHeightFromRect(win->rect);
            sourceWidth=PsychGetWidthFromRect(win->rect);
        }

        glPixelStorei(GL_UNPACK_ROW_LENGTH, sourceWidth);

        // We used to have different cases for Luminance, Luminance+Alpha, RGB, RGBA.
        // This way we saved texture memory for the source->textureMemory -- Arrays, as well as copy-time
        // in MakeTexture - In theory...
        // Reality is: We always use GL_RGBA8 as internal format, except for pure luminance textures.
        // This obviously wastes storage space for LA and RGB textures, but it is the only mode that is
        // well supported (=fast) on all common gfx-hardware. Only the very latest models of NVidia and ATI
        // are capable of handling the other formats natively in hardware :-(
	if (texturetarget==GL_TEXTURE_2D) {
          // This hardware doesn't support rectangle textures. We create and use power of two
          // textures to emulate rectangle textures...
	  
          // Compute smallest power of two dimension that fits the texture.
	  twidth=1;
	  while (twidth<sourceWidth) twidth*=2;
	  theight=1;
	  while (theight<sourceHeight) theight*=2;
	  // First we only use glTexImage2D with NULL data pointer to create a properly sized empty texture:
	  texmemptr=NULL;
	}
	else {
	  // Hardware supports rectangular textures: Use texture as-is:
	  twidth=sourceWidth;
	  theight=sourceHeight;
	  texmemptr=win->textureMemory;
	}

	// We only execute this pass for really new textures, not for recycled ones:
	if (!recycle) {
	  if (win->textureinternalformat==0) {
	    // Standard path: Derive texture format and such from requested pixeldepth:
	    switch(win->depth) {
            case 8:
	      glinternalFormat=GL_LUMINANCE8;
	      glTexImage2D(texturetarget, 0, glinternalFormat, (GLsizei) twidth, (GLsizei) theight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texmemptr);
	      break;
	      
            case 16:
	      //glinternalFormat=GL_LUMINANCE8_ALPHA8;
	      glinternalFormat=GL_RGBA8;
	      glTexImage2D(texturetarget, 0, glinternalFormat, (GLsizei) twidth, (GLsizei) theight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, texmemptr);
	      break;
	      
            case 24:
	      //glinternalFormat=GL_RGB8;
	      glinternalFormat=GL_RGBA8;
	      glTexImage2D(texturetarget, 0, glinternalFormat, (GLsizei) twidth, (GLsizei) theight, 0, GL_RGB, GL_UNSIGNED_BYTE, texmemptr);
	      break;
	      
            case 32:
	      glinternalFormat=GL_RGBA8;
	      glTexImage2D(texturetarget, 0, glinternalFormat, (GLsizei) twidth, (GLsizei) theight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texmemptr);
	      break;
	    }
	  }
	  else {
	    // Requested internal format and external data representation are explicitely requested: Use it.
	    glTexImage2D(texturetarget, 0, win->textureinternalformat, (GLsizei) twidth, (GLsizei) theight, 0, win->textureexternalformat,
			 win->textureexternaltype, texmemptr);
	    glinternalFormat = win->textureinternalformat;
	  }
	}

	if (texturetarget==GL_TEXTURE_2D || recycle) {
	  // Special setup code for pot2 textures: Fill the empty power of two texture object with content:
	  // We only fill a subrectangle (of sourceWidth x sourceHeight size) with our images content. The
	  // unused border contains all zero == black.
	  // The same path is used for efficient refilling existing textures that are to be recycled:
	  if (win->textureinternalformat==0) {
	    // Standard path: Derive texture format and such from requested pixeldepth:
	    switch(win->depth) {
	    case 8:
	      glTexSubImage2D(texturetarget, 0, 0, 0, (GLsizei)sourceWidth, (GLsizei)sourceHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, win->textureMemory);
	      break;
	    
	    case 16:
	      glTexSubImage2D(texturetarget, 0, 0, 0, (GLsizei)sourceWidth, (GLsizei)sourceHeight, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, win->textureMemory);
	      break;
	    
	    case 24:
	      glTexSubImage2D(texturetarget, 0, 0, 0, (GLsizei)sourceWidth, (GLsizei)sourceHeight, GL_RGB, GL_UNSIGNED_BYTE, win->textureMemory);
	      break;
	    
	    case 32:
	      glTexSubImage2D(texturetarget, 0, 0, 0, (GLsizei)sourceWidth, (GLsizei)sourceHeight, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, win->textureMemory);
	      break;
	    }
	  }
	  else {
	    // Requested internal format and external data representation are explicitely requested: Use it.
	    glTexSubImage2D(texturetarget, 0, 0, 0, (GLsizei)sourceWidth, (GLsizei)sourceHeight, win->textureexternalformat, win->textureexternaltype, win->textureMemory);
	    glinternalFormat = win->textureinternalformat;
	  }
	}
        
        // New internal format requested?
        if (gl_lastrequestedinternalFormat != glinternalFormat && !recycle) {
            // Seems so...
            gl_lastrequestedinternalFormat = glinternalFormat;
            
            // Query real internal format and params...
            glGetTexLevelParameteriv(texturetarget, 0, GL_TEXTURE_INTERNAL_FORMAT, &gl_realinternalformat);
            // If there is a mismatch between wish and reality, report it:
            if (false || gl_realinternalformat != glinternalFormat) {
                // Mismatch between requested format and format that the OpenGL has chosen:
                printf("In glTexImage2D: Mismatch between requested and real format: depth=%i, fcode=%x\n", win->depth, gl_realinternalformat);
                // Request sizes as well:
                glGetTexLevelParameteriv(texturetarget, 0, GL_TEXTURE_RED_SIZE, &gl_rbits);                
                glGetTexLevelParameteriv(texturetarget, 0, GL_TEXTURE_GREEN_SIZE, &gl_gbits);                
                glGetTexLevelParameteriv(texturetarget, 0, GL_TEXTURE_BLUE_SIZE, &gl_bbits);                
                glGetTexLevelParameteriv(texturetarget, 0, GL_TEXTURE_ALPHA_SIZE, &gl_abits);                
                glGetTexLevelParameteriv(texturetarget, 0, GL_TEXTURE_LUMINANCE_SIZE, &gl_lbits);                
                printf("Requested size = %i bits, real size = %i bits.\n", win->depth, gl_rbits + gl_gbits + gl_bbits + gl_abits + gl_lbits); 
                fflush(NULL);
            }
        }
        
        ///////// EXPERIMENTAL SWAPRENDER->COPY-CODE ////////////////
        if (win->textureOrientation == 1 && renderswap) {
            // Turn off alpha-blending - We want the texture "as is", overwriting any previous
            // framebuffer content completely!
            glDisable(GL_BLEND);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glTexParameteri(texturetarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(texturetarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(texturetarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(texturetarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            
            // Render full texture with swapped texture coords into lower-left corner of framebuffer:
            glBegin(GL_QUADS);
            //lower left
            glTexCoord2f((GLfloat) 0, (GLfloat) 0);			//lower left vertex in texture
            glVertex2f((GLfloat) 0, (GLfloat)0);			//upper left vertex in window
            
            //upper left
            glTexCoord2f((GLfloat) sourceWidth, (GLfloat) 0);		//upper left vertex in texture
            glVertex2f((GLfloat) 0, (GLfloat)sourceWidth);		//lower left vertex in window
            
            //upper right
            glTexCoord2f((GLfloat) sourceWidth, (GLfloat) sourceHeight);//upper right vertex in texture
            glVertex2f((GLfloat) sourceHeight, (GLfloat) sourceWidth );	//lower right  vertex in window
            
            //lower right
            glTexCoord2f((GLfloat) 0, (GLfloat) sourceHeight);		//lower right in texture
            glVertex2f((GLfloat) sourceHeight, (GLfloat)0);		//upper right in window
            glEnd();

            // Assign proper dimensions, now that the texture is "reswapped to normal" :)
            sourceHeight=PsychGetHeightFromRect(win->rect);
            sourceWidth=PsychGetWidthFromRect(win->rect);
            
            PsychGetScreenSize(win->screenNumber, &screenWidth, &screenHeight);
            
            // Texture is now displayed/stored in the top-left corner of the framebuffer
            // in its proper (upright 0 deg.) orientation. Let's make a screenshot and
            // store it as a "new" texture into our current texture object - effectively
            // transposing the texture into normal format:                        
            glCopyTexImage2D(texturetarget, 0, glinternalFormat, 0, screenHeight - sourceHeight, sourceWidth, sourceHeight, 0);
            
            // Reenable alpha-blending:
            glEnable(GL_BLEND);

            // Flush the command buffers to enforce start of texture swap operation so that it
            // really runs in parallel to the MakeTexture() C-Code...
            glFlush();
        }
        
        ///////// END OF EXPERIMENTAL SWAPRENDER->COPY-CODE ////////////////
                
        // Free system RAM backing memory buffer, if client storage extensions are not used for this texture:
        if (!clientstorage) {
            if (win->textureMemory && (win->textureMemorySizeBytes > 0)) free(win->textureMemory);
            win->textureMemory=NULL;
            win->textureMemorySizeBytes=0;
        }
        
        // Texture object ready for future use. Unbind it:
	glBindTexture(texturetarget, 0);
        // Reset pixel storage parameter:
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
        // Finished!
        return;
}

/*
	PsychFreeTextureForWindowRecord()
	
	Accepts a window record for either a texture or an onscreen window and deallocte texture resources.
	
*/
void PsychFreeTextureForWindowRecord(PsychWindowRecordType *win)
{
    // Destroy OpenGL texture object for windows that have one:
    if((win->windowType==kPsychSingleBufferOnscreen || win->windowType==kPsychDoubleBufferOnscreen || win->windowType==kPsychTexture) &&
       (win->targetSpecific.contextObject)) {
        // Activate associated OpenGL context:
        PsychSetGLContext(win);
        PsychTestForGLErrors();
        // Call special texture release routine for Movie textures: This routine will
        // check if 'win' is a movie texture and perform the necessary cleanup work, if so:
        PsychFreeMovieTexture(win);
        // If we use client-storage textures, we need to wait for completion of texture operations on the
        // to-be-released client texture buffer before deleting it and freeing the RAM backing buffers. Waiting for
        // completion is done via FinishObjectApple...
        // We need to use glFinish() here. FinishObjectApple would be better (more async operations) but it doesn't
        // work for some strange reason :(
        if ((win->textureMemory) && (win->textureNumber > 0)) glFinish(); // FinishObjectAPPLE(GL_TEXTURE_2D, win->textureNumber);
        // Perform standard OpenGL texture cleanup:
        glDeleteTextures(1, &win->textureNumber);
        PsychTestForGLErrors();
    }

    // Free system RAM backing memory buffer, if any:
    if (win->textureMemory) free(win->textureMemory);
    win->textureMemory=NULL;
    win->textureMemorySizeBytes=0;
    win->textureNumber=0;
    return;
}


void PsychBlitTextureToDisplay(PsychWindowRecordType *source, PsychWindowRecordType *target, double *sourceRect, double *targetRect,
                               double rotationAngle, int filterMode, double globalAlpha)
{
        GLdouble       			 sourceWidth, sourceHeight, tWidth, tHeight;
        GLdouble                         sourceX, sourceY, sourceXEnd, sourceYEnd;
		double                           transX, transY;
        GLenum                           texturetarget;

        // Activate rendering context of target window:
		PsychSetGLContext(target);

        // Setup texture-target if not already done:
        PsychDetectTextureTarget(target);
        
        // Query target for this specific texture:
        texturetarget = PsychGetTextureTarget(source);
        
        // Enable target's framebuffer as current drawingtarget, except if this is a
		// blit operation from a window into itself and the imaging pipe is on:
        if ((source != target) || (target->imagingMode==0)) PsychSetDrawingTarget(target);
		//printf("%i\n", source->textureOrientation);
		
        // This code allows the application of sourceRect, as it is meant to be:
        // CAUTION: This calculation with sourceHeight - xxxx  depends on if GPU texture swapping
        // is on or off!!!!
        // 0 == Transposed as from Matlab image array aka renderswap off. 1 == Renderswapped
        // texture (currently not yet enabled). 2 == Offscreen window in normal orientation.
        if ((source->textureOrientation == 1 && renderswap) || source->textureOrientation == 2) {
            sourceHeight=PsychGetHeightFromRect(source->rect);
            sourceWidth=PsychGetWidthFromRect(source->rect);

            sourceX=sourceRect[kPsychLeft];
            sourceY=sourceHeight - sourceRect[kPsychBottom];
            sourceXEnd=sourceRect[kPsychRight];
            sourceYEnd=sourceHeight - sourceRect[kPsychTop];
        }
        else {
            sourceHeight=PsychGetWidthFromRect(source->rect);
			sourceWidth=PsychGetHeightFromRect(source->rect);
            sourceX=sourceRect[kPsychTop];
            sourceY=sourceRect[kPsychLeft];
            sourceXEnd=sourceRect[kPsychBottom];
            sourceYEnd=sourceRect[kPsychRight];
        }
    
		// Overrides for special cases: Corevideo textures from Quicktime-subsystem or upside-down
        // texture from Quicktime GWorld or Sequence-Grabber...
        if (source->textureOrientation == 3) {
            sourceHeight=PsychGetHeightFromRect(source->rect);
			sourceWidth=PsychGetWidthFromRect(source->rect);
			sourceX=sourceRect[kPsychLeft];
            sourceY=sourceRect[kPsychBottom];
            sourceXEnd=sourceRect[kPsychRight];
            sourceYEnd=sourceRect[kPsychTop];
        }

		// This case can happen with some QT movies, they are upside down in an unusual way:
        if (source->textureOrientation == 4) {
            sourceHeight=PsychGetHeightFromRect(source->rect);
			sourceWidth=PsychGetWidthFromRect(source->rect);
			sourceX=sourceRect[kPsychLeft];
            sourceY=sourceRect[kPsychTop];
            sourceXEnd=sourceRect[kPsychRight];
            sourceYEnd=sourceRect[kPsychBottom];
        }

        // Special case handling for GL_TEXTURE_2D textures. We need to map the
	// absolute texture coordinates (in pixels) to the interval 0.0 - 1.0 where
	// 1.0 == full extent of power of two texture...
	if (texturetarget==GL_TEXTURE_2D) {
	  // Find size of real underlying texture (smallest power of two which is
	  // greater than or equal to the image size:
	  tWidth=1;
	  while (tWidth < sourceWidth) tWidth*=2;
	  tHeight=1;
	  while (tHeight < sourceHeight) tHeight*=2;

	  // Remap texcoords into 0-1 subrange: We subtract 0.5 pixel-units before
	  // mapping to accomodate for roundoff-error in the power-of-two gfx
	  // hardware...
	  // For a good intro into the issue of texture border seams, due to interpolation
	  // problems at texture borders, see:
	  // http://home.planet.nl/~monstrous/skybox.html
	  //sourceX-=0.5f;
	  //sourceY-=0.5f;
	  sourceXEnd-=0.5f;
	  sourceYEnd-=0.5f;
	  // Remap:
	  sourceX=sourceX / tWidth;
	  sourceXEnd=sourceXEnd / tWidth;
	  sourceY=sourceY / tHeight;
	  sourceYEnd=sourceYEnd / tHeight;
	}

        // MK: We need to reenable the proper texturing mode. This fixes bug reported in Forum message 3055,
	// because SCREENDrawText glDisable'd GL_TEXTURE_RECTANGLE_EXT, without this routine reenabling it.
	glDisable(GL_TEXTURE_2D);
	glEnable(texturetarget);
	glBindTexture(texturetarget, source->textureNumber);

        // Select filter-mode for texturing:
        switch (filterMode) {
                case 0: // Nearest-Neighbour filtering:
                    glTexParameteri(texturetarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(texturetarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
                
                case 1: // Bilinear filtering:
                    glTexParameteri(texturetarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(texturetarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;

                case 2: // Linear filtering with nearest neighbour mipmapping: Needs external support to generate mipmaps.
                    glTexParameteri(texturetarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                    glTexParameteri(texturetarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;

                case 3: // Linear filtering with linear mipmapping --> This is full trilinear filtering. Needs external support to generate mipmaps.
                    glTexParameteri(texturetarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(texturetarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }

	// Linear filtering on non-capable hardware via shader emulation?
	if (filterMode > 0 && source->textureFilterShader) {
		// Yes. Bind the shader:
		glUseProgram(source->textureFilterShader);
	}
	
	// Setup texture wrap-mode: We usually default to clamping - the best we can do
	// for the rectangle textures we usually use. Special case is the intentional
	// use of power-of-two textures with a real power-of-two size. In that case we
	// enable wrapping mode to allow for scrolling effects -- useful for drifting
	// gratings.
	if (texturetarget==GL_TEXTURE_2D && tWidth==sourceWidth && tHeight==sourceHeight) {
	  // Special case: Scrollable real power-of-two textures. Enable wrapping.
	  glTexParameteri(texturetarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameteri(texturetarget, GL_TEXTURE_WRAP_T, GL_REPEAT);	  
	}
	else {
	  // Default: Clamp to edge.
	  glTexParameteri(texturetarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  glTexParameteri(texturetarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        // We use GL_MODULATE texture application mode together with the special rectangle color
        // (1,1,1,globalAlpha) -- This way, the alpha blending value is the product of the alpha-
        // value of each texel and the globalAlpha value. --> Can apply global alpha value for
        // global blending without need for a texture alpha-channel...
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f(1, 1, 1, globalAlpha);

        // Apply a rotation transform for rotated drawing:
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        transX=(targetRect[kPsychRight] + targetRect[kPsychLeft]) * 0.5; 
        transY=(targetRect[kPsychTop] + targetRect[kPsychBottom]) * 0.5; 
        
        glTranslated(+transX, +transY, 0);
        glRotated(rotationAngle, 0, 0, 1);
        glTranslated(-transX, -transY, 0);
        // Rotation transform ready...
        
        // matchups for inverted Y coordinate frame (which is inverted?)
        // MK: Texture coordinate assignments have been changed.
        // Explanation: Matlab stores matrices in column-major order, but OpenGL requires
        // textures in row-major order. The old implementation of AWI performed row-column
        // swapping in MakeTexture via C-Code on the CPU. This makes copy-loop implementation
        // complex and creates "Cash trashing" effects on the processor. --> slow MakeTexture performance.
        // Now we store the textures as provided by Matlab, simplifying MakeTexture's implementation,
        // and let the Graphics hardware do the job of "swapping" during rendering, by drawing the texture
        // in some rotated and mirrored order. This is way faster, as the GPU is optimized for such things...
		glBegin(GL_QUADS);
        // Coordinate assignments depend on internal texture orientation...
        // Override for special case: Corevideo texture from Quicktime-subsystem.
        if ((source->textureOrientation == 1 && renderswap) || source->textureOrientation == 2 || source->targetSpecific.QuickTimeGLTexture ||
            source->textureOrientation == 3 || source->textureOrientation == 4) {
	  // NEW CODE: Uses "normal" coordinate assignments, so that the rotation == 0 deg. case
	  // is the fastest case --> Most common orientation has highest performance.
	  //lower left
	  glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceYEnd);
	  glVertex2f((GLfloat)(targetRect[kPsychLeft]), (GLfloat)(targetRect[kPsychTop]));		//upper left vertex in window
            
	  //upper left
	  glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceY);
	  glVertex2f((GLfloat)(targetRect[kPsychLeft]), (GLfloat)(targetRect[kPsychBottom]));		//lower left vertex in window
            
	  //upper right
	  glTexCoord2f((GLfloat)sourceXEnd, (GLfloat)sourceY);
	  glVertex2f((GLfloat)(targetRect[kPsychRight]), (GLfloat)(targetRect[kPsychBottom]) );		//lower right  vertex in window
            
	  //lower right
	  glTexCoord2f((GLfloat)sourceXEnd, (GLfloat)sourceYEnd);
	  glVertex2f((GLfloat)(targetRect[kPsychRight]), (GLfloat)(targetRect[kPsychTop]));		//upper right in window
        }
        else {
	  // OLD CODE: Uses swapped texture coordinates....
	  //lower left
	  glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceY);						//lower left vertex in  window
	  glVertex2f((GLfloat)(targetRect[kPsychLeft]), (GLfloat)(targetRect[kPsychTop]));		//upper left vertex in window
	  
	  //upper left
	  glTexCoord2f((GLfloat)sourceXEnd, (GLfloat)sourceY);					        //upper left vertex in texture
	  glVertex2f((GLfloat)(targetRect[kPsychLeft]), (GLfloat)(targetRect[kPsychBottom]));		//lower left vertex in window
	  
	  //upper right
	  glTexCoord2f((GLfloat)sourceXEnd, (GLfloat)sourceYEnd);					//upper right vertex in texture
	  glVertex2f((GLfloat)(targetRect[kPsychRight]), (GLfloat)(targetRect[kPsychBottom]) );	        //lower right  vertex in window
	  
	  //lower right
	  glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceYEnd);					        //lower right in texture
	  glVertex2f((GLfloat)(targetRect[kPsychRight]), (GLfloat)(targetRect[kPsychTop]));		//upper right in window
        }            
        
        glEnd();
                
        // Undo rotation transform, if any...
        glPopMatrix();

        // Unbind texture:
	glBindTexture(texturetarget, 0);
        glDisable(texturetarget);
        
	if (filterMode > 0 && source->textureFilterShader) {
		// Filtershader was used. Unbind it:
		glUseProgram(0);
	}

        // Finished!
        return;
}

/* PsychGetTextureTarget
 * Returns GLenum with the texture target used for all PTB operations.
 * This way, external code can bind the correct target for given hardware.
 */
GLenum PsychGetTextureTarget(PsychWindowRecordType *win)
{
    if (!PsychIsOnscreenWindow(win)) {
        // No-Op, just to make compiler happy...
        // Currently we have one global setting for all windows.
        // Theoretically one could auto-detect and setup a different
        // texture mapping mode for each onscreen-window. Would be
        // beneficial in the rare case were a user has two different
        // models of gfx-hardware in his machine, each with different
        // texture mapping capabilities. Other than that rare case,
        // we don't win anything...
    }
    
    // If texturetaget field for this texture isn't yet initialized, then
    // init it now from our global setting:
    if (win->texturetarget == 0) {
		// Setup texture-target if not already done:
		PsychDetectTextureTarget(win);

		win->texturetarget = texturetarget;
	}
	
    // Return texturetarget for this window:
    return(win->texturetarget);
}

void PsychMapTexCoord(PsychWindowRecordType *tex, double* tx, double* ty)
{
    GLdouble       sourceWidth, sourceHeight, tWidth, tHeight;
    GLdouble       sourceX, sourceY, sourceXEnd, sourceYEnd;
    GLenum         texturetarget;
    
    if (!PsychIsTexture(tex)) {
        PsychErrorExitMsg(PsychError_user, "Tried to map (x,y) texel position to texture coordinate for something else than a texture!");
    }
    
    if (tx==NULL || ty==NULL) PsychErrorExitMsg(PsychError_internal, "NULL-Ptr passed as tx or ty into PsychMapTexCoord()!!!");
    
    // Perform mapping: This mostly duplicates code in PsychBlitTextureToDisplay():

    // Setup texture-target if not already done:
    PsychDetectTextureTarget(tex);

    // Assign proper texturetarget for mapping:
    texturetarget = PsychGetTextureTarget(tex);
    
    // Basic mapping for rectangular textures:
    // 0 == Transposed as from Matlab image array aka renderswap off. 1 == Renderswapped
    // texture (currently not yet enabled). 2 == Offscreen window in normal orientation.
    if ((tex->textureOrientation == 1 && renderswap) || tex->textureOrientation == 2) {
        sourceHeight=PsychGetHeightFromRect(tex->rect);
        sourceWidth=PsychGetWidthFromRect(tex->rect);
        
        sourceX=*tx;
        sourceY=sourceHeight - *ty;
    }
    else {
        // Transposed, non-renderswapped texture from Matlab:
        sourceHeight=PsychGetWidthFromRect(tex->rect);
        sourceWidth=PsychGetHeightFromRect(tex->rect);
        sourceX=*tx;
        sourceY=*ty;
    }
    
    // Override for special case: Corevideo texture from Quicktime-subsystem or upside-down
    // texture from Quicktime GWorld or Sequence-Grabber...
    if (tex->targetSpecific.QuickTimeGLTexture || tex->textureOrientation == 3) {
        sourceHeight=PsychGetHeightFromRect(tex->rect);
        sourceWidth=PsychGetWidthFromRect(tex->rect);
        sourceX=*tx;
        sourceY=*ty;
    }
    
    // Special case handling for GL_TEXTURE_2D textures. We need to map the
    // absolute texture coordinates (in pixels) to the interval 0.0 - 1.0 where
    // 1.0 == full extent of power of two texture...
    if (texturetarget==GL_TEXTURE_2D) {
        // Find size of real underlying texture (smallest power of two which is
        // greater than or equal to the image size:
        tWidth=1;
        while (tWidth < sourceWidth) tWidth*=2;
        tHeight=1;
        while (tHeight < sourceHeight) tHeight*=2;
        
        // Remap texcoords into 0-1 subrange: We subtract 0.5 pixel-units before
        // mapping to accomodate for roundoff-error in the power-of-two gfx
        // hardware...
        // For a good intro into the issue of texture border seams, due to interpolation
        // problems at texture borders, see:
        // http://home.planet.nl/~monstrous/skybox.html

        sourceX-=0.5f;
        sourceY-=0.5f;

        // Remap:
        sourceX=sourceX / tWidth;
        sourceY=sourceY / tHeight;
    }
    
    // Return mapped coords:
    *tx = sourceX;
    *ty = sourceY;
    
    // Done.
    return;
}


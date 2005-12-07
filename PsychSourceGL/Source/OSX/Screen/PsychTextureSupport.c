/*
	PsychTextureSupport.c
	
	PLATFORMS:	
	
		This is the OS X Core Graphics version.  
				
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu
		Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de

	HISTORY:
	
		3/9/04		awi		Wrote it 
		7/27/04		awi		Removed commented-out code.
		1/4/05		mk              Performance optimizations, some bug-fixes.
		1/13/05		awi		Merged in Mario's changes of 1/4/05 into the psychtoolbox.org master.
		1/23/05		awi		Merged mk's update which adds glEnable(GL_TEXTURE_RECTANGLE_EXT) call.  Fixes DrawText and DrawTexture
                                                interference reported by Frans Cornelissen and others;
                1/30/05         mk              Small bugfix: Now specifying sourceRect in DrawTexture really works as expected.
		5/13/05         mk              Support for rotated drawing of textures. Switched to bilinear texture filtering instead of nearest neighbour.
                6/07/05         mk              Change definition of texture coords from GLint to GLflot -> Allows subpixel accurate texturing.
                7/23/05         mk              New options filterMode and globalAlpha for PsychBlitTextureToDisplay().
                8/11/05         mk              New texture handling: We now use real OpenGL textures instead of our old "pseudo-texture"
                                                implementation. This is *way* faster, e.g., drawing times decrease from 35 ms to 3 ms for big
                                                textures. Some experimental optimizations are implemented but not yet enabled...
                10/11/05        mk              Support for special Quicktime movie textures added.
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
static Boolean renderswap = false;

// If set to true, then the apple client storage extensions are used: I doubt that they have any
// advantage for the current way PTB is used, but it can be useful to conserve VRAM on very
// low-mem gfx cards if Screen('Preference', 'ConserveVRAM') is set appropriately.
static Boolean clientstorage = false;

void PsychInitWindowRecordTextureFields(PsychWindowRecordType *win)
{
	win->textureMemory=NULL;
	win->textureNumber=0;
	win->textureMemorySizeBytes=0;
        // NULL-Out special texture handle for Quicktime Movie textures (see PsychMovieSupport.c)
        win->targetSpecific.QuickTimeGLTexture = NULL;
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
	textureHint= GL_STORAGE_SHARED_APPLE;  //GL_STORAGE_PRIVATE_APPLE, GL_STORAGE_CACHED_APPLE

	win->textureMemorySizeBytes= (unsigned long)(frameWidth * frameHeight * sizeof(GLuint));
        // MK: Allocate texture memory page-aligned...
	win->textureMemory=valloc(win->textureMemorySizeBytes);
	
	//setup texturing
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, win->textureNumber);


	// glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_EXT, 0, NULL);
	// MK: Leave this untouched, altough i couldn't find a situation where it helps performance at with my code...
	glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_EXT,win->textureMemorySizeBytes, win->textureMemory);

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
	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	// MK: Setting GL_UNPACK_ALIGNMENT == 1 fixes a bug, where textures are drawn incorrectly, if their
	// width or height is not divisible by 4.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, (GLsizei)frameWidth, (GLsizei)frameHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, win->textureMemory);
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);  //glColor does not work while a texture is bound.  
}



void PsychCreateTexture(PsychWindowRecordType *win)
{
	GLenum				textureHint;
	double				sourceWidth, sourceHeight;
        GLint                           glinternalFormat, gl_realinternalformat = 0;
        static GLint                    gl_lastrequestedinternalFormat = 0;
        
        // Enable the proper OpenGL rendering context for the window associated with this
        // texture:
	PsychSetGLContext(win);

        // Check if user requested explicit use of clientstorage + Use of System RAM for
        // storage of textures instead of VRAM caching in order to conserve VRAM memory on
        // low-mem gfx-cards. Enable clientstorage, if so...
        clientstorage = (PsychPrefStateGet_ConserveVRAM() & kPsychDontCacheTextures) ? TRUE : FALSE;
        
	// Create a unique texture handle for this texture
	glGenTextures(1, &win->textureNumber);

	// Setup texturing via TEXTURE_RECTANGLE extension:
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_RECTANGLE_EXT);

	// Create & bind a new OpenGL texture object and attach it to our new texhandle:
        glBindTexture(GL_TEXTURE_RECTANGLE_EXT, win->textureNumber);

        // Setup texture parameters like optimization, storage format et al.

	// Choose the texture acceleration extension out of GL_STORAGE_PRIVATE_APPLE, GL_STORAGE_CACHED_APPLE, GL_STORAGE_SHARED_APPLE
        // We normally use CACHED storage for caching textures in gfx-cards VRAM for high-perf drawing,
        // but if user explicitely requests client storage for saving VRAM memory, we do so and
        // use SHARED storage in system RAM --> Slower but saves VRAM memory.
	textureHint= (clientstorage) ? GL_STORAGE_SHARED_APPLE : GL_STORAGE_CACHED_APPLE;  
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_STORAGE_HINT_APPLE , textureHint);

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

        // Explicitely disable Apple's Client storage extensions. For now they are not really useful to us.
        glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, (clientstorage) ? GL_TRUE : GL_FALSE);

        // Setting GL_UNPACK_ALIGNMENT == 1 fixes a bug, where textures are drawn incorrectly, if their
        // width or height is not divisible by 4.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        // The texture object is ready for use: Assign it our texture data:
        
        // Definition of width and height is swapped due to texture rotation trick, see comments in PsychBlit.....
	sourceHeight=PsychGetWidthFromRect(win->rect);
	sourceWidth=PsychGetHeightFromRect(win->rect);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, sourceWidth);

        // We have different cases for Luminance, Luminance+Alpha, RGB, RGBA.
        // This way we save texture memory for the source->textureMemory -- Arrays, as well as copy-time
        // in MakeTexture.
        // MK: Correction - We always use GL_RGBA8 as internal format, except for pure luminance textures.
        // This obviously wastes storage space for LA and RGB textures, but it is the only mode that is
        // well supported (=fast) on all common gfx-hardware. Only the very latest models of NVidia and ATI
        // are capable of handling the other formats natively in hardware :-(
        switch(win->depth) {
            case 8:
                glinternalFormat=GL_LUMINANCE8;
                glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, glinternalFormat, (GLsizei)sourceWidth, (GLsizei)sourceHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, win->textureMemory);
                break;
                
            case 16:
                //glinternalFormat=GL_LUMINANCE8_ALPHA8;
                glinternalFormat=GL_RGBA8;
                glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, glinternalFormat, (GLsizei)sourceWidth, (GLsizei)sourceHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, win->textureMemory);
                break;
                
            case 24:
                //glinternalFormat=GL_RGB8;
                glinternalFormat=GL_RGBA8;
                glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, glinternalFormat, (GLsizei)sourceWidth, (GLsizei)sourceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, win->textureMemory);
                break;
                
            case 32:
                glinternalFormat=GL_RGBA8;
                glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, glinternalFormat, (GLsizei)sourceWidth, (GLsizei)sourceHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, win->textureMemory);
                break;
        }
        
        // New internal format requested?
        if (gl_lastrequestedinternalFormat != glinternalFormat) {
            // Seems so...
            gl_lastrequestedinternalFormat = glinternalFormat;
            
            // Query real internal format and params...
            glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_EXT, 0, GL_TEXTURE_INTERNAL_FORMAT, &gl_realinternalformat);
            // If there is a mismatch between wish and reality, report it:
            if (false || gl_realinternalformat != glinternalFormat) {
                // Mismatch between requested format and format that the OpenGL has chosen:
                printf("In glTexImage2D: Mismatch between requested and real format: depth=%i, fcode=%x\n", win->depth, gl_realinternalformat);
                // Request sizes as well:
                GLint gl_rbits=0, gl_gbits=0, gl_bbits=0, gl_abits=0, gl_lbits=0;
                glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_EXT, 0, GL_TEXTURE_RED_SIZE, &gl_rbits);                
                glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_EXT, 0, GL_TEXTURE_GREEN_SIZE, &gl_gbits);                
                glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_EXT, 0, GL_TEXTURE_BLUE_SIZE, &gl_bbits);                
                glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_EXT, 0, GL_TEXTURE_ALPHA_SIZE, &gl_abits);                
                glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_EXT, 0, GL_TEXTURE_LUMINANCE_SIZE, &gl_lbits);                
                printf("Requested size = %i bits, real size = %i bits.\n", win->depth, gl_rbits + gl_gbits + gl_bbits + gl_abits + gl_lbits); 
                fflush(NULL);
            }
        }
        
        ///////// EXPERIMENTAL SWAPRENDER->COPY-CODE ////////////////
        
        if (renderswap) {
            // Turn off alpha-blending - We want the texture "as is", overwriting any previous
            // framebuffer content completely!
            glDisable(GL_BLEND);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            
            // Render full texture with swapped texture coords into lower-left corner of framebuffer:
            glBegin(GL_QUADS);
            //lower left
            glTexCoord2f((GLfloat) 0, (GLfloat) 0);									//lower left vertex in texture
            glVertex2f((GLfloat) 0, (GLfloat)0);			//upper left vertex in window
            
            //upper left
            glTexCoord2f((GLfloat) sourceWidth, (GLfloat) 0);										//upper left vertex in texture
            glVertex2f((GLfloat) 0, (GLfloat)sourceWidth);			//lower left vertex in window
            
            //upper right
            glTexCoord2f((GLfloat) sourceWidth, (GLfloat) sourceHeight);									//upper right vertex in texture
            glVertex2f((GLfloat) sourceHeight, (GLfloat) sourceWidth );		//lower right  vertex in window
            
            //lower right
            glTexCoord2f((GLfloat) 0, (GLfloat) sourceHeight);										//lower right in texture
            glVertex2f((GLfloat) sourceHeight, (GLfloat)0);			//upper right in window
            glEnd();

            // Assign proper dimensions, now that the texture is "reswapped to normal" :)
            sourceHeight=PsychGetHeightFromRect(win->rect);
            sourceWidth=PsychGetWidthFromRect(win->rect);
            
            long screenWidth, screenHeight;
            PsychGetScreenSize(win->screenNumber, &screenWidth, &screenHeight);
            
            // Texture is now displayed/stored in the top-left corner of the framebuffer
            // in its proper (upright 0 deg.) orientation. Let's make a screenshot and
            // store it as a "new" texture into our current texture object - effectively
            // transposing the texture into normal format:                        
            glCopyTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, glinternalFormat, 0, screenHeight - sourceHeight, sourceWidth, sourceHeight, 0);
            
            // Reenable alpha-blending:
            glEnable(GL_BLEND);

            // Flush the command buffers to enforce start of texture swap operation so that it
            // really runs in parallel to the MakeTexture() C-Code...
            glFlush();
        }
        
        ///////// END OF EXPERIMENTAL SWAPRENDER->COPY-CODE ////////////////
                
        // Free system RAM backing memory buffer, if client storage extensions are not used for this texture:
        if (!clientstorage) {
            if (win->textureMemory) free(win->textureMemory);
            win->textureMemory=NULL;
            win->textureMemorySizeBytes=0;
        }
        // Texture object ready for future use. Unbind it:
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
        // Reset pixel storage parameter:
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        
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
        GLint				 sourceWidth, sourceHeight;
        GLdouble                         sourceX, sourceY, sourceXEnd, sourceYEnd;

        // Activate rendering context of target window:
	PsychSetGLContext(target);

        // This code allows the application of sourceRect, as it is meant to be:
        // CAUTION: This calculation with sourceHeight - xxxx  depends on if GPU texture swapping
        // is on or off!!!!
        if (renderswap) {
            sourceHeight=PsychGetHeightFromRect(source->rect);
            sourceX=sourceRect[kPsychLeft];
            sourceY=sourceHeight - sourceRect[kPsychBottom];
            sourceXEnd=sourceRect[kPsychRight];
            sourceYEnd=sourceHeight - sourceRect[kPsychTop];
        }
        else {
            sourceHeight=PsychGetWidthFromRect(source->rect);
            sourceX=sourceRect[kPsychTop];
            sourceY=sourceRect[kPsychLeft];
            sourceXEnd=sourceRect[kPsychBottom];
            sourceYEnd=sourceRect[kPsychRight];
        }
    
	// Override for special case: Corevideo texture from Quicktime-subsystem.
        if (source->targetSpecific.QuickTimeGLTexture) {
            sourceHeight=PsychGetHeightFromRect(source->rect);
            sourceX=sourceRect[kPsychLeft];
            sourceY=sourceRect[kPsychBottom];
            sourceXEnd=sourceRect[kPsychRight];
            sourceYEnd=sourceRect[kPsychTop];
        }
        
        // MK: We need to reenable the proper texturing mode. This fixes bug reported in Forum message 3055,
	// because SCREENDrawText glDisable'd GL_TEXTURE_RECTANGLE_EXT, without this routine reenabling it.
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, source->textureNumber);

        // Select filter-mode for texturing:
        switch (filterMode) {
                case 0: // Nearest-Neighbour filtering:
                    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
                
                case 1: // Bilinear filtering:
                    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }
                        
        // Setup texture wrap-mode:
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        // We use GL_MODULATE texture application mode together with the special rectangle color
        // (1,1,1,globalAlpha) -- This way, the alpha blending value is the product of the alpha-
        // value of each texel and the globalAlpha value. --> Can apply global alpha value for
        // global blending without need for a texture alpha-channel...
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f(1, 1, 1, globalAlpha);

        // Apply a rotation transform for rotated drawing:
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        double transX=(targetRect[kPsychRight] + targetRect[kPsychLeft]) * 0.5; 
        double transY=(targetRect[kPsychTop] + targetRect[kPsychBottom]) * 0.5; 
        
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
        if (renderswap || source->targetSpecific.QuickTimeGLTexture) {
            // NEW CODE: Uses "normal" coordinate assignments, so that the rotation == 0 deg. case
            // is the fastest case --> Most common orientation has highest performance.
            //lower left
            glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceYEnd);						//lower left vertex in                                                                                                                                                         //glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceY);									//lower left vertex in texture
            glVertex2f((GLfloat)(targetRect[kPsychLeft]), (GLfloat)(targetRect[kPsychTop]));			//upper left vertex in window
            
            //upper left
            glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceY);
            glVertex2f((GLfloat)(targetRect[kPsychLeft]), (GLfloat)(targetRect[kPsychBottom]));			//lower left vertex in window
            
            //upper right
            glTexCoord2f((GLfloat)sourceXEnd, (GLfloat)sourceY);
            glVertex2f((GLfloat)(targetRect[kPsychRight]), (GLfloat)(targetRect[kPsychBottom]) );		//lower right  vertex in window
            
            //lower right
            glTexCoord2f((GLfloat)sourceXEnd, (GLfloat)sourceYEnd);
            glVertex2f((GLfloat)(targetRect[kPsychRight]), (GLfloat)(targetRect[kPsychTop]));			//upper right in window
        }
        else {
            // OLD CODE: Uses swapped texture coordinates....
            //lower left
            glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceY);											//lower left vertex in                                                                                                                                                         //glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceY);									//lower left vertex in texture
            glVertex2f((GLfloat)(targetRect[kPsychLeft]), (GLfloat)(targetRect[kPsychTop]));			//upper left vertex in window
            
            //upper left
            glTexCoord2f((GLfloat)sourceXEnd, (GLfloat)sourceY);										//upper left vertex in texture
            glVertex2f((GLfloat)(targetRect[kPsychLeft]), (GLfloat)(targetRect[kPsychBottom]));			//lower left vertex in window
            
            //upper right
            glTexCoord2f((GLfloat)sourceXEnd, (GLfloat)sourceYEnd);						//upper right vertex in texture
            glVertex2f((GLfloat)(targetRect[kPsychRight]), (GLfloat)(targetRect[kPsychBottom]) );		//lower right  vertex in window
            
            //lower right
            glTexCoord2f((GLfloat)sourceX, (GLfloat)sourceYEnd);										//lower right in texture
            glVertex2f((GLfloat)(targetRect[kPsychRight]), (GLfloat)(targetRect[kPsychTop]));			//upper right in window
        }            
        
        glEnd();
                
        // Undo rotation transform, if any...
        glPopMatrix();

        // Unbind texture:
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
        
        // Finished!
        return;
}


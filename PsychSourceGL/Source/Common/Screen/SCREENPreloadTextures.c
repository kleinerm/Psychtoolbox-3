/*
	SCREENPreloadTextures
 
        Try to prefetch textures into the gfx-cards VRAM memory, so they
        can be drawn with minimum delay.
	
        AUTHORS:
 
		mario.kleiner@tuebingen.mpg.de                  mk		Mario Kleiner
 
	PLATFORMS:	
	
		Should compile and work on all platforms.  
 
	HISTORY:
	
		mm/dd/yy   
 
		12/04/05	mk		Created  							
		
	TO DO:
  
 */


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[resident [texidresident]] = Screen('PreloadTextures', windowPtr [, texids]);";
//                                                                                1          2
static char synopsisString[] = 
"Try to preload textures into VRAM to facilitate fast drawing. This method tries "
"to upload textures into the local (and fast) VRAM of your graphics hardware before "
"start of trial. This can reduce texture drawing time by avoiding the upload delay. "
"\"windowPtr\" Handle for onscreen window whose textures should be preloaded. "
"\"texids\" is a vector which contains the texture handles of all textures which "
"should be preloaded into VRAM. If no vector is given, PTB tries to preload all "
"textures into VRAM. "
"The return value 'resident' tells you, if all requested textures could be preloaded. A value of 1 "
"means full success. The 'texidresident' vector tells you for each texture, if that "
"specific texture could be preloaded. Preloading requested textures can fail if your gfx-hardware "
"has an insufficient amount of free VRAM memory. ";

static char seeAlsoString[] = "MakeTexture DrawTexture GetMovieImage";	 

PsychError SCREENPreloadTextures(void)  
{	
	PsychWindowRecordType                   *windowRecord, *texwin;
	psych_bool                                 isArgThere;
        int                                     *texhandles;
        PsychWindowRecordType                   **windowRecordArray;        
        int                                     i, n, numWindows, myhandle; 
        double                                  *success;
        psych_bool*                                residency;
        GLuint*                                 texids;
        GLboolean*                              texresident;
        psych_bool                                 failed = false;
        GLclampf                                maxprio = 1.0f;
        GLenum                                  target;

	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(2));        //The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1));    //The minimum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(2));       //The maximum number of outputs
	
	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(1, kPsychArgRequired, &windowRecord);
		
	// Get optional texids vector:
	isArgThere = PsychIsArgPresent(PsychArgIn, 2);
        PsychAllocInIntegerListArg(2, FALSE, &n, &texhandles);
        if (n < 1) isArgThere=FALSE;
        
        // Enable this windowRecords framebuffer as current drawingtarget:
        PsychSetDrawingTarget(windowRecord);

		// Disable shader:
		PsychSetShader(windowRecord, 0);
	

        glDisable(GL_TEXTURE_2D);

	// Fetch global texturing mode:
	target=PsychGetTextureTarget(windowRecord);

        glEnable(target);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor4f(0, 0, 0, 0);
	// Setup identity modelview matrix:
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);            

        // Process vector of all texids for all requested textures:
        if (!isArgThere) {
            // No handles provided: In this case, we preload all textures:
            n=0;
            for(i=0; i<numWindows; i++) {                
                if (windowRecordArray[i]->windowType==kPsychTexture) {
                    n++;
                    // Prioritize this texture:
                    glPrioritizeTextures(1, (GLuint*) &(windowRecordArray[i]->textureNumber), &maxprio);
                    // Bind this texture:
                    glBindTexture(target, windowRecordArray[i]->textureNumber);
                    // Render a single textured point, thereby enforcing a texture upload:
                    glBegin(GL_QUADS);
                    glTexCoord2f(0,0); glVertex2i(10,10);
                    glTexCoord2f(0,1); glVertex2i(10,11);
                    glTexCoord2f(1,1); glVertex2i(11,11);
                    glTexCoord2f(1,0); glVertex2i(11,10);                    
                    glEnd();
                }
            }
            
            texids = (GLuint*) PsychMallocTemp(sizeof(GLuint) * n);
            texresident = (GLboolean*) PsychMallocTemp(sizeof(GLboolean) * n);

            n=0;
            for(i=0; i<numWindows; i++) {                
                if (windowRecordArray[i]->windowType==kPsychTexture) {
                    texids[n] = (GLuint) windowRecordArray[i]->textureNumber;
                    n++;
                }
            }
        }
        else {
            // Vector with texture handles provided: Just preload them.
            texids = (GLuint*) PsychMallocTemp(sizeof(GLuint) * n);
            texresident = (GLboolean*) PsychMallocTemp(sizeof(GLboolean) * n);
            myhandle=0;
            for (i=0; i<n; i++) {
                myhandle = texhandles[i];
                texwin = NULL;
                if (IsWindowIndex(myhandle)) FindWindowRecord(myhandle, &texwin);
                if (texwin && texwin->windowType==kPsychTexture) {
                    // Prioritize this texture:
                    glPrioritizeTextures(1, (GLuint*) &(texwin->textureNumber), &maxprio);
                    // Bind this texture:
                    glBindTexture(target, texwin->textureNumber);
                    // Render a single textured point, thereby enforcing a texture upload:
                    glBegin(GL_QUADS);
                    glTexCoord2f(0,0); glVertex2i(10,10);
                    glTexCoord2f(0,1); glVertex2i(10,11);
                    glTexCoord2f(1,1); glVertex2i(11,11);
                    glTexCoord2f(1,0); glVertex2i(11,10);                    
                    glEnd();
                    texids[i] = (GLuint) texwin->textureNumber;
                }
                else {
                    // This handle is invalid or at least no texture handle:
                    printf("PTB-ERROR! Screen('PreloadTextures'): Entry %i of texture handle vector (handle %i) is not a texture handle!\n",
                           i, myhandle);
                    failed = true;
                }
            }
        }
        
        // Restore old matrix from backup copy, undoing the global translation:
        glPopMatrix();
        // Disable texture engine:
        glDisable(GL_TEXTURE_2D);
        glDisable(target);

        // Wait for prefetch completion:
        glFinish();
        
        // We don't need these anymore:
        PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);
        
        if (failed) {
            PsychErrorExitMsg(PsychError_user, "At least one texture handle in texids-vector was invalid! Aborted.");
        }
        
        // Query residency state of all preloaded textures:
        success = NULL;
        PsychAllocOutDoubleArg(1, FALSE, &success);
        *success = (double) glAreTexturesResident(n, texids, texresident);
        
        // Sync pipe again, just to be safe...
        glFinish();
        
        // Count them and copy them into output vector:
        PsychAllocOutBooleanMatArg(2, FALSE, n, 1, 1, (PsychNativeBooleanType **) &residency);
        
        for (i=0; i<n; i++) {
            residency[i] = (psych_bool) ((*success) ? TRUE : texresident[i]);
        }
        
        PsychTestForGLErrors();
        
 	// Done. Our PsychMallocTemp'ed arrays will be auto-released...
	return(PsychError_none);
}


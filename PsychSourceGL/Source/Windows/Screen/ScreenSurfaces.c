/*
  ScreenSurfaces.cpp		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	All
    

  HISTORY:
  10/24/02  awi		Created.  
 
  
  
  TO DO:
  
*/

#include "Screen.h"



/*

//file local variables:
static boolean isSurfaceLocked=FALSE;

//file local functions
Uint32 GetPixelFromSurface(int x, int y, SDL_Surface *surface);

	
Uint32 GetPixelFromSurface(int x, int y, SDL_Surface *surface)
{
    int bpp = surface->format->BytesPerPixel;
    //Here p is the address to the pixel we want to retrieve
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       //shouldn't happen, but avoids warnings    }
}


//  Uint8  Rshift, Gshift, Bshift, Ashift;
//  Uint32 Rmask, Gmask, Bmask, Amask;
Uint8 GetRedPixelFromSurface(int x, int y, SDL_Surface *surface)
{
	Uint32 pixel;
	
	pixel=GetPixelFromSurface(x, y, surface);
	return((Uint8)(surface->format->Rmask & pixel)>>surface->format->Rshift);
}

Uint8 GetGreenPixelFromSurface(int x, int y, SDL_Surface *surface)
{
	Uint32 pixel;
	
	pixel=GetPixelFromSurface(x, y, surface);
	return((Uint8)(surface->format->Gmask & pixel)>>surface->format->Gshift);
}

Uint8 GetBluePixelFromSurface(int x, int y, SDL_Surface *surface)
{
	Uint32 pixel;
	
	pixel=GetPixelFromSurface(x, y, surface);
	return((Uint8)(surface->format->Bmask & pixel)>>surface->format->Bshift);
}

Uint8 GetAlphaPixelFromSurface(int x, int y, SDL_Surface *surface)
{
	Uint32 pixel;
	
	pixel=GetPixelFromSurface(x, y, surface);
	return((Uint8)(surface->format->Amask & pixel)>>surface->format->Ashift);
}


Uint8 GetCIndexPixelFromSurface(int x, int y, SDL_Surface *surface)
{
	
	return((Uint8)GetPixelFromSurface(x, y, surface));
}
*/
/*
	ScreenLockSurface()
	
	Locks the screen surface if the hardware requires it.
	
	An incomplete asynchronous blit can cause the lock to fail, 
	in which case the best thing to do is to loop over locking
	call until we get a lock.  We could consider building that 
	into this function with a time limit, once microsecond timing
	is integrated into the toolbox, because we want to drop out
	of that loop with an error if we don't get a lock in time. 
	
	This should detect attemps to relock surfaces and signal
	an error.  Maybe we should deal only with the window structure
	and not the surface structure and include a lock flag 
	within the window structure. 
  
*/
/*
PsychError PsychLockSurface(SDL_Surface *surface)
{
	if(SDL_MUSTLOCK(surface)){
		if(SDL_LockSurface(surface)==-1)
			return(PsychError_SurfaceLockFailed);
	}
	return(PsychError_none);
}

PsychError PsychUnlockSurface(SDL_Surface *surface)
{
	if(SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
	return(PsychError_none);
}



Uint8 ScreenGetDisplayPixelSize(int displayNum)
{
 	const SDL_VideoInfo* info = NULL;

    //Until SDL supports more than one display we ignore the display number argument.
	displayNum;
	info = SDL_GetVideoInfo();
	if(!info)
		PsychErrorExitMsg(PsychError_SDL, "SDL_GetVideoInfo() did not return a result");
	return(info->vfmt->BitsPerPixel);
}

Uint8 ScreenGetSurfacePixelSize(SDL_Surface *surface)
{
	return(surface->format->BitsPerPixel);
}



	
*/

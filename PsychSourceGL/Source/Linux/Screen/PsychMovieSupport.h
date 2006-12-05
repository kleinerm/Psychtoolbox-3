/*
	PsychToolbox3/Source/OSX/Screen/PsychMovieSupport.h
	
	PLATFORMS:	This is the OS X Core Graphics version.  
				
	AUTHORS:

	HISTORY:
        10/31/05    mk      Wrote it.
	DESCRIPTION:
	
	Psychtoolbox functions for dealing with movies.
        

*/




//include once
#ifndef PSYCH_IS_INCLUDED_PsychMovieSupport
#define PSYCH_IS_INCLUDED_PsychMovieSupport

#include "Screen.h"

void PsychMovieInit(void);
void PsychCreateMovie(PsychWindowRecordType *win, const char* moviename, int* moviehandle);
void PsychGetMovieInfos(int moviehandle, int* width, int* height, int* framecount, double* durationsecs, double* framerate, int* nrdroppedframes);
void PsychDeleteMovie(int moviehandle);
void PsychDeleteAllMovies(void);
int PsychGetTextureFromMovie(PsychWindowRecordType *win, int moviehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp);
int PsychPlaybackRate(int moviehandle, double playbackrate, int loop, double soundvolume);
void PsychExitMovies(void);
double PsychGetMovieTimeIndex(int moviehandle);
double PsychSetMovieTimeIndex(int moviehandle, double timeindex);

//end include once
#endif




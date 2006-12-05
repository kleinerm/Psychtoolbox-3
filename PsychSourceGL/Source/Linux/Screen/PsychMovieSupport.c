/*
	PsychMovieSupport.c
	
	PLATFORMS:	
	
		This is the Linux version. It doesn't do anything yet.
				
	AUTHORS:
	
		Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de

	HISTORY:
	
        DESCRIPTION:
	
		Psychtoolbox functions for dealing with movies.
	
	NOTES:

*/

#include "Screen.h"

/*
 *     PsychMovieInit() -- Initialize movie subsystem.
 *     This routine is called by Screen's RegisterProject.c PsychModuleInit()
 *     routine at Screen load-time. It clears out the movieRecordBANK to
 *     bring the subsystem into a clean initial state.
 */
void PsychMovieInit(void)
{
    return;
}

/** Internal helper function: Returns fps rate of movie and optionally
 *  the total number of video frames in the movie. Framecount is determined
 *  by stepping through the whole movie and counting frames. This can take
 *  significant time on big movie files.
 *
 *  Always returns fps as a double. Only counts and returns full framecount,
 *  if *nrframes is non-NULL.
 */
//double PsychDetermineMovieFramecountAndFps(Movie theMovie, int* nrframes)
//{
//  return(0);
//}

/*
 *      PsychCreateMovie() -- Create a movie object.
 *
 *      This function tries to open a Quicktime-Moviefile and create an
 *      associated movie object for it.
 *
 *      win = Pointer to window record of associated onscreen window.
 *      moviename = char* with the name of the moviefile.
 *      moviehandle = handle to the new movie.
 */
void PsychCreateMovie(PsychWindowRecordType *win, const char* moviename, int* moviehandle)
{
  PsychErrorExitMsg(PsychError_unimplemented, "Sorry, Movie playback support not yet available on GNU/Linux");
  return;
}

/*
 *  PsychGetMovieInfo() - Return basic information about a movie.
 *
 *  framecount = Total number of video frames in the movie, determined by counting.
 *  durationsecs = Total playback duration of the movie, in seconds.
 *  framerate = Estimated video playback framerate in frames per second (fps).
 *  width = Width of movie images in pixels.
 *  height = Height of movie images in pixels.
 *  nrdroppedframes = Total count of videoframes that had to be dropped during last movie playback,
 *                    in order to keep the movie synced with the realtime clock.
 */
void PsychGetMovieInfos(int moviehandle, int* width, int* height, int* framecount, double* durationsecs, double* framerate, int* nrdroppedframes)
{
  PsychErrorExitMsg(PsychError_unimplemented, "Sorry, Movie playback support not yet available on GNU/Linux");
  return;
}

/*
 *  PsychDeleteMovie() -- Delete a movie object and release all associated ressources.
 */
void PsychDeleteMovie(int moviehandle)
{
  PsychErrorExitMsg(PsychError_unimplemented, "Sorry, Movie playback support not yet available on GNU/Linux");
  return;
}

/*
 *  PsychDeleteAllMovies() -- Delete all movie objects and release all associated ressources.
 */
void PsychDeleteAllMovies(void)
{
  return;
}

/*
 *  PsychGetTextureFromMovie() -- Create an OpenGL texture map from a specific videoframe from given movie object.
 *
 *  win = Window pointer of onscreen window for which a OpenGL texture should be created.
 *  moviehandle = Handle to the movie object.
 *  checkForImage = true == Just check if new image available, false == really retrieve the image, blocking if necessary.
 *  timeindex = When not in playback mode, this allows specification of a requested frame by presentation time.
 *              If set to -1, or if in realtime playback mode, this parameter is ignored and the next video frame is returned.
 *  out_texture = Pointer to the Psychtoolbox texture-record where the new texture should be stored.
 *  presentation_timestamp = A ptr to a double variable, where the presentation timestamp of the returned frame should be stored.
 *
 *  Returns true (1) on success, false (0) if no new image available, -1 if no new image available and there won't be any in future.
 */
int PsychGetTextureFromMovie(PsychWindowRecordType *win, int moviehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp)
{
  PsychErrorExitMsg(PsychError_unimplemented, "Sorry, Movie playback support not yet available on GNU/Linux");
  return(TRUE);
}

/*
 *  PsychFreeMovieTexture() - Release texture memory for a Quicktime texture.
 *
 *  This routine is called by PsychDeleteTexture() in PsychTextureSupport.c
 *  It performs the special cleanup necessary for Quicktime created textures.
 *
 */
void PsychFreeMovieTexture(PsychWindowRecordType *win)
{
  return;
}

/*
 *  PsychPlaybackRate() - Start- and stop movieplayback, set playback parameters.
 *
 *  moviehandle = Movie to start-/stop.
 *  playbackrate = zero == Stop playback, non-zero == Play movie with spec. rate,
 *                 e.g., 1 = forward, 2 = double speed forward, -1 = backward, ...
 *  loop = 0 = Play once. 1 = Loop, aka rewind at end of movie and restart.
 *  soundvolume = 0 == Mute sound playback, between 0.0 and 1.0 == Set volume to 0 - 100 %.
 *  Returns Number of dropped frames to keep playback in sync.
 */
int PsychPlaybackRate(int moviehandle, double playbackrate, int loop, double soundvolume)
{
  return(0);
}

/*
 *  void PsychExitMovies() - Shutdown handler.
 *
 *  This routine is called by Screen('CloseAll') and on clear Screen time to
 *  do final cleanup. It deletes all Quicktime textures and releases all Quicktime
 *  movie objects. Then it shuts down the Quicktime subsystem.
 *
 */
void PsychExitMovies(void)
{
  return;
}

/*
 *  PsychGetMovieTimeIndex()  -- Return current playback time of movie.
 */
double PsychGetMovieTimeIndex(int moviehandle)
{
  return(0.0);
}

/*
 *  PsychSetMovieTimeIndex()  -- Set current playback time of movie.
 */
double PsychSetMovieTimeIndex(int moviehandle, double timeindex)
{
  return(0.0);
}


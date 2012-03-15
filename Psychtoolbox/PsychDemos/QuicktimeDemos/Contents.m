% Psychtoolbox:PsychDemos:QuicktimeDemos
%
% Demos demonstrating use of the Quicktime and GStreamer movie playback
% functions. Quicktime is the default engine on OS/X and Windows. GStreamer
% is the default engine on Linux and optionally available for Windows.
%
% All demos work with the GStreamer engine (even the ones with OSX in their name).
% Most demos also work with Quicktime, although Quicktime is deprecated.
%
% Read the help for the corresponding Screen subfunctions for infos.
%
% SimpleMovieDemo              -- Most simplistic demo on how to play a movie.
% PlayMoviesDemoOSX            -- Show simple playback of one movie with sound at a time.
% PlayDualMoviesDemoOSX        -- Same as PlayMoviesDemoOSX, but play two movies in parallel.
% PlayInterlacedMovieDemo      -- Simple demo on how to use the GLSL realtime video deinterlacer.
% PlayMoviesWithoutGapDemoOSX  -- Play one movie while opening another one for gap-less playback
% PlayMoviesWithoutGapDemo2    -- GStreamer only: Optimized version of gap-less playback.
%                                 movie in the background to reduce gaps between movies.
% DetectionRTInVideoDemoOSX    -- How to collect reaction times in response to detection of some
%                                 event in a presented movie file. Takes care to get timing right.
% LoadMovieIntoTexturesDemoOSX -- Quickly load a movie into a stack of textures for quick playback
%                                 with arbitrary speed and order.

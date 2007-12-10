function rval = kPsychNeedHalfHeightWindow
% rval = kPsychNeedHalfHeightWindow
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to request use of half-height windows. These
% onscreen windows are treated by all drawing functions (and geometry query
% functions like Screen('Rect', window); or Screen('WindowSize', window);
% as if they only have half the height in pixels. This is a trick to handle
% line interleaved stereo modes or other special display modes, where useable
% vertical drawing area is only half the physical monitor resolution.
%
% Usually user scripts won't pass this flag, but only special setup routines
% that are part of the Psychtoolbox itself.
%

rval = 8192;
return

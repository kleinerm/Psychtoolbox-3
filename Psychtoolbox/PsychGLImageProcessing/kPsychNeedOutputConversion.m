function rval = kPsychNeedOutputConversion
% rval = kPsychNeedOutputConversion
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to request support for special output
% conversion operations needed to driver special output devices.
%
% If requested, Psychtoolbox will configure itself to support special
% OpenGL shader plugins for all kinds of on-the-fly conversions of final
% image data into the special data format needed by special output devices.
%
% FIXME: More info...
%
% It is part of the Psychtoolbox imaging pipeline and only works on modern
% graphics hardware, e.g., ATI Radeon 9600 and later, NVidia GeforceFX 5000
% and later. Check the www.psychtoolbox.org Wiki for graphics hardware
% recommendations and for a description of PTB's image processing
% capabilities.
rval = 4;
return
